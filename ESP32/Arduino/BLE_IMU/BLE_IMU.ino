//bno055周り
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055(-1,BNO055_ADDRESS_B);

//BLE周り
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t values[16];

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

//2220,2221使うので設定。5～8桁目以外は共通
#define SERVICE_UUID        "00002220-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "00002221-0000-1000-8000-00805f9b34fb"


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};



void setup() {
//  Wire.begin(21,22);
  Wire.begin(22,23);//for Feather ESP32
//  Wire.begin(5,4);//for ESP32 with OLED board
  Serial.begin(115200);
  //bno055 init
  delay(100);
  while(!bno.begin())
  {
    Serial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    delay(100);
  }

  bno.setExtCrystalUse(true);
  // Create the BLE Device
  BLEDevice::init("MyESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}
//floatをバイト列に変換する方法としてunionを選択
union thing{
  float d;
  uint8_t b[8];
};
//バイト列にfloatバイト列を代入する処理
void setFloatToArray(uint8_t array[], uint8_t index, union thing v)
{
  for(uint8_t i=0;i<4;i+=1)
    array[index+i]=v.b[i];
}

void loop() {

//以下の一行でquaternion取得できる
    imu::Quaternion q = bno.getQuat();
    union thing things[4];
//doubleだと長さ足りない（>23バイト）のでfloatにキャスト    
    things[0].d=(float)q.x();
    things[1].d=(float)q.y();
    things[2].d=(float)q.z();
    things[3].d=(float)q.w();
//単体確認用出力.傾けると連続して動くはず    
    Serial.print(things[0].d);
    Serial.print(" ");
    Serial.print(things[0].b[0],HEX);
    Serial.print(" ");
    Serial.print(things[0].b[1],HEX);
    Serial.print(" ");
    Serial.print(things[0].b[2],HEX);
    Serial.print(" ");
    Serial.print(things[0].b[3],HEX);
    Serial.print(" ");
    Serial.print(sizeof(things[0].d));
    Serial.println();
//送信バイト列に代入    
    for(uint8_t i=0;i<4;i+=1)
      setFloatToArray(values,i*4,things[i]);
    // notify changed value
    if (deviceConnected) {
        pCharacteristic->setValue(values, 16);
        pCharacteristic->notify();
        delay(10); // bluetooth stack will go into congestion, if too many packets are sent
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
