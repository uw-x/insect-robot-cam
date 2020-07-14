# InsectCam Android App

This companion Android app receives the video stream from our vision system mounted onboard the insect. The corresponding BLE firmware uses the NRF52832 to read image data from the Himax HM01B0 camera and stream it to this Android App. The app receives images and updates the display in real time, and also includes features for logging data and storing the images. It also includes an interface to steer pan the camera right and left, and to capture images when the insect moves and triggers an onboard accelerometer. This app is based on an [this example](https://github.com/NordicPlayground/Android-Image-Transfer-Demo) developed by Nordic Semiconductor.

## Setup
- Download [Android Studio](https://developer.android.com/studio)
- Open the project in Android Studio

Tested on:   
- Samsung Galaxy S9 (achieved 5 fps framerate using 2 Mbps mode)   
- Samsung Galaxy S6 (1Mbps mode only, results in lower framerate)   

### Note
- Android 4.3 or later is required.
