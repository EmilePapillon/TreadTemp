
/**
 * MLX90641 Driver adaptation for use with the Arduino framework
 * @author Emile Papillon 
 * Original files from Melexis: https://github.com/melexis/mlx90641-library/
 */

#pragma once

#include <stdint.h>
#include <cstdint>

constexpr uint16_t DEFAULT_CONNECTION_TIMEOUT{10000U};

/// @brief Initializes the I2C interface and returns whether it can connect to the device at specified address
/// @param slaveAddr The I2C address to attempt connecting
/// @return Whether successful or not (timeout)
bool MLX90641_I2CInit(std::uint8_t slaveAdd, std::uint16_t timeout = DEFAULT_CONNECTION_TIMEOUT);
int MLX90641_I2CGeneralReset(void);
int MLX90641_I2CRead(uint8_t slaveAddr, uint16_t startAddress, std::size_t nMemAddressRead, uint16_t *data);
int MLX90641_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data);
void MLX90641_I2CFreqSet(std::uint32_t frequency_Hz);