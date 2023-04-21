/*
* MP8862 - 2.8V-22V VIN, 2A IOUT, 4-Switch, Integrated Buck-Boost Converter with I2C Interface
*
* Author  : Helge B. Wurst
* Created : 2021-07-14
* License : Creative Commons - Attribution - ShareAlike 3.0
*/

#include "MP8862.h"

uint8_t MP8862_init(struct MP8862_t *_MP8862, I2C_HandleTypeDef *_hI2C, uint8_t addr) {
    _MP8862->hI2C = _hI2C;
    _MP8862->deviceAddress = addr;
    uint8_t initialized   = MP8862_isReady(_MP8862);
    _MP8862->VOUT_soft_limit_mV = 9000;
    _MP8862->IOUT_soft_limit_mA = 3000;
    return initialized;
}

uint8_t MP8862_isReady(struct MP8862_t *_MP8862) {
    return HAL_OK == HAL_I2C_IsDeviceReady(_MP8862->hI2C, _MP8862->deviceAddress << 1, 3, 5);
}

uint8_t MP8862_write(struct MP8862_t *_MP8862, enum MP8862_register reg, uint8_t *data, uint8_t len ) {
    // S
    // deviceAddress << 1 | 0 : ACK : REG_ADDR : ACK
    // DATA0 : ACK [ : DATA1* : ACK [ : ... ]]
    // P
    // *: Register addresses are post-incremented, and *** read-only registers will be skipped ***.
    return HAL_OK == HAL_I2C_Mem_Write(_MP8862->hI2C, _MP8862->deviceAddress << 1, reg, 1, data, len, 5);
}

uint8_t MP8862_read(struct MP8862_t *_MP8862, enum MP8862_register reg, uint8_t *data, uint8_t len ) {
    // S
    // deviceAddress << 1 | 0 : ACK : REG_ADDR_K : ACK
    // Sr
    // deviceAddress << 1 | 1 : ACK : BYTE K [: ACK : BYTE K+1* [ : ... ]] : NACK
    // P
    // *: Datasheet p.23 I2C write and read examples do not mention multi-byte read, but experimental evidence shows:
    //    - when master ACK is sent instead of NACK, multiple bytes can be read from auto-incrementing reg addresses
    //    - unmapped register addresses 0x0D ... 0x26 and 0x2A .. 0xFF read as 0x00
    //    -
    return HAL_OK == HAL_I2C_Mem_Read( _MP8862->hI2C, _MP8862->deviceAddress << 1, reg, 1, data, len, 5);
}

uint8_t MP8862_setEnable(struct MP8862_t *_MP8862, uint8_t soft_EN) {
    uint8_t reg;
    uint8_t success = MP8862_read(_MP8862, MP8862_REG_CTL1 , &reg, 1 );
    reg |= MP8862_REG_CTL1 & MP8862_EN;
    return success & MP8862_write(_MP8862, MP8862_REG_CTL1 , &reg, 1 );
}

uint8_t MP8862_setCurrentLimit_mA(struct MP8862_t *_MP8862, uint16_t current_mA) {
    if( current_mA  > _MP8862->IOUT_soft_limit_mA ){
        return 0;
    }
    if(current_mA > 4025){ // highest permitted value producing int part 80
        current_mA = 4025; // 4.0 A max (I_LIM = 0x50)
    }
    uint32_t limit_raw = (current_mA * 1311 + (24 * 1311)) >> 16; // * 0.02 (~ 1311/65536)
    uint8_t tmp = limit_raw & 0x7F;
    return MP8862_write(_MP8862, MP8862_REG_IOUT_LIM , &tmp, 1 );
}

uint8_t MP8862_readCurrentLimit_mA(struct MP8862_t *_MP8862, uint16_t *current_mA) {
    uint8_t success;
    uint8_t tmp;
    success = MP8862_read(_MP8862, MP8862_REG_IOUT_LIM , &tmp, 1 );
    *current_mA = (tmp & 0x7F) * 50; // scale by 50 mA / LSB
    return success;
}

uint8_t MP8862_setVoltageSetpoint_mV(struct MP8862_t *_MP8862, uint16_t voltage_mV) {
    if( voltage_mV  > _MP8862->VOUT_soft_limit_mV ){
        return 0;
    }
    if( voltage_mV > 20480 ){ // highest permitted value producing int part 2047
        voltage_mV = 20480; // 20.47 V max (VOUT = 2047)
    }
    uint8_t tmp[3];
    uint32_t voltage_raw = (voltage_mV * 819 + (5 * 819)) >> 13; // * 0.1 (~ 819/8192)
    tmp[0] = voltage_raw & 0x07;                 // VOUT_L value = voltage_raw[ 2:0]
    tmp[1] = (voltage_raw >> 3) & 0xFF;          // VOUT_H value = voltage_raw[10:3]
    tmp[2] = MP8862_GO_BIT; // Apply VOUT changes by setting the GO bit right after updating VOUT.
    return MP8862_write(_MP8862, MP8862_REG_VOUT_L , tmp, 3 );
}

uint8_t MP8862_readVoltageSetpoint_mV(struct MP8862_t *_MP8862, uint16_t *voltage_mV) {
    uint8_t success;
    uint8_t tmp[2];
    success = MP8862_read(_MP8862, MP8862_REG_VOUT_L , tmp, 2 );
    *voltage_mV = (tmp[1] << 3 | (tmp[0] & 0x07)) * 10; // scale by 10 mV / LSB
    return success;
}

uint8_t MP8862_readPG(struct MP8862_t *_MP8862) {
    uint8_t SR;
    uint8_t success = MP8862_read(_MP8862, MP8862_REG_STATUS, &SR, 1);
    return success && ((SR & MP8862_STATUS_PG) != 0);
}
