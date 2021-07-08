int bitRate = 25;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(3, OUTPUT); //sets the digital pin 3 as an output
  for (int flow = 0; flow < 100; flow += 5) {
    analogWrite(3, bitRate);
    delay(1000);
    bitRate += 5;
    Serial.print(bitRate);
    Serial.println(" Bit Rate");
    Serial.print(flow);
    Serial.println(" Flow Rate");}
}

void loop() {
  // put your main code here, to run repeatedly:
  }
