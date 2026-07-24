#include "i2c.hpp"

//#include "em_bmx280.h"
/*
bool EmBmx280::begin(uint8_t addr, IirFilter filter) {
    // Already initialized with this address?
    if (m_initialized && m_addr == addr) {
        return true;
    }

    m_addr = addr;
    m_initialized = false;

    uint8_t id = 0;
    // Read Hardware Chip ID (Register 0xD0)
    if (!read_register(0xD0, &id, 1)) {
        return false;
    }

    if (id == 0x60) {
        m_isBme280 = true; // Sensor is a BME280 (With Humidity tracking)
    } else if (id == 0x58) {
        m_isBme280 = false; // Sensor is a BMP280 (Temperature & Pressure only)
    } else {
        return false; // Unknown hardware architecture identifier
    }

    // Read standard T & P calibration coefficients (Registers 0x88 to 0xA1 = 24 bytes)
    uint8_t calib[24]; 
    if (!read_register(0x88, calib, 24)) return false;

    dig_T1 = (uint16_t)((calib[1] << 8) | calib[0]);
    dig_T2 = (int16_t)((calib[3] << 8)  | calib[2]);
    dig_T3 = (int16_t)((calib[5] << 8)  | calib[4]);
    dig_P1 = (uint16_t)((calib[7] << 8) | calib[6]);
    dig_P2 = (int16_t)((calib[9] << 8)  | calib[8]);
    dig_P3 = (int16_t)((calib[11] << 8) | calib[10]);
    dig_P4 = (int16_t)((calib[13] << 8) | calib[12]);
    dig_P5 = (int16_t)((calib[15] << 8) | calib[14]);
    dig_P6 = (int16_t)((calib[17] << 8) | calib[16]);
    dig_P7 = (int16_t)((calib[19] << 8) | calib[18]);
    dig_P8 = (int16_t)((calib[21] << 8) | calib[20]);
    dig_P9 = (int16_t)((calib[23] << 8) | calib[22]);

    // Read BME280-specific humidity calibration coefficients
    if (m_isBme280) {
        uint8_t h_calib_1;
        if (!read_register(0xA1, &h_calib_1, 1)) return false;
        dig_H1 = h_calib_1;

        // Reads 7 bytes mapping to registers 0xE1 through 0xE7 sequentially
        uint8_t h_calib_2[7]; 
        if (!read_register(0xE1, h_calib_2, 7)) return false;
        
        dig_H2 = (int16_t)((h_calib_2[1] << 8) | h_calib_2[0]);
        dig_H3 = h_calib_2[2];

        // Correct mapping of bitwise segments according to Bosch packaging rules
        int16_t raw_h4 = ((int16_t)h_calib_2[3] << 4) | (h_calib_2[4] & 0x0F);
        int16_t raw_h5 = ((int16_t)h_calib_2[5] << 4) | (h_calib_2[4] >> 4);

        // Manual 12-bit to 16-bit sign extension to prevent offset truncation
        dig_H4 = (raw_h4 & 0x0800) ? (raw_h4 | 0xF000) : (raw_h4 & 0x0FFF);
        dig_H5 = (raw_h5 & 0x0800) ? (raw_h5 | 0xF000) : (raw_h5 & 0x0FFF);

        dig_H6 = (int8_t)h_calib_2[6];

        // Set BME280 humidity oversampling to x1 (Register 0xF2)
        if (!write_register(0xF2, 0x01)) return false;
    }

    // Configure hardware IIR Filter setting inside config register (0xF5)
    uint8_t config_val = (static_cast<uint8_t>(filter) << 2);
    if (!write_register(0xF5, config_val)) {
        return false;
    }

    // Configure operational mode: Normal Mode, Temp x1, Press x1 (Register 0xF4)
    uint8_t ctrl_meas = (1 << 5) | (1 << 2) | 3; 
    if (!write_register(0xF4, ctrl_meas)) {
        return false;
    }

    m_initialized = true;
    return true;
}

bool EmBmx280::read(float& temperature, float& pressure, float* humidity) {
    size_t read_len = m_isBme280 ? 8 : 6;
    uint8_t data[8]; 
    
    if (!read_register(0xF7, data, read_len)) return false;

    // Process 20-bit raw analog signals
    int32_t adc_P = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | ((int32_t)data[2] >> 4);
    int32_t adc_T = ((int32_t)data[3] << 12) | ((int32_t)data[4] << 4) | ((int32_t)data[5] >> 4);

    // Temperature Compensation Formula
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    int32_t t_fine = var1 + var2;
    temperature = ((t_fine * 5 + 128) >> 8) / 100.0f;

    // Pressure Compensation Formula
    int64_t p_var1 = ((int64_t)t_fine) - 128000;
    int64_t p_var2 = p_var1 * p_var1 * (int64_t)dig_P6;
    p_var2 = p_var2 + ((p_var1 * (int64_t)dig_P5) << 17);
    p_var2 = p_var2 + (((int64_t)dig_P4) << 35);
    p_var1 = ((p_var1 * p_var1 * (int64_t)dig_P3) >> 8) + ((p_var1 * (int64_t)dig_P2) << 12);
    p_var1 = (((((int64_t)1) << 47) + p_var1)) * ((int64_t)dig_P1) >> 33;

    if (p_var1 == 0) {
        pressure = 0.0f; // Avoid division by zero fault conditions
    } else {
        int64_t p = 1048576 - adc_P;
        p = (((p << 31) - p_var2) * 3125) / p_var1;
        p_var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
        p_var2 = (((int64_t)dig_P8) * p) >> 19;
        p = ((p + p_var1 + p_var2) >> 8) + (((int64_t)dig_P7) << 4);
        pressure = (float)p / 25600.0f;
    }

    // Humidity Compensation Formula (BME280 only)
    if (humidity != nullptr) {
        if (m_isBme280) {
            int32_t adc_H = ((int32_t)data[6] << 8) | data[7];
            int32_t v_x1_u32r = (t_fine - ((int32_t)76800));
            
            v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) +
                            ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) *
                            (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                            ((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
            
            v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
            v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
            v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
            *humidity = (float)(v_x1_u32r >> 12) / 1024.0f;
        } else {
            *humidity = 0.0f; // Explicit safety zero out for standard BMP units
        }
    }
    return true;
}
    */