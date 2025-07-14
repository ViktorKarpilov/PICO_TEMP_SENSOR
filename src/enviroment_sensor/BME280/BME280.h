/**
  ******************************************************************************
  * @file    SGP40.h
  * @author  Waveshare Team
  * @version V1.0
  * @date    Dec-2021
  * @brief   
  
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, WAVESHARE SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2021 Waveshare</center></h2>
  ******************************************************************************
  */
#ifndef __BME280_H__
#define __BME280_H__

/***********  BME280_TEST  ****************/

#define BME280_ADDR 0x76

#define ctrl_meas_reg 0x27
#define config_reg 0xA0
#define ctrl_hum_reg 0x01
#include <cstdint>

// inline int32_t digT[3];
// inline int32_t digP[9];
// inline int32_t digH[6];
// inline int32_t t_fine = 0.0;
void BME280_Init();
void BME280_value();

void DEV_I2C_Write(uint8_t addr, uint8_t reg, uint8_t Value);
void DEV_I2C_Write_nByte(uint8_t addr, uint8_t *pData, uint32_t Len);
uint8_t DEV_I2C_ReadByte(uint8_t addr, uint8_t reg);


double compensate_T(int32_t adc_T);
double compensate_H(int32_t adc_H);
double compensate_P(int32_t adc_P);
inline double pres_raw[3];
/***********  END  ****************/

#endif 
