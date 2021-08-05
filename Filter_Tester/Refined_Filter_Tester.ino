/* Filter Tester - Program controls 
fan speed with LPM variable to calculate 
differnetial pressure PSI at the mask */

// Filter Tester Libraries
#include <Wire.h> // Wire library
#include <sdpsensor.h> // SDP sensor library
#include <MS5525DSO.h> // MS5525DSO library
#include <font_Arial.h> // ILI9341  Arial Font Library
#include <font_ArialBlack.h> // ILI9341  Arial Bold Font Library
#include "Atorlogo2.c" // AtorLABS Splash Image
#include <ZzzMovingAvg.h> // Sensor Smoothing Library

// ILI9341 HMI Pin Definitions
#define BACKLIGHT_PIN   3 
#define TFT_DC          20
#define TFT_CS          21
#define TFT_RST         255  // 255 = unused. connect to 3.3V
#define TFT_MOSI        7
#define TFT_SCLK        14
#define TFT_MISO        12
// ASCII RGB Definitions
#define AtorBlue (11, 84, 139)

// Library Constructors
ILI9341_t3 HMI = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO); // Call ILI9341's constructor
SDP8XXSensor flowSensor; // Call SDP sensor's constructor
MS5525DSO diffSensor(pp001DS); // Call MS5525DSO sensor's constructor
ZzzMovingAvg <10, float, float> avg; // Sensor Smoothing Constructor

// Initialize Variables
double flowPressure; // SDP Pressure
double result; // flowPressure Smoothed 
double diffPressure; // MS5525DSO Pressure
double paToflow; // flowPressure converted to flow
double mmH20; // diffPressure converted to mmH20
double fanSpeed = 90; // PWM Fan Speed
int ret; // Returned value of SDP Pressure Validation Sequence
int Switch; // Switch ON/OFF

// Main Setup
void setup() // put your setup code here, to run once:
{
  // Device Setups
  HMI.begin(); // Begin SPI comm link with ILI9341 Display
  Wire.begin(); // Begin i2c comm link
  Serial.begin(115300);

  // Pin Setups
  pinMode(2, OUTPUT); // PWM FAN CONTROL
  pinMode(4, OUTPUT); // FILTER INSERTION SWITCH
  pinMode(BACKLIGHT_PIN, OUTPUT);
  
  // Adjust HMI backlight
  analogWrite(BACKLIGHT_PIN, 255); // 0 to 255 / LOW to HIGH
  ret = flowSensor.init();
  
  delay(2000); // 2 Second Delay
  
  // Methods
  Sensor_Val(); // Check if sensors started up
  Ator_Splash_Screen();// HMI Screen Splash Screen
  Second_Screen(); // Generate Second HMI Screen
}

// Main Loop
void loop() // put your main code here, to run repeatedly:
{
  Switch = digitalRead(4); // Read Switch ON/OFF
  ret = flowSensor.readSample(); // Attempt to read from SDP sensor

  if (ret == 0 && diffSensor.begin(I2C_MS5525DSO_ADDR_ALT)) { // If SDP sensor gives a Pressure readout
    // Flow Pressure
    flowPressure = flowSensor.getDifferentialPressure();
    Serial.printf("Value added: %lf | Average = ", flowPressure);
    Serial.println(avg.add(flowPressure));
    result = avg.get();
    // paToflow = (-0.00004 * result ** 2
    HMI.setCursor(60, 60);
    HMI.setFont(Arial_16);
    HMI.fillRect(20, 50, 210, 45, ILI9341_WHITE);
    HMI.setTextColor(ILI9341_BLACK);
    HMI.print(paToflow, 2);
    HMI.print(" LPM");
    
    // Differential Pressure
    diffSensor.readPressureAndTemperature(&diffPressure);
    HMI.setCursor(35, 160);
    HMI.fillRect(20, 150, 210, 45, ILI9341_WHITE);
    HMI.setFont(Arial_16);
    HMI.setTextColor(ILI9341_BLACK);
    mmH20 = diffPressure * 703.08893732448;
    HMI.print(mmH20, 2);
    HMI.print(" mmH2O");
    
    delay(500);

    // Start Up Fan Speed Adjustment
    if (result < 41.5 && Switch == LOW && fanSpeed <= 255) {
        fanSpeed += 1;
        analogWrite(2, fanSpeed);
        Serial.println(fanSpeed);
        }
    if (result > 46) {
      fanSpeed -= 1;
      analogWrite(2, fanSpeed);
      Serial.println(fanSpeed);
      delay(200);
    }
    // Post Filter Removal Fan Speed Re-Adjustment
    if (flowPressure > 70 && Switch == LOW) {
      fanSpeed = 90;
      analogWrite(2, fanSpeed);
      if (result < 41.5 && fanSpeed < 255) {
        fanSpeed += 1;
        analogWrite(2, fanSpeed);}}
    }
    else {
      Sensor_Val();
      if (ret == 0 && diffSensor.begin(I2C_MS5525DSO_ADDR_ALT)) {
        Second_Screen();
      }
    }
} // MAIN LOOP

// HMI Splash Screen
void Ator_Splash_Screen() // HMI Splash Screen Function
{
  // Create Splash Screen
  HMI.setRotation(4);
  HMI.fillScreen(ILI9341_WHITE);
  HMI.writeRect(15, 75, AtorDrag.width, AtorDrag.height, (uint16_t*)(AtorDrag.pixel_data));
  HMI.drawRect(5, 5, 230, 310, AtorBlue);
  HMI.drawRect(6, 6, 227, 307, AtorBlue);
  HMI.drawRect(7, 7, 225, 305, AtorBlue);
  HMI.drawRect(8, 8, 226, 306, AtorBlue);
  HMI.setCursor(15, 160);
  HMI.setTextColor(AtorBlue);
  HMI.setTextSize(2);
  HMI.setFont(ArialBlack_18);
  HMI.print("FILTER CHECK");
  delay(5000); // 5 Second Delay
} // End of Ator_Splash_Screen()

// HMI Second Screen
void Second_Screen() 
{
  // Create Second Screen
  HMI.fillRect(1, 1, 240, 320, ILI9341_WHITE);
  HMI.drawRect(5, 5, 230, 310, AtorBlue);
  HMI.drawRect(6, 6, 227, 307, AtorBlue);
  HMI.drawRect(7, 7, 225, 305, AtorBlue);
  HMI.drawRect(8, 8, 226, 306, AtorBlue);
  HMI.setFont(ArialBlack_24);
  HMI.setCursor(60, 20);
  HMI.setTextColor(ILI9341_BLACK);
  HMI.setFont(Arial_24);
  HMI.print("Air Flow \n");
  HMI.setCursor(15, 120);
  HMI.setTextColor(ILI9341_BLACK);
  HMI.setFont(Arial_24);
  HMI.print("Filter Pressure \n");
} // End of Second_Screen()

// Sensor Validation
void Sensor_Val() // Sensor Start Up Validation Function
{
  while (ret != 0 || !diffSensor.begin(I2C_MS5525DSO_ADDR_ALT)) { // Otherwise Throw Error Code
    HMI.setRotation(4);
    HMI.fillScreen(ILI9341_RED);
    HMI.setCursor(60, 65);
    HMI.setTextColor(ILI9341_BLACK);
    HMI.setFont(Arial_24);
    HMI.print("ERROR!");
    HMI.setCursor(10, 150);
    HMI.setFont(Arial_14);
    HMI.print("Sensor(s) Failed To Start!");
    analogWrite(2, 0);
    ret = flowSensor.init();
    delay(1000);}
} // End of Sensor_Val()
