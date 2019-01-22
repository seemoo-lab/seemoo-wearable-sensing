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
* This app can only be tested on a real device and will not work on the emulator. 
* To connect the *Tizen Studio* and the targer *Gear S3* device make sure they are in the same WiFi network.
* For each Gear S3 watch a new Samsung Certificate **MUST be created** in order to install the app! 


### Running the App
1. Make sure the correct Samsung Certificate is selected (one device – one certificate) and the watch is connected to the *Tizen Studio*.
2. First, run the native service on the watch by right clicking on the *ServiceSensor* project and selecting ```Run As -> Tizen Native Application``` (the service automatically gets built and executed on the watch).
3. Second, run the web app on the watch  by right clicking on the *HelloAccessoryProvider* project and selecting ```Run As -> Tizen Web Application```.
4. The app automatically starts on the watch and it also appears in the list of installed apps (clik on the icon to start the app). The data collection is started/stopped by pressing the START/STOP button in the center of the screen.

**Notes:**
* Each time the data collection is started it **overwrites** the data collected before!
* The native service functionality is implemented in *native-app/src/servicesensor.c*, the web app UI is implemented using *web-app/index.html* and *web-app/js/app.js*. 

### Data Extraction and Structure
The collected sensor data is stored internally on the watch (keep in mind the space requirements, especially for audio data) under */home/owner/media/Others*. The data is extracted using the *Device Manager* in *Tizen Studio* (opens via *Alt+Shift+V*)—the watch should be connected to the *Tizen Studio*. In the *Device Manager* select the required watch and navigate to the */home/owner/media/Others* folder, then right click on the *Others* folder and select the "Pull" option. 

The collected data is stored in the following structure (use [structure-gear-data.py](https://github.com/seemoo-lab/ubicomp19_zero_interaction_security/tree/master/Preprocessing) to reformat the data):

```
+ Others/ 
| + accData.txt
| + audio.wav
| + audio.time
| + barData.txt
| + ble.txt
| + gyrData.txt
| + luxData.txt
| + wifi.txt
```

## Authors

Timm Lippert and Mikhail Fomichev


## License

The code is licensed under the GNU GPLv3. See LICENSE.txt for details.
