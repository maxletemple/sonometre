import json
from flask import Flask, render_template, jsonify
import requests

app = Flask(__name__)

@app.route("/")
def index():
    return render_template('index.html')

@app.route("/index.html")
def alt_index():
    return render_template('index.html')

@app.route("/sound")
def sound():
    file = requests.get('http://192.168.0.113/sound')
    return file.json()

@app.route("/history")
def history():
    file = requests.get('http://192.168.0.113/history')
    return file.json()