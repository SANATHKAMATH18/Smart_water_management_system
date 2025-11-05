from flask import Flask, request, render_template, jsonify
import sqlite3
from datetime import datetime

app = Flask(__name__)

# ---------------- Database Setup ----------------
def init_db():
    conn = sqlite3.connect('water_data.db')
    c = conn.cursor()
    c.execute('''CREATE TABLE IF NOT EXISTS data (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    timestamp TEXT,
                    device_id TEXT,
                    flow REAL,
                    level REAL,
                    temp REAL,
                    hum REAL,
                    points INTEGER)''')
    conn.commit()
    conn.close()

init_db()

# ---------------- Data Upload API ----------------
@app.route('/upload', methods=['POST'])
def upload():
    data = request.get_json()
    conn = sqlite3.connect('water_data.db')
    c = conn.cursor()
    c.execute("INSERT INTO data (timestamp, device_id, flow, level, temp, hum, points) VALUES (?, ?, ?, ?, ?, ?, ?)",
              (datetime.now(), data.get('device_id', 'home1'), data['flow'], data['level'],
               data['temp'], data['hum'], data['points']))
    conn.commit()
    conn.close()
    return jsonify({"status": "success"}), 200

# ---------------- Dashboard Page ----------------
@app.route('/')
def dashboard():
    conn = sqlite3.connect('water_data.db')
    c = conn.cursor()
    c.execute("SELECT timestamp, flow, level, temp, hum, points FROM data ORDER BY id DESC LIMIT 50")
    records = c.fetchall()
    conn.close()
    return render_template('dashboard.html', records=records)

# ---------------- Chart Data API ----------------
@app.route('/chart-data')
def chart_data():
    conn = sqlite3.connect('water_data.db')
    c = conn.cursor()
    c.execute("SELECT timestamp, flow, level, temp FROM data ORDER BY id DESC LIMIT 20")
    rows = c.fetchall()
    conn.close()
    rows.reverse()  # chronological order
    labels = [r[0][11:19] for r in rows]
    flow = [r[1] for r in rows]
    level = [r[2] for r in rows]
    temp = [r[3] for r in rows]
    return jsonify({"labels": labels, "flow": flow, "level": level, "temp": temp})

# ---------------- Leaderboard Page ----------------
@app.route('/leaderboard')
def leaderboard():
    conn = sqlite3.connect('water_data.db')
    c = conn.cursor()
    c.execute("SELECT device_id, MAX(points) FROM data GROUP BY device_id ORDER BY MAX(points) DESC")
    leaderboard_data = c.fetchall()
    conn.close()
    return render_template('leaderboard.html', leaderboard=leaderboard_data)

# ---------------- Run Server ----------------
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
