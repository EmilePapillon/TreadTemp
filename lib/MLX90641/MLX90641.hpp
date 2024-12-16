#include <MLX90641_API.hpp>
#include <Arduino.h>
#include <Wire.h>

constexpr uint8_t TA_SHIFT {8U}; // default shift for MLX90641 in open air
constexpr uint8_t MLX90641_ADDRESS {0x33};

class MLX90641 {
private:
  byte refreshRate;        
  float temperatures[768]; //Contains the calculated temperatures of each pixel in the array
  float Tambient;          //Tracks the changing ambient temperature of the sensor
  float Vdd;               //Tracks ... well, Vdd.
  paramsMLX90641 mlx90641;
  bool initialized_{false};
public:
  boolean isConnected();
  boolean isInitialized();
  void measure(bool);
  float getTemperature(int num);
  boolean initialise(int refrate, TwoWire *thisI2c = &Wire);
};
