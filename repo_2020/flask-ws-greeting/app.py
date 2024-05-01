from flask import Flask, render_template, request, jsonify


app = Flask(__name__)
app.config["TEMPLATES_AUTO_RELOAD"] = True

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/greeting', methods=["GET","POST"])
def greeting():
    if request.method == "GET":
        name = request.args.get('name')
        if name != '':
            return render_template('hello.html', name=name)
        else:
            return render_template('hello.html')
    if request.method == "POST":        
        try:
            payload = request.get_json(force=True)
            name = payload['name']
            if name != '':
                resp = {'status':'OK', 'greeting':'Hello, ' + name}
            else:
                resp = {'status':'ERR', 'greeting':'Who are you?'}
            return jsonify(resp)
        except:
            pass        
    return 'Unknown request'


if __name__ == '__main__':
    app.run(debug=True)