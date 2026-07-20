#ifndef __BMX280_SENSOR_H_
#define __BMX280_SENSOR_H_

#include <cstring>
#include <iostream>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <i2c.hpp>
#include <bmp280.h>


// The class for reading values from the BME280 and BMP280 devices
class EmBmx280 {
public:
    EmBmx280(espp::I2c& shared_i2c, uint8_t dev_addr = BME280_I2C_ADDR_PRIM)
        : i2c_bus_(shared_i2c), dev_address_(dev_addr) {
        
        bme280_device_.intf = BME280_I2C_INTF;
        bme280_device_.intf_ptr = this; // Passiamo 'this' come puntatore di contesto
        bme280_device_.read = bme280_i2c_read_adapter;
        bme280_device_.write = bme280_i2c_write_adapter;
        bme280_device_.delay_us = bme280_delay_us_adapter;

        int8_t rslt = bme280_init(&bme280_device_);
        if (rslt != BME280_OK) {
            std::cerr << "Errore inizializzazione BME280: " << (int)rslt << std::endl;
            return;
        }

        bme280_device_.settings.osr_h = BME280_OVERSAMPLING_1X;
        bme280_device_.settings.osr_p = BME280_OVERSAMPLING_1X;
        bme280_device_.settings.osr_t = BME280_OVERSAMPLING_1X;
        bme280_device_.settings.filter = BME280_FILTER_COEFF_OFF;

        uint8_t settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;
        bme280_set_sensor_settings(settings_sel, &bme280_device_);
        
        // Imposta il sensore in modalità Normal (campionamento continuo)
        bme280_set_sensor_mode(BME280_NORMAL_MODE, &bme280_device_);
    }

    struct EnvironmentalData {
        float temperature; // °C
        float pressure;    // hPa (Hectopascal / millibar)
        float humidity;    // % Umidità Relativa
    };

    EnvironmentalData readData() {
        struct bme280_data comp_data;
        int8_t rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &bme280_device_);
        
        if (rslt != BME280_OK) {
            std::cerr << "Errore lettura dati BME280" << std::endl;
            return {0.0f, 0.0f, 0.0f};
        }

        // Il driver ufficiale restituisce i dati convertiti in float o interi stabili
        return {
            static_cast<float>(comp_data.temperature),
            static_cast<float>(comp_data.pressure) / 100.0f, // Converte Pa in hPa
            static_cast<float>(comp_data.humidity)
        };
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
    espp::I2c& i2c_bus_;
    uint8_t dev_address_;
    struct bme280_dev bme280_device_;

    static int8_t bme280_i2c_read_adapter(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
        auto* instance = static_cast<Bme280Reader*>(intf_ptr);
        
        bool success = instance->i2c_bus_.write_read(instance->dev_address_, &reg_addr, 1, reg_data, len);
        return success ? BME280_OK : BME280_E_COMM_FAIL;
    }

    static int8_t bme280_i2c_write_adapter(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
        auto* instance = static_cast<Bme280Reader*>(intf_ptr);
        
        uint8_t* write_buf = new uint8_t[len + 1];
        write_buf[0] = reg_addr;
        std::copy(reg_data, reg_data + len, write_buf + 1);

        bool success = instance->i2c_bus_.write(instance->dev_address_, write_buf, len + 1);
        delete[] write_buf;

        return success ? BME280_OK : BME280_E_COMM_FAIL;
    }

    static void bme280_delay_us_adapter(uint32_t period, void *intf_ptr) {
        uint32_t ms = period / 1000;
        if (ms == 0) ms = 1;
        vTaskDelay(pdMS_TO_TICKS(ms));
    }
};

#ifdef _____

// The class for reading values from the BME280 and BMP280 devices
class EmBmx280 {
public:
    EmBmx280()
     : m_initialized(false) {
        memset(&m_dev, 0, sizeof(bmp280_t));
    }

    ~EmBmx280() = default;

    bool begin(EmI2C& i2c, uint8_t addr = BMP280_I2C_ADDRESS_0) {
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

#endif

#endif //__BMX280_SENSOR_H_
