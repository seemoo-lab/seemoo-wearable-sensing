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
1. Make sure you selected the correct certificate for the correct device and that in the upper drop down menu the correct device is selected and connected.
2. First run the native service app on your device by right click on the native App Project in Tizen Studio and select "Run As" -> "Tizen Native Application" (Then the project automatically gets build and executed on the device).
3. Then run the WebApp on your device right click on the WebApp project in tizen studio and select "Run As" -> "Tizen Web Application".



1. Download and install *Tizen Studio* (https://www.tizen.org/).
2. Download and install *Tizen Studio* (https://www.tizen.org/).
2. Use the *Tizen Package Manager* (press *Alt+Shift+P* in *Tizen Studio* to open it) to install at least Wearable SDK 3.2.2 and Wearable 3.0 and under Extension SDK install additionally Samsung Certification Extension and Samsung Wearable extension. 
3. Enable debugging on your Tizen Smartwatch and connect it with Tizen Studio as described in here: (https://developer.tizen.org/development/training/web-application/getting-started/creating-your-first-tizen-wearable-web-application#run) -> Get to the point "Running on a Target Device"
    
    **Note**: Since both Tizen Studio and the Tizen device need to be in the same network it might be helpful to host an hotspot on your computer and connect your watch to it
        For Windows the following script might work by changing the password to one you want to have for your hotspot: 
        
        **Note**: Not every machine supports this
        
        ```CMD
        $ netsh wlan set hostednetwork mode=allow ssid=Hotspot key=password
        $ netsh wlan start hostednetwork
        ```
4. Create Samsung Certificates (Samsung Account Needed) \
Create a Samsung Certificate via the Certificate Manager (Alt+Shift+C) as described in here : (https://developer.tizen.org/development/visual-studio-tools-tizen/tools/certificate-manager).


    **Note**: For each hardware Device you need to create its own certificate in order to be able to install an App on it.

5. Import the Projects. \
Import the native service app as well as the the provided Tizen WebApp into Tizen Studio by selecting File -> Open Projects from File System... and then select each project the native service app as well as the Web App.

**Note**: You need and actual device in order to start and test the app it wont work with an emulator.


### Run the App
1. Make sure you selected the correct certificate for the correct device and that in the upper drop down menu the correct device is selected and connected.
2. First run the native service app on your device by right click on the native App Project in Tizen Studio and select "Run As" -> "Tizen Native Application" (Then the project automatically gets build and executed on the device).
3. Then run the WebApp on your device right click on the WebApp project in tizen studio and select "Run As" -> "Tizen Web Application".

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
