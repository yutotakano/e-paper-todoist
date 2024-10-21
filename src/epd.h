/**
  ******************************************************************************
    @file    epd.h
    @author  Waveshare Team
    @version V1.0.0
    @date    23-January-2018
    @brief   This file provides e-Paper driver functions
              void EPD_SendCommand(byte command);
              void EPD_SendData(byte data);
              void EPD_WaitUntilIdle();
              void EPD_Send_1(byte c, byte v1);
              void EPD_Send_2(byte c, byte v1, byte v2);
              void EPD_Send_3(byte c, byte v1, byte v2, byte v3);
              void EPD_Send_4(byte c, byte v1, byte v2, byte v3, byte v4);
              void EPD_Send_5(byte c, byte v1, byte v2, byte v3, byte v4, byte v5);
              void EPD_Reset();
              void EPD_dispInit();

             varualbes:
              EPD_dispLoad;                - pointer on current loading function
              EPD_dispIndex;               - index of current e-Paper
              EPD_dispInfo EPD_dispMass[]; - array of e-Paper properties

  ******************************************************************************
*/

#include <SPI.h>

/* SPI pin definition --------------------------------------------------------*/
// SPI pin definition
#define PIN_SPI_SCK 14
#define PIN_SPI_DIN 13
#define CS_PIN 15
#define RST_PIN 2
#define DC_PIN 4
#define BUSY_PIN 5

/* Pin level definition ------------------------------------------------------*/
#ifndef LOW
#define LOW 0
#endif
#ifndef HIGH
#define HIGH 1
#endif

#define GPIO_PIN_SET 1
#define GPIO_PIN_RESET 0

void GPIO_Mode(unsigned char GPIO_Pin, unsigned char Mode)
{
    if (Mode == 0)
    {
        pinMode(GPIO_Pin, INPUT);
    }
    else
    {
        pinMode(GPIO_Pin, OUTPUT);
    }
}

/* Lut mono ------------------------------------------------------------------*/
byte lut_full_mono[] = {
    0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22,
    0x66, 0x69, 0x69, 0x59, 0x58, 0x99, 0x99, 0x88,
    0x00, 0x00, 0x00, 0x00, 0xF8, 0xB4, 0x13, 0x51,
    0x35, 0x51, 0x51, 0x19, 0x01, 0x00};

