# BaslerCameraHardwareTrigger
Basler Camera using hardware trigger
# Description
This is a simple program used for grabbing image from a Basler USB3 camera, trigger signal generated from Arduino microcontroller

# Dependencies
1.OpenCV3

2.Pylon SDK

--OpenCV3 is used for display image grabbed and write image to disk
In case you don't want to display images or you have your own function for saving image to disk, for free to disable OpenCV part

--Pylon SDK
Since we are using Basler camera, Pylon SDK must be installed on your system

You could follow the instruction from Pylon documentation, https://www.baslerweb.com/en/products/software/basler-pylon-camera-software-suite/, select the corresponding documentation e.g. I'm using Basler acA1300-200um

# Usage
cd BaslerCameraHardTrigger

mkdir build

cd build

cmake ..

make

../bin/BaslerCameraHardTrigger

# Parameter
--camConfig:Camera configuration file, you could load camera parameters from configuration file before grabbing images

--saveImgDir:Directory where you would like to put images in

--imgPrefix: Prefix of image filename

--imgPostfix:Postfix of image filename

--showView:bool whether display image grabbed or not

