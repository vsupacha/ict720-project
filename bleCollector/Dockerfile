FROM python:3.9-bullseye

WORKDIR /usr/src/app
COPY requirements.txt ./
RUN pip install -r requirements.txt
COPY . .
EXPOSE 8080
CMD ["python", "-u", "./app.py"]
