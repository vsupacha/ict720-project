from flask import Flask, request, jsonify
import sqlite3

app = Flask(__name__)
app.config["TEMPLATES_AUTO_RELOAD"] = True

@app.route('/register', methods=["POST"])
def register():
    payload = request.get_json(force=True)
    name = payload['name']
    conn = sqlite3.connect('example.db')
    c = conn.cursor()
    c.execute('INSERT INTO records(name) VALUES(?)', (name,))
    conn.commit()
    conn.close()
    resp = {'status':'OK'}
    return jsonify(resp)

@app.route('/summary', methods=["GET"])
def summary():
    name = request.args.get('name')
    conn = sqlite3.connect('example.db')
    c = conn.cursor()
    if name != None and name != '':
        c.execute('SELECT * FROM records WHERE name=?', (name,))
    else:
        c.execute('SELECT * FROM records')
    records = c.fetchall()
    results = []
    for r in records:
        results.append({'timestamp':r[1], 'name':r[2]})
    conn.commit()
    conn.close()
    resp = {'status':'OK', 'results':results}
    return jsonify(resp)

if __name__ == '__main__':
    conn = sqlite3.connect('example.db')
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS records
             (_id INTEGER PRIMARY KEY AUTOINCREMENT,
             timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
             name TEXT NOT NULL)''')
    conn.commit()
    conn.close()
    app.run(debug=True)
