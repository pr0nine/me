**IOT HUB**
-----------

This project transforms a simple ESP8266 microcontroller into a powerful smart hub for monitoring household environmental conditions and AC power consumption. It reads data from temperature, humidity, voltage, and current sensors, logs the data to a PostgreSQL database, and visualizes it in real-time using Grafana.

Furthermore, it integrates with **Sinric Pro** to provide seamless smart home control via the Sinric Pro app, Google Home, and Amazon Alexa.

### **Features**

*   **Real-time Monitoring**: Continuously tracks ambient temperature, humidity, AC line voltage, and current.
    
*   **Data Logging**: Pushes sensor readings to a time-series-friendly PostgreSQL database.
    
*   **Rich Data Visualization**: Connects to Grafana to create beautiful, interactive graphs and dashboards to analyze historical data.
    
*   **Smart Home Integration**: Uses Sinric Pro to connect with Google Home and Amazon Alexa.
    
    1.  **Temperature & Humidity Sensor**: Only Temperature data is fetching to Google Home, not tested with Amazon Alexa but both are fetching in Sinric Pro Dashboard.
        
    2.  **Dimmable Switch**: Uses a digital pin of esp8266 to generate a pwm signal, intensity can be controlled using Google Home and Amazon Alexa.
        
    3.  **On/Off Switch**: Uses another digital pin of esp8266 to generate a high or low signal can be hooked up to a relay or other switching device.
        

### **System Architecture**

The system is composed of four main parts: the sensor node, the database, the visualization platform, and the smart home service.

1.  **Sensor Node (ESP8266)**: The heart of the project. It connects to the physical sensors, reads the data, connects to your local WiFi, and sends the data to both the PostgreSQL database running locally and connects to Sinric Pro.
    
2.  **Database (PostgreSQL)**: A robust, open-source object-relational database system that stores all the time-stamped sensor readings.
    
3.  **Visualization (Grafana)**: Queries the PostgreSQL database and displays the data in insightful graphs and dashboards that can be accessed from any web browser on your network.
    
4.  **Smart Home Service (Sinric Pro)**: Acts as a bridge between your ESP8266 and smart assistants like Google Home and Alexa, enabling voice commands and remote access.
    

```text-plain
+-------------------+      +----------------+      +------------------+
|      Sensors      |      |                |      |   PostgreSQL DB  |
| <| DHT22 (Temp/H) |----->|   ESP8266 12F  |----->| (Data Logging)   |
| <| ZMPT101B   (V) |      |        +       |      +--------+---------+
| <| ACS712 30A (A) |      |    (Firmware)  |               |
+-------------------+      +----------------+               |
                                   ^                        |
                                   |                        | (Query)
                                   | (Real-time updates     |
                                   |  & commands)           |
                                   v                        v
+-------------------+      +----------------+      +------------------+
| Google Home/Alexa |<---->|  Sinric Pro    |      |     Grafana      |
|  (Voice Control)  |      | (Cloud Service)|      | (Visualization)  |
+-------------------+      +----------------+      +------------------+
```

### **1\. Hardware Requirements**

|     |     |     |
| --- | --- | --- |
| **Component** | **Description** | **Purpose** |
| **ESP8266 12F** | A Wi-Fi enabled microcontroller (e.g., NodeMCU, Wemos D1 Mini). | Main microcontroller for the project. |
| **DHT11/DHT22** | Digital Temperature and Humidity sensor. | Measures ambient conditions. |
| **ZMPT101B + ACS712 30A** | AC Voltage Sensor Module paired with Current Sensor Module | Measures Voltage and Current. |
| **12V Power Supply** | To power the project | Powering the circuit. |
| **D4184 Module**    or | Solid-state relay replacement designed for efficient switching of DC loads | For switching functionality. |
| **Dimmer Module + Relay module** | An AC light dimmer module controllable by a PWM signal. and a relay module for switching ac loads | For the Dimmable and on/off Switch functionality. |
| **MP1584 Buck module + AMS1117 3.3v** | Mini MP1584 DC-DC Adjustable Buck module set to 5v | Paired with some decoupling caps for powering the esp8266. |

