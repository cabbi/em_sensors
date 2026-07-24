#ifndef __BMX280_SENSOR_H_
#define __BMX280_SENSOR_H_

#include <stdint.h>

#include "em_i2c.h"


// The class for reading values from the BME280 and BMP280 devices
class EmBmx280 {
public:
    static constexpr uint8_t ADDRESS_0 = 0x76;
    static constexpr uint8_t ADDRESS_1 = 0x77;

    // IIR filter coefficients settings (Register 0xF5 config)
    enum class IirFilter : uint8_t {
        OFF      = 0,
        COEFF_2  = 1,
        COEFF_4  = 2,
        COEFF_8  = 3,
        COEFF_16 = 4
    };

    EmBmx280(EmI2c& sharedI2c)
     : m_initialized(false),
       m_i2c(sharedI2c),
       m_addr(0),
       m_isBme280(false) {}

    ~EmBmx280() = default;

    bool isBme280() const { 
        return m_initialized && m_isBme280; 
    }

    bool isBmp280() const { 
        return m_initialized && !m_isBme280; 
    }
    
    bool begin(uint8_t addr, IirFilter filter = IirFilter::OFF) { return false; }

    // Read temperature [deg.C] and pressure [hPa] data
    bool read(float &temperature, float &pressure) {
        return read(temperature, pressure, nullptr);
    }   

    // Read temperature [deg.C], pressure [hPa] and humidity [%] (BME280 ONLY!)
    bool read(float &temperature, float &pressure, float &humidity) {
        return read(temperature, pressure, &humidity);
    }

private:
    bool read(float &temperature, float &pressure, float *humidity) { return false;}

    bool write_register(uint8_t reg, uint8_t value) {
        uint8_t data[2] = { reg, value };
        return false; //m_i2c.write(m_addr, data, 2);
    }

    bool read_register(uint8_t reg, uint8_t* buffer, size_t length) {
        return false; //m_i2c.write_read(m_addr, &reg, 1, buffer, length);
    }

    bool m_initialized;
    EmI2c& m_i2c;
    uint8_t m_addr;
    bool m_isBme280;

    // T & P Calib coefficients
    uint16_t dig_T1; int16_t dig_T2; int16_t dig_T3;
    uint16_t dig_P1; int16_t dig_P2; int16_t dig_P3; int16_t dig_P4;
    int16_t dig_P5;  int16_t dig_P6; int16_t dig_P7; int16_t dig_P8; int16_t dig_P9;
    
    // Humidity Calib coefficients
    uint8_t dig_H1;  int16_t dig_H2; uint8_t dig_H3; int16_t dig_H4; int16_t dig_H5; int8_t dig_H6;
};

#endif //__BMX280_SENSOR_H_
