const int pumpPin = 25; // Pin connected to the water pump
const int maxSpeed = 90; // Maximum PWM value (corresponds to 100% duty cycle)
const int minSpeed = 80; // Minimum PWM value (corresponds to 0% duty cycle)
const int fadeDelay = 10; // Delay between speed adjustments in milliseconds

void setup() {
  pinMode(pumpPin, OUTPUT); // Set pump pin as output
}

void loop() {
  waterflow();
}

void waterflow(){
  // Gradually increase the pump speed
  for (int speed = minSpeed; speed <= maxSpeed; speed++) {
    analogWrite(pumpPin, map(speed, minSpeed, maxSpeed, 0, 255)); // Set pump speed
    delay(fadeDelay); // Wait before adjusting speed again
  }
  
  // Gradually decrease the pump speed
  for (int speed = maxSpeed; speed >= minSpeed; speed--) {
    analogWrite(pumpPin, map(speed, minSpeed, maxSpeed, 0, 255)); // Set pump speed
    delay(fadeDelay); // Wait before adjusting speed again
  }
}