### **2\. Software & Services**

*   **Arduino IDE** or **PlatformIO**: For compiling and uploading the firmware to the ESP8266.
    
*   **PostgreSQL Server**: To install on a local machine or a cloud server.
    
*   **Grafana**: To install on a local machine or a cloud server.
    
*   **Sinric Pro Account**: A free account is required.
    

### **3\. Setup & Installation**

#### **Step 1: Hardware Wiring**

1.  **DHT Sensor**:
    
    1.  VCC to 3.3V
        
    2.  GND to GND
        
    3.  DATA to GPIO5 ( D1 ).
        
    
    Now unfortunately the esp8266 have only one analog input A0 so to gather both of the sensor's data we can use a multiplexer but I have used a silly diy route using two PNP BJTs to switch each sensors and a diode on their output to stop signal mixing, and the esp8266 12f module have input range between 0 to 1 volt so used a voltage divider consisting of 220k ohm and 125k ohm resistors smaller one for the gnd side with compensating voltage drop created by diode, this voltage divider is already present in dev boards such as nodemcu.
    
2.  **ZMPT101B Sensor** :
    
    1.  Connect the AC lines through the measuring transformer as per the module's diagram.
        
    2.  VCC to collector of transistor.
        
    3.  GND to GND.
        
    4.  Output to positive terminal of a diode to the voltage divider.
        
