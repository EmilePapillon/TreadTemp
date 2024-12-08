
/**
 * MLX90641 Driver adaptation for use with the Arduino framework
 * @author Emile Papillon 
 * Original files from Melexis: https://github.com/melexis/mlx90641-library/
 */

#include <Wire.h>
#include <Arduino.h>
#include "MLX90641_I2C_Driver.hpp"

constexpr std::size_t READ_PAYLOAD_SIZE = 2;
constexpr std::size_t WRITE_PAYLOAD_SIZE = 4;

void MLX90641_I2CInit()
{   
    Wire.begin();
}

static int sendI2CCommand(std::uint8_t slaveAddr, std::uint8_t *cmd, std::size_t size, bool stop = true)
{
    Wire.beginTransmission(slaveAddr);
    Wire.write(cmd, size);
    return Wire.endTransmission(stop);
}

int MLX90641_I2CGeneralReset(void)
{   
    std::uint8_t cmd[READ_PAYLOAD_SIZE] = {0x00/*general call*/, 0x06/*reset*/};

    if (sendI2CCommand(0x00/*broadcast addr*/, cmd, READ_PAYLOAD_SIZE) != 0)
    {
        return -1;
    }
    
    delayMicroseconds(50); // Delay to ensure reset completes
    return 0;
}

int MLX90641_I2CRead(std::uint8_t slaveAddr, std::uint16_t startAddress, std::size_t nMemAddressRead, std::uint16_t *data)
{
    std::uint8_t cmd[READ_PAYLOAD_SIZE] = {static_cast<std::uint8_t>(startAddress >> 8), static_cast<std::uint8_t>(startAddress & 0xFF)};

    if (sendI2CCommand(slaveAddr, cmd, READ_PAYLOAD_SIZE, false) != 0)
    {
        return -1;
    }

    std::size_t bytesToRead = 2 * nMemAddressRead;
    Wire.requestFrom(slaveAddr, bytesToRead);

    if (Wire.available() < bytesToRead)
    {
        return -1;
    }

    for (std::size_t index = 0; index < nMemAddressRead; ++index)
    {
        std::uint8_t highByte = Wire.read();
        std::uint8_t lowByte = Wire.read();
        data[index] = (highByte << 8) | lowByte;
    }

    return 0;
}

void MLX90641_I2CFreqSet(int freq)
{
    Wire.setClock(freq * 1000); // Frequency in Hz
}

int MLX90641_I2CWrite(std::uint8_t slaveAddr, std::uint16_t writeAddress, std::uint16_t data)
{
    std::uint8_t cmd[WRITE_PAYLOAD_SIZE] = {
        static_cast<std::uint8_t>(writeAddress >> 8),
        static_cast<std::uint8_t>(writeAddress & 0xFF),
        static_cast<std::uint8_t>(data >> 8),
        static_cast<std::uint8_t>(data & 0xFF)};

    if (sendI2CCommand(slaveAddr, cmd, WRITE_PAYLOAD_SIZE) != 0)
    {
        return -1;
    }

    std::uint16_t dataCheck;
    int readResult = MLX90641_I2CRead(slaveAddr, writeAddress, 1, &dataCheck);

    if (readResult != 0 || dataCheck != data)
    {
        return -2;
    }

    return 0;
}
