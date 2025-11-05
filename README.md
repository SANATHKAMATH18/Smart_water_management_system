# Smart Water Management System

This project is an IoT-based Smart Water Management System designed to monitor, visualize, and analyze water usage in real-time. It uses an Arduino and a NodeMCU to collect sensor data, a Flask server to store and process it, and a web-based dashboard to display the information. The system also includes a gamification element to encourage water conservation.

## 🌟 Features

* **Real-time Monitoring**: Tracks water flow, tank level, temperature, and humidity.
* **Web Dashboard**: A live-updating dashboard (built with Flask and Chart.js) displays current sensor data and historical trends.
* **Data Analytics**: An analytics page provides a summary of average water usage (flow, level, temperature).
* **Gamification**: Encourages water conservation by awarding points for efficient usage and subtracting points for waste (e.g., overflow or high flow indicating leaks). Badges like "🌱 New User," "💧 Saver," and "🏅 Eco Hero" are assigned based on points.
* **Leaderboard**: A web page ranks users (devices) by their highest-achieved points, highlighting the top 3 conservationists.
* **Data Logging**: All incoming sensor data is timestamped and stored in an SQLite database for historical analysis.
* **Blynk Integration**: The system also sends data to the Blynk cloud platform for mobile monitoring.

## 🛠️ Components

### Hardware
1.  **Arduino Uno**: Reads data from all the physical sensors.
2.  **NodeMCU (ESP8266)**: Connects to WiFi, gets data from the Uno, applies gamification logic, and sends data to the Flask server and Blynk.
3.  **Sensors**:
    * **Flow Sensor**: Measures the rate of water flow.
    * **Ultrasonic Sensor (HC-SR04)**: Measures the water level in a tank.
    * **DHT11 Sensor**: Measures ambient temperature and humidity.

### Software
1.  **Backend**: A **Flask** (Python) server (`app.py`) that:
    * Provides a `/upload` API endpoint to receive data from the NodeMCU.
    * Stores data in an **SQLite** database (`water_data.db`).
    * Serves the HTML web pages and a `/chart-data` API for the charts.
2.  **Frontend**: Simple HTML templates (`dashboard.html`, `analytics.html`, `leaderboard.html`) styled with CSS (`style.css`) and using **Chart.js** for data visualization.
3.  **Cloud**: **Blynk** for mobile app connectivity.

## 🌊 Data Flow

1.  The **Arduino Uno** reads data from the flow, ultrasonic, and DHT11 sensors.
2.  It formats this data into a string (`flow,level,temp,hum`) and sends it to the **NodeMCU** via serial communication.
3.  The **NodeMCU** parses the data, applies the gamification rules to calculate points, and assigns a badge.
4.  The NodeMCU sends the data to two destinations:
    * **Blynk Cloud** (for the mobile app).
    * The **Flask Server's** `/upload` endpoint via an HTTP POST request with a JSON payload.
5.  The **Flask Server** receives the JSON data and inserts it into the `water_data.db` SQLite database.
6.  When a user opens the **web dashboard**, their browser fetches the HTML and then makes requests to `/chart-data` to populate and update the charts.

## 🚀 Setup & Installation

### 1. Hardware
* Connect the Flow Sensor, Ultrasonic Sensor, and DHT11 Sensor to the appropriate pins on the **Arduino Uno** as defined in `smart_water_management_system_uno_code.ino`.
* Connect the **Arduino Uno** to the **NodeMCU** via SoftwareSerial:
    * Uno `A2` (TX) -> NodeMCU `D2` (RX)
    * Uno `GND` -> NodeMCU `GND`
    *(Note: The Uno's `A3` (RX) is connected to the NodeMCU's `D3` (TX), as per the code).*

### 2. Microcontrollers
1.  **Arduino Uno**:
    * Open `Hardware code/smart_water_management_system_uno_code/smart_water_management_system_uno_code.ino` in the Arduino IDE.
    * Flash the code to your Arduino Uno.
2.  **NodeMCU (ESP8266)**:
    * Open `Hardware code/smart_water_management_system_node_mcu/smart_water_management_system_node_mcu.ino` in the Arduino IDE (with ESP8266 board support).
    * Update the following variables:
        * `ssid`: Your WiFi network name.
        * `pass`: Your WiFi password.
        * `BLYNK_AUTH_TOKEN`: Your Blynk project's auth token.
        * `flaskServer`: The URL of your Flask server (e.g., `http://YOUR_COMPUTER_IP:5000/upload`).
    * Flash the code to your NodeMCU.

### 3. Backend Server
1.  Ensure you have Python 3 and Flask installed.
    ```bash
    pip install Flask
    ```
2.  Navigate to the project's root directory (where `app.py` is located).
3.  Run the Flask application:
    ```bash
    python app.py
    ```
4.  The server will start (by default on `0.0.0.0:5000`) and automatically create the `water_data.db` database file.

### 4. Access the Dashboard
Open your web browser and navigate to the IP address of the computer running the Flask server.
* **Dashboard**: `http://127.0.0.1:5000/`
* **Leaderboard**: `http://127.0.0.1:5000/leaderboard`
* **Analytics**: `http://127.0.0.1:5000/analytics`