3.  **ACS712 30A Sensor** :
    
    1.  Connect the AC lines through the current sensors load terminals as per the module's diagram.
        
    2.  VCC to collector of transistor.
        
    3.  GND to GND.
        
    4.  Output to positive terminal of a diode to the voltage divider.
        
        !\[Circuit diagram\](https://github.com/pr0nine/me/blob/a7156f77bfd5dab9e2f24cadc0e2659759d64aa4/iot/cct.jpeg)
        
4.  **D4184 Module**:
    
    1.  Module VCC to 12V to Load VCC.
        
    2.  Module GND to GND.
        
    3.  Module OUT to Load GND.
        
    4.  PWM to GPIO14 ( D5 ) same for other one PWM to GPIO4 ( D2 ).
        
    5.  GND to GND
        
5.  **Relay Module (On/Off Switch)** if not using **D4184** module:
    
    1.  VCC to 5V.
        
    2.  GND to GND.
        
    3.  IN to GPIO14 ( D5 ).
        
6.  **Dimmer Module** if not using **D4184** module:
    
    1.  Connect AC lines as per module instructions.
        
    2.  VCC to 5V.
        
    3.  GND to GND.
        
    4.  PWM/Control Pin to GPIO4 ( D2 ).
        
7.  **Power supply** : 
    
    1.   12v from dc jack to input of MP1584 Buck module set to 5v 
        
    2.  Output of MP1584 Buck module to input of ams1117 3.3v voltage regulator 
        
    3.  Output of ams1117 to esp8266 skip voltage regulator if using a dev board
        
    4.  Add Decoupling capacitors on inputs and outputs
        

[!\[image alt\](cd.jpeg)](https://github.com/pr0nine/me/blob/821124aef381486dfec31ae5137c151eaa65f45c/iot/cd.jpeg)

!\[Complete Product\](cp.jpeg)

**⚠️ CAUTION:** Working with mains AC voltage is dangerous. Ensure you know what you are doing and take all necessary safety precautions. If you are unsure, consult a professional.

#### **Step 2: Database Setup (PostgreSQL)**

1.  Connect to the database which we want to use and run the following SQL command to create the table for storing sensor data:
    
    ```text-plain
    CREATE TABLE esp (
        ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
        n SERIAL PRIMARY KEY,
        tempo REAL,
        humidity REAL,
        voltage REAL,
        current REAL,
        watt REAL
    );
    ```
    
2.  Create a correction function to calibrate the sensors if readings are not accurate 
    
    ```text-x-sql
    CREATE OR REPLACE FUNCTION volt_correction()
    RETURNS TRIGGER AS $$
    BEGIN
       IF NEW.voltage > 20 THEN
       NEW.voltage := NEW.voltage - 20; -- add or substract to calibrate
       END IF;
       RETURN NEW;
    END;
    $$ LANGUAGE plpgsql;
    
    CREATE TRIGGER correction
    BEFORE INSERT ON esp
    FOR EACH ROW
    EXECUTE FUNCTION volt_correction();
    ```
    

#### **Step 3: ESP8266 Firmware**

1.  **Install Libraries**: In your Arduino IDE, install the following libraries from the Library Manager:
    
    *   `SinricPro`
        
    *   `DHT sensor library` by Adafruit
        
    *   `ZMPT101B` 
        
    *   `ACS712`
        
    *   **SimplePgSQL**  a PostgreSQL client library run `git clone https://github.com/ethanak/SimplePgSQL.git` in Arduino IDE's libraries directory.
        
2.  **Configure Credentials**: Open the main `.ino` sketch and update the following placeholders:
    
    ```text-plain
    //Setup connection and Database=====================
    const char* ssid = "xxxxxxxx"; // your wifi ssid
    const char* pass = "xxxxxxxxxxxxxxxxx"; // your wifi password
    WiFiClient client;
    IPAddress PGIP(69, 69, 69, 69);     // your PostgreSQL server IP
    const char user[] = "xxxx";       // your database user
    const char password[] = "xxxxxxxxxx";   // your database password
    const char dbname[] = "xxxxxx";         // your database name
    ```
    
    _You will get the Sinric Pro credentials and Device IDs from the Sinric Pro portal._
    
    ```text-plain
    #define APP_KEY           "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    #define APP_SECRET        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    #define TEMP_SENSOR_ID    "xxxxxxxxxxxxxxxxxxxxxxxx"
    #define SWITCH_ID_1       "xxxxxxxxxxxxxxxxxxxxxxxx"
    #define RELAYPIN_1        D2
    #define DIMSWITCH_ID      "xxxxxxxxxxxxxxxxxxxxxxxx"
    #define RELAYPIN_2        D5
    ```
    
3.  **Upload Firmware**: Select your ESP8266 board in the Arduino IDE, choose the correct COM port, and upload the sketch.
    

#### **Step 4: Sinric Pro Setup**

1.  Log in to your [Sinric Pro](https://portal.sinric.pro/) account.
    
2.  Go to **Devices** and click **Add Device**.
    
3.  Add the following three devices, giving each a name and saving its Device ID to use in the firmware:
    
    *   **Device Type**: `Temperature Sensor` (Note its capabilities for humidity).
        
    *   **Device Type**: `Dimmable Switch`.
        
    *   **Device Type**: `Switch`.
        
4.  Go to the **Credentials** tab and copy your App Key and App Secret into the firmware.
    

#### **Step 5: Grafana Setup**

1.  Install and run Grafana on your server.
    
2.  Log in to Grafana.
    
3.  **Add Data Source**:
    
    *   Go to Configuration -> Data Sources -> Add data source.
        
    *   Select **PostgreSQL**.
        
    *   Enter the connection details for your PostgreSQL database.
        
    *   Click **Save & Test**.
        
4.  **Create a Dashboard**:
    
    *   Go to Dashboards -> New Dashboard -> Add a new panel.
        
    *   In the query editor, select your PostgreSQL data source.
        
    *   Use a query like the one below to visualize temperature over time:
        
    
    ```text-plain
    SELECT $__time(ts), tempo FROM esp
    ```
    
    *   Customize the visualization type (e.g., Time series) and repeat for humidity, voltage, current and power(watt).
