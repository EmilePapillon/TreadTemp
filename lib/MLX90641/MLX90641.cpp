#include <Wire.h>
#include <MLX90641.hpp>
#include <MLX90641_I2C_Driver.hpp>

static constexpr uint32_t CLOCK_FREQUENCY_Hz{1000};

boolean MLX90641::initialise(int refrate, TwoWire *thisI2c) {
  bool success = MLX90641_I2CInit(MLX90641_ADDRESS);
  if (!success)
  {
    return false;
  }
  MLX90641_I2CFreqSet(CLOCK_FREQUENCY_Hz); //Changing gears, ensure that I2C clock speed set to 1MHz
  uint16_t eeMLX90641[832];
  if (MLX90641_DumpEE(MLX90641_ADDRESS, eeMLX90641) != 0) {
    Serial.println("ERROR: Failed to load MLX90641 system parameters.");
    return false;
  }

  if (MLX90641_ExtractParameters(eeMLX90641, &mlx90641) != 0) {
    Serial.printf("ERROR: MLX90641 Parameter extraction failed.\n");
    return false;
  }

  byte Hz;
  switch (refrate) {
    case 0:  Hz = 0x00; break;
    case 1:  Hz = 0x01; break;
    case 2:  Hz = 0x02; break;
    case 4:  Hz = 0x03; break;
    case 8:  Hz = 0x04; break;
    case 16: Hz = 0x05; break;
    case 32: Hz = 0x06; break;
    case 64: Hz = 0x07; break;
    default: Hz = 0b00;
  }

  if (MLX90641_SetRefreshRate(MLX90641_ADDRESS, Hz) != 0) {
    Serial.println("ERROR: Setting MLX90641 refresh rate failed.");
    return false;
  }

  initialized_ = true;
  Serial.printf("MLX90641 initialised correctly at I2C address %u...\n", MLX90641_ADDRESS);
  return true;
}

void MLX90641::measure(bool) {
  uint16_t mlx90641Frame[834];
  MLX90641_I2CFreqSet(CLOCK_FREQUENCY_Hz); //Changing gears, ensure that I2C clock speed set to 1MHz
  int _stat = MLX90641_GetFrameData((uint8_t)MLX90641_ADDRESS, mlx90641Frame);
  if (_stat < 0) Serial.printf("GetFrame Error: %d\n", _stat);
  //Vdd = MLX90641_GetVdd(mlx90641Frame, &mlx90641);
  //int subpage = MLX90641_GetSubPageNumber(mlx90641Frame);
  Tambient = MLX90641_GetTa(mlx90641Frame, &mlx90641);
  float tr = Tambient - TA_SHIFT; //Reflected temperature based on the sensor ambient temperature
  float emissivity = 1;
  MLX90641_CalculateTo(mlx90641Frame, &mlx90641, emissivity, tr, temperatures);
}

float MLX90641::getTemperature(int num) {
  if ((num >= 0) && (num < 768)) {
    return temperatures[num];
  } else {
    return 0;
  }
}
