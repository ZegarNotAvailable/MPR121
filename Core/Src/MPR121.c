/*
 * MPR121.c
 *
 *  Created on: Jul 24, 2022
 *      Author: Zegar
 */
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "mpr121.h"

// ------------------------------------------------------------------------------
//  Keyboard constants
// ------------------------------------------------------------------------------

//const uint8_t PCF_kbd = 0x38;     //Address PCF8574A 0x38 , for PCF8574 0x20
const uint8_t MPR121 = 0x5A;      //Address MPR121

// ------------------------------------------------------------------------------
// Kody klawiszy tworzymy wg. wzoru: starsza cyfra nr kolumny, mlodsza ma zero na pozycji
// numeru wiersza
//  wiersz \ kolumna  5 4 3 2 1 0
// -------------------------------
//  3 (0111)          Z C D E F M
//  2 (1011)          X 8 9 A B G
//  1 (1101)          Y 4 5 6 7 .
//  0 (1110)          W 0 1 2 3 =
// ------------------------------------------------------------------------------
char keys[4][7]=
    {
	"ZCDEFM",
	"Y89ABG",
	"X4567.",
	"W0123="
    };

// ------------------------------------------------------------------------------
//  Keyboard functions
// ------------------------------------------------------------------------------

void readKeyboard()
{
  uint8_t touchNumber;
  //while (checkInterrupt());
  uint16_t touchstatus;
  touchNumber = 0;
  touchstatus = Read16(MPR121);//, Register);//((MSB << 8) | LSB); //16bits that make up the touch states
  uint8_t row = 0;
  uint8_t rowTemp = 0;
  uint8_t column = 0;
  for (uint8_t j = 0; j < 4; j++) // Check how many electrodes were pressed
  {
    if ((touchstatus & (1 << j)))
    {
      touchNumber++;
      row |= 1 << (3 - j);  // Reverse order of electrodes ...
      rowTemp = (j);
    }
  }
  if (touchNumber == 1)
  {
    //Serial.print(row);
    touchNumber = 0;
    for (uint8_t j = 4; j < 10; j++) // Check how many electrodes were pressed
    {
      if ((touchstatus & (1 << j)))
        touchNumber++;
    }
    if (touchNumber == 1)
    {
      //uint8_t col = touchstatus & 0x3F0;
      for (uint8_t j = 4; j < 10; j++) // Check which electrode were pressed
      {
        if ((touchstatus & (1 << j)))
          column = 5 - (j - 4);   // Reverse order of electrodes ...
          //column =(j - 4);
      }
      //uint8_t keyCode = (row ^ 0xF) | ( column << 4 ); // Calculate the key code
      uint8_t keyCode = keys[rowTemp][column];
      sendKey(keyCode);
    }
    else
    {
      uint8_t keyCode = 0xFF;  //noKey()
      sendKey(keyCode);
    }
  }
  else
  {
    uint8_t keyCode = 0xFF;
    sendKey(keyCode);     //noKey()
  }
}


void mpr121_setup(void)
{
  set_register(MPR121, ELE_CFG, 0x00);

  // Section A - Controls filtering when data is > baseline.
  set_register(MPR121, MHD_R, 0x01);
  set_register(MPR121, NHD_R, 0x01);
  set_register(MPR121, NCL_R, 0x00);
  set_register(MPR121, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(MPR121, MHD_F, 0x01);
  set_register(MPR121, NHD_F, 0x01);
  set_register(MPR121, NCL_F, 0xFF);
  set_register(MPR121, FDL_F, 0x02);

  // Section C - Sets touch and release thresholds for each electrode
  set_register(MPR121, ELE0_T, TOU_THRESH);
  set_register(MPR121, ELE0_R, REL_THRESH);

  set_register(MPR121, ELE1_T, TOU_THRESH);
  set_register(MPR121, ELE1_R, REL_THRESH);

  set_register(MPR121, ELE2_T, TOU_THRESH);
  set_register(MPR121, ELE2_R, REL_THRESH);

  set_register(MPR121, ELE3_T, TOU_THRESH);
  set_register(MPR121, ELE3_R, REL_THRESH);

  set_register(MPR121, ELE4_T, TOU_THRESH);
  set_register(MPR121, ELE4_R, REL_THRESH);

  set_register(MPR121, ELE5_T, TOU_THRESH);
  set_register(MPR121, ELE5_R, REL_THRESH);

  set_register(MPR121, ELE6_T, TOU_THRESH);
  set_register(MPR121, ELE6_R, REL_THRESH);

  set_register(MPR121, ELE7_T, TOU_THRESH);
  set_register(MPR121, ELE7_R, REL_THRESH);

  set_register(MPR121, ELE8_T, TOU_THRESH);
  set_register(MPR121, ELE8_R, REL_THRESH);

  set_register(MPR121, ELE9_T, TOU_THRESH);
  set_register(MPR121, ELE9_R, REL_THRESH);

  set_register(MPR121, ELE10_T, TOU_THRESH);
  set_register(MPR121, ELE10_R, REL_THRESH);

  set_register(MPR121, ELE11_T, TOU_THRESH);
  set_register(MPR121, ELE11_R, REL_THRESH);

  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(MPR121, FIL_CFG, 0x04);

  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(MPR121, ELE_CFG, 0x0A);  // Enables 10 Electrodes


  // Section F
  // Enable Auto Config and auto Reconfig
  /*set_register(MPR121, ATO_CFG0, 0x0B);
    set_register(MPR121, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(MPR121, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
    set_register(MPR121, ATO_CFGT, 0xB5);*/  // Target = 0.9*USL = 0xB5 @3.3V

  set_register(MPR121, ELE_CFG, 0x0C);

}

void set_register(uint8_t Address, uint8_t Register, uint8_t Value)
{
  HAL_I2C_Mem_Write( &hi2c1, Address<<1, Register, 1, &Value, 1, 1000);
}

void sendKey (uint8_t k)
{
  //HAL_I2C_Master_Transmit(&hi2c1, PCF_kbd, &k, 1, 1000);
  if (k == 0xff)
    {
      k = '-';
    }
  HAL_UART_Transmit(&huart2, &k , 1, 1000);
}

//
// Write 8 bits to BMP280 to Register
//
void Write8(uint8_t Address , uint8_t Register, uint8_t Value)
{
	HAL_I2C_Mem_Write( &hi2c1, Address<<1, Register, 1, &Value, 1, 1000);
}

//
// Read 16 bits from MPR121
//
uint16_t Read16(uint8_t Address)
{
	uint8_t Value[2];

	HAL_I2C_Master_Receive(&hi2c1, (Address<<1), Value, 2, 1000);

	return ((Value[1] << 8) | Value[0]);
}

