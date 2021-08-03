# DragTapVib

**DragTapVib** is an electromagnetic wearable actuator.


## Manufacturing and Assembly

1. Print *DragTapVib_bottom.stl*, *DragTapVib_tactor.stl* and *DragTapVib_upper.stl* under [DragTapVib_3D](https://github.com/merlinz165/DragTapVib/tree/main/DragTapVib_3D) with 3D printer;
1. Use double-sided tape to adhere the tactor to a 10x3mm permanent magnet.
1. Assemble the actuator according to the following diagram:

<center>
<img src="https://lsp4dmodel.s3.eu-central-1.amazonaws.com/DragTapVib/dragtapvib_assemble.png" width="800">
</center>

Installation can refer to this sketch:

<center>
<img src="https://lsp4dmodel.s3.eu-central-1.amazonaws.com/DragTapVib/sketch_arduino.png" width="300">
</center>

## Android App

One application of **DragTapVib** is as a wireless notifier for smartphone Notifications, and we have developed a sample app (BLENotification) that implements this.

The code of the Android app is in the directory named BLENotificationAndroid, and the corresponding ESP32 control code is in the directory named BLENotification_ESP32.

Installation is shown in the following sketch:

<center>
<img src="https://lsp4dmodel.s3.eu-central-1.amazonaws.com/DragTapVib/esp32_sketch.png" width="300">
</center>

A demo video of the BLENotification App [can be found here](https://youtu.be/wV7LtIC8myA).
