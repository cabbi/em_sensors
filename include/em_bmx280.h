#ifndef __BMX280_SENSOR_H_
#define __BMX280_SENSOR_H_

#include <cstring>
#include <bmp280.h>

#include "em_i2c.h"

// NOTE:
// In order to use this class, please add 'esp-idf-lib/bmp280: "*"' in your 'idf_component.yml' file 

// The class for reading values from the BME280 and BMP280 devices
class EmBmx280 {
public:
    EmBmx280()
     : m_initialized(false) {
        memset(&m_dev, 0, sizeof(bmp280_t));
    }

    ~EmBmx280() = default;

    bool begin(EmI2c& i2c, uint8_t addr = BMP280_I2C_ADDRESS_0) {
        if (!i2c.isInitialized()) {
            return false;
        }

        bmp280_params_t params;
        if (bmp280_init_default_params(&params) != ESP_OK) {
            return false;
        }
        params.mode = BMP280_MODE_NORMAL;
        params.filter = BMP280_FILTER_4;
        params.oversampling_temperature = BMP280_STANDARD;
        params.oversampling_pressure = BMP280_STANDARD;        
        params.oversampling_humidity = BMP280_STANDARD; 

        esp_err_t err = bmp280_init_desc(&m_dev, addr, i2c.getPort(), GPIO_NUM_NC, GPIO_NUM_NC);
        if (err != ESP_OK) {
            return false;
        }

        err = bmp280_init(&m_dev, &params);
        if (err != ESP_OK) {
            return false;
        }

        m_initialized = true;
        return true;
    }

    // Read temperature [deg.C] and pressure [Pascal] data
    bool read(float &temperature, float &pressure) {
        return read(temperature, pressure, nullptr);
    }   

    // Read temperature [deg.C], pressure [Pascal] and humidity [%] (BME280 ONLY!) data
    bool read(float &temperature, float &pressure, float &humidity) {
        return read(temperature, pressure, &humidity);
    }

     bool read(float &temperature, float &pressure, float *humidity) {
        if (!m_initialized) {
             return false;
        }
        esp_err_t err = bmp280_read_float(&m_dev, &temperature, &pressure, humidity);
        if (err != ESP_OK) {
            return false;
        }
        return true;
    }    

private:
    bmp280_t m_dev;
    bool m_initialized;
};


#endif //__BMX280_SENSOR_H_
