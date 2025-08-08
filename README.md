# Smart home docker installation
Docker installation of Home Assistant, ESPHome and MQTT broker (Mosquitto). + ESP32 configuration for ESPHome and MQTT
### Prerequisities:
* Docker or something similar (I used **podman** for my containers)

## Entire folder structure containing Home Assistant, ESPHome and Mosquitto:
      smart-home/
      ├── ESPHome/
      │   └── docker-compose.yml
      ├── home-assistant/
      │   ├── Dockerfile
      │   ├── docker-compose.yml  (This docker-compose.yml will have the script for Home Assistant as well as Mosquitto MQTT broker.)
      │   └── config/   (Config folder in which the home-assistant's own config will be directed.)
      │   └── mosquitto/
      │       └── config/
      │           ├── mosquitto.conf   (The main configuration file for the MQTT broker.)
      │           └── pwfile   (The password file for authentication (generated with mosquitto_passwd).)

<br><br>

# Home Assistant - Docker
## Create these files for docker inside the home-assistant/ folder:
### 1. Create the **Dockerfile**: (name it as Dockerfile)
   
        FROM ghcr.io/home-assistant/home-assistant:stable
        
        ENV TZ=Europe/Helsinki
        
        COPY ./config /config
### 2. Create a **docker-compose.yml** file:

        version: "3.9" # This is the Compose file format version.
        services:
          homeassistant:
            build: .
            container_name: homeassistant
            privileged: true
            restart: unless-stopped
            network_mode: host # host = your machine's local host address. Any device in the same local network can reach that IP address.
            volumes:
              - ./config:/config
              - /run/dbus:/run/dbus:ro
## Setup the docker as a running service:

### 1. Build the container. In terminal, once you are inside the home-assistant/ folder, run this to build the service and set it running:

        docker compose up -d --build

✅***Congratulations!*** Now the container is created and running as service in your localhost. 

***Bonus info.*** From now on you can simply **start** and **stop** the docker with the following commands:

    sudo docker start homeassistant


    sudo docker stop homeassistant

If you change the Dockerfile or docker-compose.yml later. This rebuilds and restarts  with updates.

    sudo docker compose up -d --build

If you just want to restart without rebuild:

    sudo docker compose restart

### 🔷 Learn how to create cool dashboards for Home Assistant [here](https://www.home-assistant.io/dashboards/)

<br><br>
<br><br>

# ESPHome - Docker

### 1. Create an ESPhome docker file. If it doesn’t exist yet:

        version: '3.8'
        
        services:
          esphome:
            container_name: esphome
            image: ghcr.io/esphome/esphome:latest
            restart: always
            network_mode: host  # Important for mDNS and OTA to work
            volumes:
              - ./esphome:/config   # Change path if you want configs elsewhere
            environment:
              - TZ=Your/Timezone    # e.g., Europe/Helsinki

### 2. Plug your ESP32 device to a computer. 

You must access the ESPhome interface from the browser using the default host IP (same as Home Assistant) and the Port is by default: 6052. (e.g http://192.168.69.13:6052)

### 3. Click New Device. The page will ask you to open the ESP HOME WEB, click it and it will open a new window.

  In the ESP HOME WEB– page, click connect. That’s when a list of USB devices will be shown. Choose the one in which your ESP32 is connect, it will be probably named something like *USB to UAR bridge controller*.
  
 #### *NOTE*: Firefox cannot be used for this, another good browser could be Chrome for example.
  
  Choose the **Install for first use** option and let it do its thing. Once it’s done, setup WiFI to your ESP32 by giving it the network's SSID and Password. Once done, you can test if the network has given it a proper IP address by clicking the visit option.
  
  You have now set up the ESP32 as a working IoT device using ESPHome. Go back to the ESPhome interface which resides in the :6052 port. The page should automatically detect a new available ESP32 device. Click the **show devices** button and then **take control**. 
  
  **! The SSID and Password might have to be reconfigured to the .YAML file so just click EDIT and paste this in there:**

    wifi:
      ssid: "WIFI_NAME"
      password: "WIFI_PASSWORD"

**PLEASE NOTE**: The ESP32 can already be edited to work with your desired sensors or whatever. For example, setting it up to fetch data from a temperature and humidity sensor can be done by: 
Going to the ESPHome website component section and then searching your sensor (in this case it is DTH22). Bellow is an example code which can be then edited to your PIN mapping:
### Example configuration entry
    sensor:
      - platform: dht
        pin: D2
        temperature:
          name: "Living Room Temperature"
        humidity:
          name: "Living Room Humidity"
        update_interval: 60s

copy and paste it to the .YAML file. Done.

### 4. Then, once everything is good to go, install the updated .YAML file to the device. 

You can choose wirelessly and then just wait. If it starts complaining about the WiFI SSID and/or password, there’s something wrong with the WiFI config in the .YAML file.

***WAIT*** until the log says something like “seems like booting went well”. Then you can click *stop*.

### 5. Let’s add it to the Home Assistant.

Go to Home Assistant ➡️ settings ➡️ Devices and services ➡️ add integration and search ESPHome. It will ask for the IP address that has been assigned to the new ESP32 device and its own port. All of these should be found in its .YAML file. Then, name it how you like and there you go.

✅You have succesfully setup ESPHome running in a container, connected an ESP32 microcontroller to ESPHome and integrated it to Home Assistant!

<br><br>
<br><br>

# MQTT broker (Mosquitto - Docker)

More detailed installation quide [here](https://github.com/sukesh-ak/setup-mosquitto-with-docker).

## Setup Mosquitto docker

Before starting, inside the home-assistant folder, create a folder called **mosquitto**. Inside the mosquitto folder create a folder called **config**, and finally inside config create a file called **mosquitto.conf** like this:

      ├── home-assistant/
      │   ├── other folders/files...
      │   └── mosquitto/
      │       └── config/
      │           ├── mosquitto.conf   (The main configuration file for the MQTT broker.)
      │           └── pwfile   (The password file for authentication (generated with mosquitto_passwd).)

### 1. Open the docker-compose.yml file for Home Assistant and modify it. Enter to the bottom the compose script for Mosquitto:

     version: "3.9" # This is the Compose file format version.
     services:
       homeassistant:
         Home
         Assistant
         Compose
         Here
        mosquitto:   <-- Here begins Mosquitto compose script.
          image: docker.io/eclipse-mosquitto:2.0
          container_name: mosquitto
          ports:
            - "1883:1883" #default mqtt port
            - "9001:9001" #default mqtt port for websockets
          volumes:
            - ./config:/mosquitto/config:rw
            - ./data:/mosquitto/data:rw
            - ./log:/mosquitto/log:rw
          restart: unless-stopped

***The compose script doesn't mention the network at all which means that Mosquitto will be using the 0.0.0.0 (127.0.0.1) IP which is accessible through your computer's local host for wireless devices.***

### 2. Navigate to the home-assistant folder where the docker-compose.yml resides. Create and run the container for Mosquitto:

      podman-compose up -d --build

### 3. Check if the container is up and working (note down container-id):
      
      podman ps

### 4. Create a user/password in the pwfile:

      # login interactively into the mqtt container
      sudo docker exec -it <container-id or container-name> sh
      
      # Create new password file and add user and it will prompt for password. That is going to be the user password.
      mosquitto_passwd -c /mosquitto/config/pwfile user1

### 5. Restart the container:

      podman restart mosquitto

There you go. Your mosquitto is now running in 0.0.0.0:1883

### 6. Connect Home Assistant to the broker (Mosquitto):

Go to settings ➡️ Devices and services ➡️ add integration and search for MQTT. Choose the plain MQTT option.

#### 1. In the **Broker** section insert the IP address (probably 0.0.0.0).

#### 2. In the **Port** section enter the port you have configured.

#### 3. Enter the username and password you have created.

### ✅ Home Assistant is now connected to your MQTT borker (Mosquitto)

<br><br>
<br><br>

## Setup ESP32 to connect to Home Assistant using MQTT

!!! The functional ESP32 MQTT script that reads and sends temperature, humidity and light data can be found [here](/code/ESP32_MQTT_temp_hum_light.ino).

### 1. Edit everyhting in the beginning of the code based on your setup:

      // WiFi credentials
      const char* ssid = "wifi_ssid";
      const char* password = "wifi_password";
      
      // --- MQTT Broker ---
      const char* mqtt_server = "localhost_of_your_MQTT";  // IP of Docker host running Mosquitto.
      const int mqtt_port = 1883; // Edit if yoou are using a different port.
      const char* mqtt_user = "your_username";
      const char* mqtt_pass = "your_password";
      const char* topic = "home/esp32/sensors";
      
      // --- Light Sensor ---
      #define LIGHT_PIN 34   // ADC1_CH6 (GPIO34) - input only

### 2. Understand how the code sends data to the MQTT broker:

The code sends its message to the broker using a **topic** mentioned here

      const char* topic = "home/esp32/sensors";

It sends the data in **.json** format.

       // Create JSON with both temperature and humidity
        StaticJsonDocument<128> doc;
        doc["temperature"] = t;
        doc["humidity"] = h;
        doc["lightP"] = lightPercent;
        doc["lightV"] = lightVoltage;
      
        char jsonBuffer[320];
        serializeJson(doc, jsonBuffer);
      
        client.publish(topic, jsonBuffer);
      
        Serial.print("Published JSON: ");
        Serial.println(jsonBuffer);
      }
      
The data would then look something like this:

      {
        "temperature": 24.3,
        "humidity": 53.7,
        "lightP": 82.0,
        "lightV": 2.7
      }

### 3. Configure the entities into Home Assistant:

**First**, go to **Home Assistant's own configuration.yaml** file and paste this mqtt config line to the bottom (This tells the configuration to look for the mqtt config from a file called **mqtt.yaml**):

      mqtt = !include mqtt.yaml

**Second**, create the **mqtt.yaml** file inside the config folder in which the configuration.yaml file also resides:

      ├── home-assistant/
      │   ├── Dockerfile
      │   ├── docker-compose.yml
      │   └── config/
      |        ├── configuration.yaml
      |        └── mqtt.yaml

**Third**, inside the mqtt.yaml file, create the **new MQTT entities for Home Assistant** (here's a direct copy that was used for this ESP32):

      sensor:
        - name: ESP32_Lab_Temperature
          state_topic: "home/esp32/sensors"
          unit_of_measurement: "°C"
          value_template: "{{ value_json.temperature }}"
          device_class: temperature
          state_class: measurement
        - name: ESP32_Lab_Humidity
          state_topic: "home/esp32/sensors"
          unit_of_measurement: "%"
          value_template: "{{ value_json.humidity }}"
          device_class: humidity
          state_class: measurement
        - name: ESP32_Lab_Light_Percentage
          state_topic: "home/esp32/sensors"
          unit_of_measurement: "%"
          value_template: "{{ value_json.lightP }}"
          state_class: measurement
        - name: ESP32_Lab_Light_Voltage
          state_topic: "home/esp32/sensors"
          unit_of_measurement: "V"
          value_template: "{{ value_json.lightV }}"
          state_class: measurement

**NOTE:** Using ***value_template*** is a convenient way to take values from a specific key in a json package (e.q. "temperature"). This way you can fit everything into one json package which can be sent to one topic.

### 4. Go to Home Assistant's developer tools and *check configuration*. 

This way you are making sure there are no isssues in the configurations. If everything is okay, restart Home Assistant using:

      podman restart homeassistant

### 5. Check MQTT entities

Go to settings ➡️ Devices and services. There you should see entities underneath the MQTT tab. Every entity should be properly configured and deployable as any other IoT devices' entity.

### ✅ Now you have a working MQTT IoT device completely built on an ESP32!

<br><br>

🔴 Future improvement! The entities don't appear to belong to any device. Most likely this could be configured in the **mqtt.yaml** file. 

<br><br>
