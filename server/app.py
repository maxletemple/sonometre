import json
from flask import Flask, render_template, jsonify
import requests

app = Flask(__name__)
ESP32url = 'http://192.168.0.113'

@app.route("/")
def index():
    return render_template('index.html')

@app.route("/index.html")
def alt_index():
    return render_template('index.html')

@app.route("/sound")
def sound():
    file = requests.get(ESP32url + '/sound')
    return file.json()

@app.route("/history")
def history():
    file = requests.get(ESP32url +'/history')
    return file.json()