#define STEPPER_PIN_1 16
#define STEPPER_PIN_2 17
#define STEPPER_PIN_3 5
#define STEPPER_PIN_4 18

#include <WiFi.h>
#include <LCD_I2C.h>
#include <Stepper.h>
#include <MQTT.h>

Stepper stepper(2048, STEPPER_PIN_1, STEPPER_PIN_2, STEPPER_PIN_3, STEPPER_PIN_4);
LCD_I2C lcd(0x27, 16, 2);
int MQTT_PORT = 1883;


const char ssid[] = "@JumboPlusIoT";
const char pass[] = "gv0wob4a";
const char mqtt_broker[] = "test.mosquitto.org";
const char mqtt_topic[] = "261215/brewbuddy";               // main topic to controll driping machine
const char mqtt_topic_brewing[] = "261215/brewbuddy/time";  // topic for time publish
const char mqtt_client_id[] = "esp32_brewbuddy";            // must change this string to a unique value


WiFiClient net;
MQTTClient client;

int mode = 0;

unsigned long lastMoveMillis = 0;
const unsigned long moveInterval = 5;  // Set the interval in milliseconds

const int pumpPin = 25;    // Pin connected to the water pump
const int maxSpeed = 90;   // Maximum PWM value (corresponds to 100% duty cycle)
const int minSpeed = 80;   // Minimum PWM value (corresponds to 0% duty cycle)
const int fadeDelay = 10;  // Delay between speed adjustments in milliseconds

int loopCount = 0;  // Initialize loop count

// void connect() {
//   Serial.print("Checking wifi...");
//   while (WiFi.status() != WL_CONNECTED) {
//     Serial.print(".");
//     delay(1000);
//   }

//   Serial.print("\nConnecting...");
//   while (!client.connect(mqtt_client_id)) {
//     Serial.print(".");
//     delay(1000);
//   }

//   Serial.println("\nConnected!");

//   client.subscribe(mqtt_topic);
//   client.subscribe(mqtt_topic_brewing);
// }

// void messageReceived(String &topic, String &payload) {
//   Serial.println("incoming: " + topic + " - " + payload);
//   if (payload == "Unclockwise") {
//     mode = 1;
//     printLed("Unclockwise");
//   } else if (payload == "Clockwise") {
//     mode = 2;
//     printLed("Clockwise");
//   } else if (payload == "idle") {
//     mode = 3;
//     printLed("Idle");
//   }
// }

// void publishTimeToMQTT(int remainingTime) {
//   String timePayload = String(remainingTime) + "s";
//   client.publish(mqtt_topic_brewing, timePayload.c_str());
// }

void setup() {
  //I2C and pinMode setup part
  lcd.begin();
  lcd.backlight();
  pinMode(STEPPER_PIN_1, OUTPUT);
  pinMode(STEPPER_PIN_3, OUTPUT);
  pinMode(STEPPER_PIN_2, OUTPUT);
  pinMode(STEPPER_PIN_4, OUTPUT);
  Serial.begin(9600);
  pinMode(pumpPin, OUTPUT);  // Set pump pin as output

  //Wifi Connection part
  // WiFi.begin(ssid, pass);
  // client.begin(mqtt_broker, MQTT_PORT, net);
  // client.onMessage(messageReceived);
  // connect();
}

// unsigned long startTime;
// int brewingTimeInSeconds = 300;  // 5 minutes (adjust as needed)

// void timeCount() {
//   unsigned long elapsedTime = millis() - startTime;
//   int remainingTime = (brewingTimeInSeconds * 1000 - elapsedTime) / 1000;

//   if (remainingTime >= 0) {
//     printLed("Brewing", String(remainingTime) + "s");
//   } else {
//     printLed("Idle", "0s");
//     // Additional actions when the timer reaches zero can be added here
//   }
// }

void loop() {
  unsigned long currentMillis = millis();

  // Move the stepper motor
  if (currentMillis - lastMoveMillis >= moveInterval) {
    moveMotors(STEPPER_PIN_4, STEPPER_PIN_3, STEPPER_PIN_2, STEPPER_PIN_1);
    lastMoveMillis = currentMillis;
  }

  while (loopCount < 6) {                // Loop 5 times
    unsigned long startTime = millis();  // Record the start time

    while (millis() - startTime < 10500) {
      waterflow();
    }
    delay(2000); 
    loopCount++;
  }
}


void printLed(String state, String time) {
  lcd.clear();
  lcd.print("Status:");
  lcd.print(state);
  lcd.setCursor(0, 1);
  lcd.print("Time ramains:");
  lcd.print(time);
}

void moveMotors(int pin1, int pin2, int pin3, int pin4) {
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
  delay(5);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, HIGH);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
  delay(5);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, HIGH);
  digitalWrite(pin4, LOW);
  delay(5);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, HIGH);
  delay(5);
}

void waterflow() {
  for (int speed = minSpeed; speed <= maxSpeed; speed++) {
    analogWrite(pumpPin, map(speed, minSpeed, maxSpeed, 0, 255));  // Set pump speed
    delay(fadeDelay);                                              // Wait before adjusting speed again
  }

  // Gradually decrease the pump speed
  for (int speed = maxSpeed; speed >= minSpeed; speed--) {
    analogWrite(pumpPin, map(speed, minSpeed, maxSpeed, 0, 255));  // Set pump speed
    delay(fadeDelay);                                              // Wait before adjusting speed again
  }
}