#define PWM 2 // PWM Signal from pin 2 on the Teensy 2.5
 int pinA = 5; // Connected to CLK on KY-040
 int pinB = 4; // Connected to DT on KY-040
 int encoderPosCount = 0; 
 int pinALast; 
 int aVal;
 int bVal;
 boolean bCW;
 double dc_percent;
 double pos;
 
 void setup() { 
   pinMode(pinA, INPUT);
   pinMode(pinB, INPUT);
   pinMode(PWM, OUTPUT);
   //analogWriteFrequency(PWM, 25000);  // Set PWM Frequency to 25KHz
   
   // Read Pin A - Whatever state it's in will reflect the last position 
   pinALast = digitalRead(pinA); 
   Serial.begin(9600);
 } 
 void loop() {
   aVal = digitalRead(pinA);
   bVal = digitalRead(pinB);
   if (aVal != pinALast){ // Means the knob is rotating
     // if the knob is rotating, we need to determine direction
     // We do that by reading pin B.
     if (bVal != aVal && encoderPosCount < 255) { // Means pin A Changed first - We're Rotating Clockwise
       bCW = true;
       encoderPosCount += 1;
       analogWrite(PWM, encoderPosCount);
     }
   if (bVal == aVal && encoderPosCount > 0){ // Otherwise B changed first and we're moving CCW
     bCW = false;
     encoderPosCount -= 1;
     analogWrite(PWM, encoderPosCount);
   }
   Serial.printf("PWM Position: %d\n", encoderPosCount);
   pos = encoderPosCount;
   dc_percent = ((pos / 255) * 100);
   Serial.printf("PWM Percent: %lf%\n", dc_percent);
   
   } 
   pinALast = aVal;
 }
