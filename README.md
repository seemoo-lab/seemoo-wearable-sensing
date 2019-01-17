# Tizen Data Collector Service
This is a Tizen service native App to record sensors, audio, WIFI and bluetooth LE on a Samsung Galaxy smartwatch. The service app can't be run independently and is used by the Tizen WebApp. The native service app should be called by another Tizen app.

## Getting Started

**Note**: You need and actual device in order to start and test the app it wont work with an emulator.
### Set Up the Environment
1. Download and install Tizen Studio (https://www.tizen.org/).
2. Install in the Tizen Package Manager on installation or later (can be opened in Tizen Studio by Pressing Alt+Shift+P) and install at least Wearable SDK 3.2.2 and Wearable 3.0 and under Extension SDK install additionally Samsung Certification Extension and Samsung Wearable extension. 
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
