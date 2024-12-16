#include <MLX90641.hpp>
 
#include <Wire.h>
#include <Arduino.h>
 
constexpr uint8_t temp_sensor_refreash_rate{4U};
MLX90641 temp_sensor; 
bool sensor_init_succeded{false};

void setup()
{
  delay(5000);
  Serial.begin(115200);
  while (!Serial); // Wait for Serial
  Serial.printf("\nAttempting to start serial comms");
  Serial.printf("\nBegin startup. Arduino version: %d\n", ARDUINO);//
  Serial.printf("Address of Wire object: %d", &Wire);
  // Wire.begin(); // initialize I2C
  if(!temp_sensor.initialise(temp_sensor_refreash_rate, &Wire))
  {
    Serial.printf("\nFailed to initialize the temperature sensor\n");
  }
  else {
     Serial.printf("\nTemperature sensor successfully initialized!\n");
     sensor_init_succeded = true;
  }
}
 
uint8_t iteration{0};
void loop()
{
  iteration++;
  Serial.printf("In main loop, iteration:%d\n ", iteration);
  if (sensor_init_succeded)
  {
  } else {
    Serial.println("The init failed");
  }
  delay(5000);        
}