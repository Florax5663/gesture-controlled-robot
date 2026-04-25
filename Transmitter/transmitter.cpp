#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;
RF24 radio(9, 10);
const byte address[6] = "00001";

struct Data_Package {
  float tiltX;
  float tiltY;
};
Data_Package data;

void setup() {
  Serial.begin(9600);
  
  // I2C Anti-Freeze Timeout
  Wire.begin();
  Wire.setWireTimeout(3000, true); 
  
  // Direct startup without 'if' checks
  mpu.begin();
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  
  // TWEAK #4: The "Reflexes" (Faster, punchier sensor reading)
  mpu.setFilterBandwidth(MPU6050_BAND_44_HZ); 
  
  // Direct startup without 'if' checks
  radio.begin();

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);       
  radio.setDataRate(RF24_250KBPS);     
  radio.setChannel(108);               
  radio.setPayloadSize(sizeof(Data_Package)); 
  radio.setAutoAck(false);             
  radio.stopListening();               
  
  Serial.println("Glove SENDER: Race Mode Active on Channel 108!");
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  data.tiltX = a.acceleration.x;
  data.tiltY = a.acceleration.y;

  radio.write(&data, sizeof(Data_Package));

  Serial.print("SENDING -> Tilt X: "); 
  Serial.print(data.tiltX);
  Serial.print("  |  Tilt Y: "); 
  Serial.println(data.tiltY);

  // TWEAK #3: The "Adrenaline" (Broadcasting 100 times a second)
  delay(10); 
}