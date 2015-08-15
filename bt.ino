#include <SoftwareSerial.h>
#include <Servo.h>

// General stuff
const long BAUD_RATE = 115200;
unsigned long interval = 10000;
unsigned long lastTime;

// Bluetooth stuff
SoftwareSerial mySerial(2, 3);
char laptop[] = "34363BC3D57B";
bool isConnected = false;

// Servo stuff
const int SERVO_PIN = 4;
const int FLAG_UP = 180;
const int FLAG_DOWN = 90;
int position = FLAG_DOWN;
Servo servo;

void setup()
{
  // Initialize USB Serial for monitoring
  Serial.begin(BAUD_RATE);
  Serial.println("ready!");

  // Initialize Software Serial
  mySerial.begin(BAUD_RATE);
  
  // Take Bluetooth modem out of command mode (in case it was)
  mySerial.println("---");

  // Initialize servo pin and position
  servo.attach(SERVO_PIN);
  servo.write(position);
}

void loop()
{
  unsigned long currentTime = millis();

  if ((currentTime - lastTime) > interval) {
    lastTime = currentTime;

    // Set Bluetooth modem to command mode
    Serial.println("$$$");
    mySerial.print("$$$");
    delay(200);

    // Empty serial from any available strings before asking 
    // for connection status
    flushSerial();

    // Ask for connection status
    Serial.println("GK");
    mySerial.println("GK");
    delay(200);

    // Ideally, we would get only 1.0.0 or 0.0.0
    // but there might be some extra output.
    int len = mySerial.available();
    char buffer[20];

    if (len > 0) {
      // Read Serial data until we find the first 1 or 0
      for (int i = 0; i < len; i++) {
        char c = mySerial.read();
        Serial.print(c);
        if (c == '0' || c == '1') {
          isConnected = c == '1';
          flushSerial();
          break;
        }
      }
    }
    
    if (isConnected) {
      // If Bluetooth Modem is connected, ask the servo
      // to take the flag up
      // Serial.println("> connected");
      
      position = FLAG_UP;
    } else {
      // If Bluetooth Modem is not connected, ask the servo
      // to take the flag down ...
      // Serial.println("> disconnected");
      position = FLAG_DOWN;

      // ... and attempt to connect
      char command[14] = "C,";
      strcat(command, laptop);
      Serial.print("< ");
      Serial.println(command);
      mySerial.println(command);
    }
    
    // Update servo to last known status
    servo.write(position);

    // Take Bluetooth modem out of command mode
    mySerial.println("---");
  }
}

// Empty Serial data
void flushSerial() {
  for (int i = 0; i < mySerial.available(); i++) {
    Serial.print((char)mySerial.read());
  }
}

