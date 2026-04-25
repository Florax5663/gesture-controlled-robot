#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// --- MOTOR PINS ---
#define ENA 6   
#define IN1 5   
#define IN2 4   
#define ENB 3   
#define IN3 7   
#define IN4 2   

// --- ULTRASONIC PINS ---
#define TRIG_PIN 8
#define ECHO_PIN A0

RF24 radio(9, 10); 
const byte address[6] = "00001"; 

struct Data_Package {
  float tiltX;
  float tiltY;
};
Data_Package data;

unsigned long lastReceiveTime = 0;

int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 25000); 
  
  if (duration == 0) return 999; 
  
  int distance = duration * 0.034 / 2;
  return distance;
}

void setup() {
  // Upgraded to 115200 for zero-lag printing!
  Serial.begin(115200);
  
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN); 
  radio.setDataRate(RF24_250KBPS);     
  radio.setChannel(108);               
  radio.setPayloadSize(sizeof(Data_Package));
  radio.setAutoAck(false);             
  radio.startListening();             

  Serial.println("====================================");
  Serial.println("CAR RECEIVER BOOTED: TELEMETRY LIVE");
  Serial.println("====================================");
}

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package));
    lastReceiveTime = millis(); 
  }

  // RADIO FAILSAFE
  if (millis() - lastReceiveTime > 500) { 
    analogWrite(ENA, 0); analogWrite(ENB, 0);
    digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
    
    // Print every 500ms so it doesn't spam the monitor too fast
    static unsigned long lastFailsafePrint = 0;
    if (millis() - lastFailsafePrint > 500) {
        Serial.println("⚠ FAILSAFE: Connection Lost! Motors Stopped.");
        lastFailsafePrint = millis();
    }
    return; 
  }

  int tiltX = (int)data.tiltX; 
  int tiltY = (int)data.tiltY;
  int speed = 0;
  String action = "STOP";
  
  // --- COLLISION RADAR ---
  int obstacleDist = getDistance();
  
  // Auto-Brake Override
  if (tiltY < -2 && obstacleDist < 15) {
    analogWrite(ENA, 0); analogWrite(ENB, 0);
    digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
    
    Serial.print("RADAR: "); Serial.print(obstacleDist);
    Serial.println("cm | ⚠ CRASH AVOIDED! Auto-Braking!");
    
    delay(50); 
    return; 
  }

  // --- NORMAL DRIVING LOGIC ---
  if (tiltY < -2) { 
    speed = constrain(map(tiltY, -2, -10, 150, 255), 150, 255); 
    action = "FORWARD";
    analogWrite(ENA, speed); analogWrite(ENB, speed);
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); 
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); 
  }
  else if (tiltY > 2) {
    speed = constrain(map(tiltY, 2, 10, 150, 255), 150, 255);
    action = "BACKWARD";
    analogWrite(ENA, speed); analogWrite(ENB, speed);
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); 
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); 
  }
  else if (tiltX > 2) {
    speed = constrain(map(tiltX, 2, 10, 150, 255), 150, 255);
    action = "LEFT";
    analogWrite(ENA, speed); analogWrite(ENB, speed);
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); 
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); 
  }
  else if (tiltX < -2) {
    speed = constrain(map(tiltX, -2, -10, 150, 255), 150, 255);
    action = "RIGHT";
    analogWrite(ENA, speed); analogWrite(ENB, speed);
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); 
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); 
  }
  else {
    analogWrite(ENA, 0); analogWrite(ENB, 0);
    digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  }

  // --- LIVE TELEMETRY PRINT ---
  Serial.print("Dist: "); 
  if (obstacleDist == 999) Serial.print("CLEAR ");
  else { Serial.print(obstacleDist); Serial.print("cm "); }
  
  Serial.print(" | X: "); Serial.print(tiltX);
  Serial.print(" | Y: "); Serial.print(tiltY);
  Serial.print(" | Speed: "); Serial.print(speed);
  Serial.print(" | Action: "); Serial.println(action);
}