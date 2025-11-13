// Motor A
int motorA1 = 25;  // foward 1
int motorA2 = 26;  // back 2

// Motor B
int motorB1 = 14;  // foward 1
int motorB2 = 27;  // back 2

// Sensors
int sensorLeft  = 34;
int sensorRight = 35;

int Vaccumm = 21;



// The sensors were found to be largely effected by the power the were receving
// This function helps to mitigate that
int callibrate(int pin){
  double total = 0;
  int iterations = 500;
  for (int i = 0; i < iterations; i++){
    total += analogRead(pin);
    delay(10);
  }
  return (total / iterations) - ((total / iterations) / 5);
}


void VaccummSwitch(){
  digitalWrite(Vaccumm,LOW);
  delay(20);
  digitalWrite(Vaccumm,HIGH);
  delay(500);
  digitalWrite(Vaccumm,LOW);
  delay(20);
}


int clampSpeed(int s) {
  if (s > 255) return 255;
  if (s < -255) return -255;
  return s;
}

// Drive any H-bridge pair with one signed speed
void setMotorPair(int pin1, int pin2, int speed) {
  speed = clampSpeed(speed);
  if (speed > 0) {
    analogWrite(pin1, speed);
    analogWrite(pin2, 0);
  } else if (speed < 0) {
    analogWrite(pin2, -speed);
    analogWrite(pin1, 0);
  } else {
    analogWrite(pin1, 0);
    analogWrite(pin2, 0);
  }
}


void setMotorA(int speed) { setMotorPair(motorA1, motorA2, speed); }
void setMotorB(int speed) { setMotorPair(motorB1, motorB2, speed); }

void FullStop(){
  analogWrite(motorA1,0);
  analogWrite(motorA2,0);
  analogWrite(motorB1,0);
  analogWrite(motorB2,0);
  delay(50);
}

// Control Functions
void forward(int speed) {
  setMotorA(speed);
  setMotorB(speed);
}
void stopMotors() {
  setMotorA(0);
  setMotorB(0);
}
void turnLeft(int speed) {
  setMotorA(-speed);
  setMotorB(speed);
}
void turnRight(int speed) {
  setMotorA(speed);
  setMotorB(-speed);
}

int thresholdRight;
int thresholdLeft;


void setup() {
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);
  pinMode(sensorLeft, INPUT);
  pinMode(sensorRight, INPUT);
  pinMode(Vaccumm, OUTPUT);

  Serial.begin(115200);
  Serial.println("Dual motor with obstacle detection ready");
  thresholdLeft = callibrate(sensorLeft);
  thresholdRight = callibrate(sensorRight);

  VaccummSwitch();
}

void loop() {
  int leftDetect  = analogRead(sensorLeft);
  int rightDetect = analogRead(sensorRight);

  if (leftDetect > thresholdLeft && rightDetect > thresholdRight) {
    // Nothing detected - go forward
    forward(255);
    Serial.print(leftDetect);
    Serial.print(", ");
    Serial.print(rightDetect);
    Serial.println("...................................");
  } 
  else if (leftDetect < thresholdLeft) {
    // Left sensor sees obstacle - turn right
    Serial.println("Obstacle left → turn right");
    FullStop();
    turnRight(255);
    delay(1000);
  } 
  else if (rightDetect < thresholdRight) {
    // Right sensor sees obstacle - turn left
    Serial.println("Obstacle right → turn left");
    FullStop();
    turnLeft(255);
    delay(1000);
  }

  delay(50);
}