byte lut_partial_mono[] = {
    0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x44, 0x12,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* The procedure of sending a byte to e-Paper by SPI -------------------------*/
void EpdSpiTransferCallback(byte data)
{
    // SPI.beginTransaction(spi_settings);
    digitalWrite(CS_PIN, GPIO_PIN_RESET);

    for (int i = 0; i < 8; i++)
    {
        if ((data & 0x80) == 0)
            digitalWrite(PIN_SPI_DIN, GPIO_PIN_RESET);
        else
            digitalWrite(PIN_SPI_DIN, GPIO_PIN_SET);

        data <<= 1;
        digitalWrite(PIN_SPI_SCK, GPIO_PIN_SET);
        digitalWrite(PIN_SPI_SCK, GPIO_PIN_RESET);
    }

    // SPI.transfer(data);
    digitalWrite(CS_PIN, GPIO_PIN_SET);
    // SPI.endTransaction();
}

unsigned char DEV_SPI_ReadByte()
{
    unsigned char j = 0xff;
    GPIO_Mode(PIN_SPI_DIN, 0);
    digitalWrite(CS_PIN, GPIO_PIN_RESET);
    digitalWrite(DC_PIN, GPIO_PIN_SET);
    for (int i = 0; i < 8; i++)
    {
        j = j << 1;
        if (digitalRead(PIN_SPI_DIN))
            j = j | 0x01;
        else
            j = j & 0xfe;

        digitalWrite(PIN_SPI_SCK, GPIO_PIN_SET);
        digitalWrite(PIN_SPI_SCK, GPIO_PIN_RESET);
    }
    digitalWrite(CS_PIN, GPIO_PIN_SET);
    digitalWrite(DC_PIN, GPIO_PIN_RESET);
    GPIO_Mode(PIN_SPI_DIN, 1);
    return j;
}

byte lut_vcom0[] = {15, 0x0E, 0x14, 0x01, 0x0A, 0x06, 0x04, 0x0A, 0x0A, 0x0F, 0x03, 0x03, 0x0C, 0x06, 0x0A, 0x00};
byte lut_w[] = {15, 0x0E, 0x14, 0x01, 0x0A, 0x46, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x86, 0x0A, 0x04};
byte lut_b[] = {15, 0x0E, 0x14, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x4A, 0x04};
byte lut_g1[] = {15, 0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04};
byte lut_g2[] = {15, 0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04};
byte lut_vcom1[] = {15, 0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
byte lut_red0[] = {15, 0x83, 0x5D, 0x01, 0x81, 0x48, 0x23, 0x77, 0x77, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
byte lut_red1[] = {15, 0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* Sending a byte as a command -----------------------------------------------*/
void EPD_SendCommand(byte command)
{
    digitalWrite(DC_PIN, LOW);
    EpdSpiTransferCallback(command);
}

/* Sending a byte as a data --------------------------------------------------*/
void EPD_SendData(byte data)
{
    digitalWrite(DC_PIN, HIGH);
    EpdSpiTransferCallback(data);
}

/* Waiting the e-Paper is ready for further instructions ---------------------*/
void EPD_WaitUntilIdle()
{
    // 0: busy, 1: idle
    while (digitalRead(BUSY_PIN) == 0)
        delay(100);
}

/* Waiting the e-Paper is ready for further instructions ---------------------*/
void EPD_WaitUntilIdle_high()
{
    // 1: busy, 0: idle
    while (digitalRead(BUSY_PIN) == 1)
        delay(100);
}

/* Send a one-argument command -----------------------------------------------*/
void EPD_Send_1(byte c, byte v1)
{
    EPD_SendCommand(c);
    EPD_SendData(v1);
}

/* Send a two-arguments command ----------------------------------------------*/
void EPD_Send_2(byte c, byte v1, byte v2)
{
    EPD_SendCommand(c);
    EPD_SendData(v1);
    EPD_SendData(v2);
}

/* Send a three-arguments command --------------------------------------------*/
void EPD_Send_3(byte c, byte v1, byte v2, byte v3)
{
    EPD_SendCommand(c);
    EPD_SendData(v1);
    EPD_SendData(v2);
    EPD_SendData(v3);
}

/* Send a four-arguments command ---------------------------------------------*/
void EPD_Send_4(byte c, byte v1, byte v2, byte v3, byte v4)
{
    EPD_SendCommand(c);
    EPD_SendData(v1);
    EPD_SendData(v2);
    EPD_SendData(v3);
    EPD_SendData(v4);
}

/* Send a five-arguments command ---------------------------------------------*/
void EPD_Send_5(byte c, byte v1, byte v2, byte v3, byte v4, byte v5)
{
    EPD_SendCommand(c);
    EPD_SendData(v1);
    EPD_SendData(v2);
    EPD_SendData(v3);
    EPD_SendData(v4);
    EPD_SendData(v5);
}

/* Writting lut-data into the e-Paper ----------------------------------------*/
void EPD_lut(byte c, byte l, byte *p)
{
    // lut-data writting initialization
    EPD_SendCommand(c);

    // lut-data writting doing
    for (int i = 0; i < l; i++, p++)
        EPD_SendData(*p);
}

/* Writting lut-data of the black-white channel ------------------------------*/
void EPD_SetLutBw(byte *c20, byte *c21, byte *c22, byte *c23, byte *c24)
{
    EPD_lut(0x20, *c20, c20 + 1); // g vcom
    EPD_lut(0x21, *c21, c21 + 1); // g ww --
    EPD_lut(0x22, *c22, c22 + 1); // g bw r
    EPD_lut(0x23, *c23, c23 + 1); // g wb w
    EPD_lut(0x24, *c24, c24 + 1); // g bb b
}

/* Writting lut-data of the red channel --------------------------------------*/
void EPD_SetLutRed(byte *c25, byte *c26, byte *c27)
{
    EPD_lut(0x25, *c25, c25 + 1);
    EPD_lut(0x26, *c26, c26 + 1);
    EPD_lut(0x27, *c27, c27 + 1);
}

/* This function is used to 'wake up" the e-Paper from the deep sleep mode ---*/
void EPD_Reset()
{
    digitalWrite(RST_PIN, HIGH);
    delay(50);
    digitalWrite(RST_PIN, LOW);
    delay(5);
    digitalWrite(RST_PIN, HIGH);
    delay(50);
}

/* e-Paper initialization functions ------------------------------------------*/

int EPD_dispIndex;        // The index of the e-Paper's type
int EPD_dispX, EPD_dispY; // Current pixel's coordinates (for 2.13 only)
void (*EPD_dispLoad)();   // Pointer on a image data writting function

/* Show image and turn to deep sleep mode (a-type, 4.2 and 2.7 e-Paper) ------*/
void EPD_showA()
{
    Serial.print("\r\n EPD_showA");
    // Refresh
    EPD_Send_1(0x22, 0xC4); // DISPLAY_UPDATE_CONTROL_2
    EPD_SendCommand(0x20);  // MASTER_ACTIVATION
    EPD_SendCommand(0xFF);  // TERMINATE_FRAME_READ_WRITE
    EPD_WaitUntilIdle();

    // Sleep
    EPD_SendCommand(0x10); // DEEP_SLEEP_MODE
    EPD_WaitUntilIdle();
}
#include "epd4in2.h"
