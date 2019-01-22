# Gear S3 Data Collector (Tizen)

This folder contains the source code of the Samsung Gear S3 app (Tizen) used for data collection (audio, sensor data, WiFi and BLE beacons) in the paper "Perils of Zero-Interaction Security in the Internet of Things", by Mikhail Fomichev, Max Maass, Lars Almon, Alejandro Molina, Matthias Hollick, in Proceedings of the ACM on Interactive, Mobile, Wearable and Ubiquitous Technologies, vol. 3, Issue 1, 2019.

The *Samsung Gear S3* app consists of two parts: the native service is used for actual data collection and the web app is used to invoke the native service.  

In our experimetns we used *Samsung Gear S3* (both Classic and Frontier) watches with *Tizen Version 3.0.0.2* to collect the following sensor modalities:

| **Hardware**      | **Sensors**       | **Sampling rate**  | **Comments** |
| ------------- |:-------------:| -----:|:-----------------------:|
| Gear S3  | Barometric pressure (hPa), luminosity (lux);  movement -> accelerometer (*m/s^2*), gyroscope (*deg/s*), magnetometer (*uT*) | 10 Hz / 50 Hz |        10 Hz for pressure and lux, 50 Hz for movement sensors            |
| Gear S3  | Bluetooth low energy (BLE) and WiFi beacons      |   0.1 Hz |  Scan visible BLE and WiFi access points (APs) for 10 seconds     |
| Gear S3  | Raw audio stream   |    16 kHz |     The audio is stored in a **.WAV* file 

## Getting Started

### Requirements

```
SDK Wearable 3.0               # Necessary for the native app (Main SDK)
SDK Wearable 2.3.2             # Necessary for the web app (Main SDK)
Samsung Certificate Extension  # Extension SDK 
Samsung Wearable Extension     # Extension SDK 

Tizen version >= 3.0.0.2 
```

### Setting Up the Environment
1. Download and install *Tizen Studio* (https://www.tizen.org/).
2. Use the *Tizen Package Manager* (press *Alt+Shift+P* to open in *Tizen Studio*) to install the necessary SDKs and Extensions mentioned above.
3. Enable debugging on the *Gear S3* watch and connect it to the *Tizen Studio* as described [here](https://developer.tizen.org/development/training/web-application/getting-started/creating-your-first-tizen-wearable-web-application#run) (see the section "Running on a Target Device").  
4. Create a Samsung Certificate using the *Certificate Manager* (press *Alt+Shift+P* to open in *Tizen Studio*) as described [here](https://developer.tizen.org/development/visual-studio-tools-tizen/tools/certificate-manager).
5. Import native and web projects by using ```File -> Import -> Tizen -> Tizen Project``` and following the wizzard instructions.

**Notes:**
* This app can only be tested on a real device and will not work on emulator. 
* To connect the *Tizen Studio* and the targer *Gear S3* device make sure they are in the same WiFi network.
* For each Gear S3 watch a new Samsung Certificate **MUST be created** in order to install the app! 


### Running the App
1. Make sure the correct Samsung Certificate is selected (one device – one certificate) and the watch is connected to the *Tizen Studio*.
2. First, run the native service on the watch by right clicking on the *ServiceSensor* project and selecting ```Run As -> Tizen Native Application``` (the service automatically gets built and executed on the watch).
3. Second, run the web app on the watch  by right clicking on the *HelloAccessoryProvider* project and selecting ```Run As -> Tizen Tizen Web Application```.

The Web application should then open automatically on the smartwatch, if not you should be able to find the app at the end of your application list on your watch and select and start it. \

The source code of the native app can be found in it's project folder under src/servicesensor.c \
The Web apps UI can be found in it's project folder in the index.html file and the JS script can be found in js/app.js

### Data Structure

All recorded data are saved on the device which can be accessed via Tizen Studio and are located on the device under /home/owner/media/Others/. You can access the devices storage via the Device Manager in Tizen Studio (open via Alt+Shift+V). In the Device Manager Simply select your device and on the right since you can see the file system of the device and navigate to the location of the stored files. \
All data are saved as simple Text files with timestamps except of the audio Files which are saved as raw wav files.

```
Timestamp/              # Root folder of the sensor data, corresponds the start time of data collection
  + audio/
  | + XX.wav            # Encoded audio data, XX is the sensor number, e.g., 01, 02, etc. set in the DataCollector app
  | + audio.time        # Time when the audio started
  + ble/
  | + ble.txt           # BLE data
  + sensors/
  | + accData.txt       # Accelerometer data
  | + barData.txt       # Barometric data
  | + gyrData.txt       # Gyroscope data
  | + luxData.txt       # Lluminosity data
  | + magData.txt       # Magnetometer data
  + wifi/
    + wifi.txt          # WiFi data
```

## Authors

Timm Lippert and Mikhail Fomichev


## License

The code is licensed under the GNU GPLv3. See LICENSE.txt for details.
