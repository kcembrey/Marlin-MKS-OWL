/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#pragma once

#ifdef HAS_LPC55XX  //caden
//   #include "stm32f1xx_hal.h"
// #elif defined(STM32F4xx)
//   #include "stm32f4xx_hal.h"
// #else
//   #error SPI TFT is currently only supported on STM32F1 and STM32F4 hardware.
#endif

// #ifndef LCD_READ_ID
//   #define LCD_READ_ID 0x04   // Read display identification information (0xD3 on ILI9341)
// #endif
// #ifndef LCD_READ_ID4
//   #define LCD_READ_ID4 0xD3   // Read display identification information (0xD3 on ILI9341)
// #endif

#define DMA_MINC_ENABLE 1   // caden
#define DMA_MINC_DISABLE 0  // caden
#define DATASIZE_8BIT    kSPI_Data8Bits   // caden
#define DATASIZE_16BIT   kSPI_Data16Bits  // caden
#define TFT_IO_DRIVER TFT_SPI
#define DMA_MAX_WORDS     0xFFF

#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3

#define DATA_SIZE_8BIT SSP_DATABIT_8
#define DATA_SIZE_16BIT SSP_DATABIT_16

#define SPI_CLOCK_MAX_TFT  30000000UL
#define SPI_CLOCK_DIV2     8333333 //(SCR:  2)  desired: 8,000,000  actual: 8,333,333  +4.2%  SPI_FULL_SPEED
#define SPI_CLOCK_DIV4     4166667 //(SCR:  5)  desired: 4,000,000  actual: 4,166,667  +4.2%  SPI_HALF_SPEED
#define SPI_CLOCK_DIV8     2083333 //(SCR: 11)  desired: 2,000,000  actual: 2,083,333  +4.2%  SPI_QUARTER_SPEED
#define SPI_CLOCK_DIV16    1000000 //(SCR: 24)  desired: 1,000,000  actual: 1,000,000         SPI_EIGHTH_SPEED
#define SPI_CLOCK_DIV32     500000 //(SCR: 49)  desired:   500,000  actual:   500,000         SPI_SPEED_5
#define SPI_CLOCK_DIV64     250000 //(SCR: 99)  desired:   250,000  actual:   250,000         SPI_SPEED_6
#define SPI_CLOCK_DIV128    125000 //(SCR:199)  desired:   125,000  actual:   125,000         Default from HAL.h

#define SPI_CLOCK_MAX SPI_CLOCK_DIV2

class TFT_SPI {
private:
  static uint32_t readID(uint16_t Reg);
  static void transmit(uint16_t Data);
  static void transmitDMA(uint32_t MemoryIncrease, uint16_t *Data, uint16_t Count);

public:
  static SPIClass SPIx;

  static void init();
  static uint32_t getID();
  static bool isBusy();
  static void abort();

  static void dataTransferBegin(uint16_t DataWidth = DATASIZE_16BIT);
  static void dataTransferEnd() { digitalWrite(TFT_CS_PIN, HIGH);  HS_SPI.end(); };
  static void dataTransferAbort();

  static void writeData(uint16_t Data) { transmit(Data); }
  static void writeReg(uint16_t Reg) { digitalWrite(TFT_DC_PIN, LOW); transmit(Reg); digitalWrite(TFT_DC_PIN, HIGH); }

  static void writeSequence_DMA(uint16_t *data, uint16_t count) { transmitDMA(DMA_MINC_ENABLE, data, count); }
  static void writeMultiple_DMA(uint16_t color, uint16_t count) { static uint16_t data; data = color; transmitDMA(DMA_MINC_DISABLE, &data, count); }

  static void writeSequence(uint16_t *data, uint16_t Count) { transmitDMA(DMA_MINC_ENABLE, data,Count); }
  // static void writeMultiple(uint16_t Color, uint16_t Count) { static uint16_t Data; Data = Color; TransmitDMA(DMA_MINC_DISABLE, &Data, Count); }
  static void writeMultiple(uint16_t Color, uint32_t Count) {
    static uint16_t Data; Data = Color;
    //LPC dma can only write 0xFFF bytes at once.
    #define MAX_DMA_SIZE (0x0400 - 1)
    while (Count > 0) {
      transmitDMA(DMA_MINC_DISABLE, &Data, Count > MAX_DMA_SIZE ? MAX_DMA_SIZE : Count);
      Count = Count > MAX_DMA_SIZE ? Count - MAX_DMA_SIZE : 0;
    }
    // #undef MAX_DMA_SIZE
  }
};
