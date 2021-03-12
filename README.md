# UnityAndESP32_BLEExamples
BLE examples between unity and esp32. this repo used Unity Store Asset "Bluetooth LE for iOS, tvOS, android"

## DEMO

https://www.youtube.com/watch?v=wEsxTf1MekU

## notice

You must to buy Unity Store Asset below to run this repo's BLE code for Unity.

Bluetooth LE for iOS, tvOS, android
https://www.assetstore.unity3d.com/#!/content/26661/directpurchase?aid=1101lGs4 (Unity Asset Store Link)

# Unity 

## SimpleTest

* Rotate cube by notify it include quaternion.
* Search and auto-connect to notify device it need to match 3 infomation. Device Name, Service UUID, Characteristic UUID.
* notify data is 16bytes array. it is serialized quaternion(4 floats).

# ESP32

* Arduino folder include code for arduino-esp32

## Arduino BLE_IMU

* this code is needed bno055, it is IMU.
* Get quaternion by IMU(bno055).
* Send data as notify.



