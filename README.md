# Wireless Steerable Vision for Insects and Robots
## Abstract
Vision serves as an essential sensory input for insects but consumes substantial  energy resources. The cost to support sensitive photoreceptors has led many insects to develop high visual acuity in only small retinal regions and evolve to move their visual systems independent of their bodies through head motion. By understanding the tradeoffs made by insect vision systems in nature, we can design better vision systems for insect-scale robotics in a way that balances energy, computation and mass. Here we report a fully wireless, power autonomous, mechanically steerable vision system that imitates head motion in a form factor small enough to mount on the back of a live beetle or a similarly sized terrestrial robot. Our electronics and actuator weigh 248 milligrams and can steer the camera over 60◦ based on commands from a smartphone.  The camera streams ‘first person’ 160×120 monochrome video at 1–5 frames per second (fps) to a Bluetooth radio from up to 120 m away.  We mount this vision system on two species of freely walking live beetles, demonstrating that triggering image capture using an on-board accelerometer achieves operational times of up to 6 hours with a 10 mAh battery. We also built a small, terrestrial robot (1.6×2cm) that can move at up to 3.5 cm/s, support vision and operate for 63–260 mins. Our results demonstrate that steerable vision can enable object tracking and wide angle views for 26–84 times lower energy than moving the whole robot.

## Installation and Organization
This repository includes code for both the insect and robot mounted cameras. They are largely the same with some minor additions in the robot version to control it. The directory structure is shown below:
```bash
├── insect_cam (Robot mounted camera software)
│   ├── Insect-Cam-Android (Android code)
│   └── insect_ble_firmware (NRF52832 Bluetooth chip firmware)
└── robo_cam (Robot mounted camera software)
    ├── Robo-Cam-Android (Android code)
    └── robot_ble_firmware (NRF52832 Bluetooth chip firmware)
```

### BLE
1. Download Segger Embedded Studio
2. Download NRF SDK: 
-  Go to https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK/Download#infotabs
-  Select Version 15.2.0 and download it
-  Unzip the “nRF5_SDK_15.2.0_9412b96” in a high level directory (e.g. C:\ on Windows, or home directory on linux/mac, there’s a bug that gives an error if the path is too long)
- Copy the firmware (e.g. "insect_ble_firmware") into this SDK subfolder: nRF5_SDK_15.2.0_9412b96\examples\ble_peripheral
- Copy “nrfx_spis.c” from the “sdk_patch” directory in the repo to “[sdk_location]/modules/nrfx/drivers/src”
3. To open the project go to File > Open solution…
insect_ble_firmware\pca10040\s132\ses\image_transfer_demo_pca10040_s132.emProject
