
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

bool MLX90641_I2CInit(const std::uint8_t slaveAddr, const uint16_t timeout)
{   
    bool success{false};
    Wire.begin();
    uint32_t startTime = millis();
    Serial.printf("Waiting for device to connect at I2C address %u...\n", slaveAddr);
    while (millis() < (startTime+timeout))
    {
        Wire.beginTransmission(slaveAddr);
        if(Wire.endTransmission(true) == 0)
        {
            success = true;
            Serial.printf("Successfully connected to device at I2C address %u.\n", slaveAddr);
            break;
        }
        delay(100);
    }
    if (!success)
    {
        Serial.printf("ERROR: Failed connecting to device at I2C address %u.\n", slaveAddr);
    }
    return success;
}

static int sendI2CCommand(const std::uint8_t slaveAddr, const std::uint8_t *cmd, const std::size_t size, const bool stop = true)
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

int MLX90641_I2CRead(const std::uint8_t slaveAddr, const std::uint16_t startAddress, const std::size_t nMemAddressRead, std::uint16_t *data)
{
    std::uint8_t cmd[READ_PAYLOAD_SIZE] = {static_cast<std::uint8_t>(startAddress >> 8), static_cast<std::uint8_t>(startAddress & 0xFF)};
    Serial.println("20");
    if (sendI2CCommand(slaveAddr, cmd, READ_PAYLOAD_SIZE, false) != 0)
    {
        return -1;
    }
    Serial.println("21");
    std::size_t bytesToRead = 2U * nMemAddressRead;
    Wire.requestFrom(slaveAddr, bytesToRead);
    Serial.printf("Address of Wire object: %d", &Wire);
    Serial.println("22");
    if (static_cast<std::size_t>(Wire.available()) < bytesToRead)
    {
        return -1;
    }
    Serial.println("23");
    Serial.printf("Will run through for loop %d times\n", nMemAddressRead);
    for (std::size_t index = 0U; index < nMemAddressRead; ++index)
    {
        Serial.printf("in for loop iteration %d\n", index); 
        std::uint8_t highByte = Wire.read();
        Serial.println("24");
        std::uint8_t lowByte = Wire.read();
        Serial.println("25");
        data[index] = (highByte << 8) | lowByte;
    }

    return 0;
}

void MLX90641_I2CFreqSet(const std::uint32_t frequency_Hz)
{
    Wire.setClock(frequency_Hz * 1000U/*Hz to kbps baudrate*/); 
}

int MLX90641_I2CWrite(const std::uint8_t slaveAddr, const std::uint16_t writeAddress, const std::uint16_t data)
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
