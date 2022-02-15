#define Enable 1
#define ControlLock 2
#define PWM 3

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(600);
  pinMode(Enable, OUTPUT);
  pinMode(ControlLock, OUTPUT);
  pinMode(PWM, OUTPUT);
  delay(1000);
  digitalWrite(Enable, HIGH);
  delay(1000);
  digitalWrite(ControlLock, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(PWM, 255);
  delay(1500);
  analogWrite(PWM, 1);
  delay(1500);
}
