// motors
const int PWMA = 9;   // fizyczny pin 15 ATmegi
 
const int AIN1 = 10;
const int AIN2 = 11;
 
const int PWMB = 6;   // fizyczny pin 12 ATmegi
const int BIN1 = 8;   // fizyczny pin 14 ATmegi
const int BIN2 = 7;   // fizyczny pin 13 ATmegi
 
// sensors
const int sensorPins[6] = {A5, A4, A3, A2, A1, A0}; 
const int prog = 900; 
 
// pid
float Kp = 3.0;  
float Kd = 5.0;  
float Ki = 0.0;  
 
int lastError = 0;
int integral = 0;
 

int ostatniaZanotowanaPozycja = 0; 
 
// speed
const int predkoscMax = 255; 
 
void setup() {
  Serial.begin(9600);
 
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
 
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
 
  ustawSilniki(0, 0);
  delay(2000); 
}
 
void loop() {
  // sensor read
  int errorSum = 0;
  int activeSensors = 0;
  bool s[6];
 
  int weights[6] = {-50, -30, -10, 10, 30, 50}; 
 
  for (int i = 0; i < 6; i++) {
    s[i] = analogRead(sensorPins[i]) > prog;
    if (s[i]) {
      errorSum += weights[i];
      activeSensors++;
    }
  }
 
  int error = 0;
  int currentBaseSpeed = 0;
 
  if (activeSensors > 0) {
    error = errorSum / activeSensors;
    ostatniaZanotowanaPozycja = error; 
    currentBaseSpeed = predkoscMax - (abs(error) * 2.0); 
    if (currentBaseSpeed < 90) currentBaseSpeed = 90; 
 
  } else {
    if (ostatniaZanotowanaPozycja > 0) {
      error = 85;
    } else {
      error = -85;
    }
  }
 
  // calculating pid
  int P = error;
  int I = integral + error;
  int D = error - lastError;
 
  int correction = (Kp * P) + (Ki * I) + (Kd * D);
 
  lastError = error;
  integral = I;
 
  int speedLeft = currentBaseSpeed + correction; 
  int speedRight = currentBaseSpeed - correction;
 
  speedLeft = constrain(speedLeft, -255, 255);
  speedRight = constrain(speedRight, -255, 255);
 
  ustawSilniki(speedLeft, speedRight);
}
 
// motor steering
void ustawSilniki(int mocLewa, int mocPrawa) {
  // left motor (A)
  if (mocLewa >= 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, mocLewa);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    analogWrite(PWMA, -mocLewa); 
  }
 
  // right motor (B)
  if (mocPrawa >= 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, mocPrawa);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    analogWrite(PWMB, -mocPrawa);
  }
}
