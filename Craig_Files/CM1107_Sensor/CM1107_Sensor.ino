#include <cm1106_i2c.h> // CM1106 & CM1107 Sensor Constructor

CM1106_I2C cm1106_i2c; // CM1106 & CM1107 Sensor Constructor

#define CM1107 31 // Define Sensor So Library Can Detect Alternate Device

void setup() { // Code Ran Once
  cm1106_i2c.begin(); // Begin Sensor Communication
  Serial.begin(9600); // Begin Serial Communication
  delay(1000); // 1 Sec Delay
  cm1106_i2c.read_serial_number(); Serial.println();// Display CM1107's Serial Number
  delay(1000); // 1 Sec Delay
  cm1106_i2c.check_sw_version(); Serial.println(); // Check Software Version
  delay(1000); // 1 Sec Delay
}

void loop() { // Looped Code
  uint8_t ret = cm1106_i2c.measure_result(); // Check If Sensor Has Given An Output

  if (ret == 0) { // If measure_result() returned 0
    Serial.print("Co2 : ");
    Serial.println(cm1106_i2c.co2); // Log Output Of .co2 Method To Console
    Serial.print("Sensor Status >> ");


#if defined(CM1107)
    if (cm1106_i2c.status & (1 << CM1106_I2C_STATUS_CM1107_PREHEATING)) {
      Serial.print("Preheating,");
    } else {
      Serial.print("Preheat complete,");
    }

    if (cm1106_i2c.status & (1 << CM1106_I2C_STATUS_CM1107_OPERATING_NORMAL)) {
      Serial.print(" Sensor Error,");
    } else {
      Serial.print(" Operating normal,");
    }

    if (cm1106_i2c.status & (1 << CM1106_I2C_STATUS_CM1107_OVER_MEASUREMENT_RANGE)) {
      Serial.print(" WARNING: Over Measurement Range!,");
    } 

    if (cm1106_i2c.status & (1 << CM1106_I2C_STATUS_CM1107_LESS_THAN_MEASUREMENT_RANGE)) {
      Serial.print(" WARNING: Less than Measurement Range!,");
    }

    if (cm1106_i2c.status & (1 << CM1106_I2C_STATUS_CM1107_CALIBRATED)) {
      Serial.print(" Non-calibrated,");
    } else {
      Serial.print(" Calibrated,");
    }

    if (cm1106_i2c.status & (1 << CM1106_I2C_STATUS_CM1107_LIGHT_AGING)) {
      Serial.print(" Light Aging,");
    } else {
      Serial.print(" Light Normal,");
    }

    if (cm1106_i2c.status & (1 << CM1106_I2C_STATUS_CM1107_DRIFT)) {
      Serial.print(" Drift\n");
    } else {
      Serial.print(" No Drift Detected\n");
    }
    Serial.println();
#endif
  } else {
    Serial.println("Sensor failed to give an output");
  }
  delay(1000);
}
