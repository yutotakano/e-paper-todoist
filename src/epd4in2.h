/**
 ******************************************************************************
 * @file    edp4in2.h
 * @author  Waveshare Team
 * @version V1.0.0
 * @date    23-January-2018
 * @brief   This file describes initialisation of 4.2 and 4.2b e-Papers
 *
 ******************************************************************************
 */

int EPD_Init_4in2_V2()
{
    EPD_Reset();
    EPD_WaitUntilIdle_high();

    EPD_SendCommand(0x12);
    EPD_WaitUntilIdle_high();

    EPD_Send_2(0x21, 0x80, 0x00);
    EPD_Send_1(0x3C, 0x05);
    EPD_Send_1(0x11, 0x03);

    EPD_Send_2(0x44, 0x00, 0x31);
    EPD_Send_4(0x45, 0x00, 0x00, 0x2B, 0x01);

    EPD_Send_1(0x4E, 0x00);
    EPD_Send_2(0x4F, 0x00, 0x00);

    // EPD_SendCommand(0x24); // DATA_START_TRANSMISSION_1
    // for (int i = 0; i < 15000; i++)
    //     EPD_SendData(0xFF); // Red channel

    // EPD_SendCommand(0x22);
    // EPD_SendData(0xF7);
    // EPD_SendCommand(0x20);
    // EPD_WaitUntilIdle_high();

    // EPD_SendCommand(0x24); // DATA_START_TRANSMISSION_1
    return 0;
}

void EPD_4IN2_V2_Show(void)
{
    EPD_SendCommand(0x22);
    EPD_SendData(0xF7);
    EPD_SendCommand(0x20);
    EPD_WaitUntilIdle_high();

    EPD_SendCommand(0x10); // DEEP_SLEEP
    EPD_SendData(0x01);
}

int EPD_Init_4in2b_V2()
{

    unsigned char i;
    EPD_Reset();

    EPD_SendCommand(0x2F);
    delay(2);

    digitalWrite(CS_PIN, GPIO_PIN_SET);
    delay(2);
    i = DEV_SPI_ReadByte();
    Serial.printf("i=%d", i);
    delay(2);

    if (i == 0x01)
    {
        flag = 0;
        EPD_Reset();
        EPD_WaitUntilIdle_high();
        EPD_SendCommand(0x12);
        EPD_WaitUntilIdle_high();

        EPD_Send_1(0x3C, 0x05);
        EPD_Send_1(0x18, 0x80);
        EPD_Send_1(0x11, 0x03);

        EPD_Send_2(0x44, 0x00, 0x31);
        EPD_Send_4(0x45, 0x00, 0x00, 0x2B, 0x01);

        EPD_Send_1(0x4E, 0x00);
        EPD_Send_2(0x4F, 0x00, 0x00);
        EPD_WaitUntilIdle_high();

        EPD_SendCommand(0x24);
        delay(2);
    }
    else
    {
        flag = 1;
        EPD_Reset();
        EPD_WaitUntilIdle();

        EPD_SendCommand(0x04);
        EPD_WaitUntilIdle();
        EPD_Send_1(0x00, 0x0F); // PANEL_SETTING

        EPD_SendCommand(0x10); // DATA_START_TRANSMISSION_1
        delay(2);
    }
    return 0;
}

void EPD_4IN2B_V2_load(void)
{
    if (flag == 0)
    {
        EPD_loadAFilp();
    }
    else
    {
        EPD_loadA();
    }
}

void EPD_4IN2B_V2_Show(void)
{
    if (flag == 0)
    {
        EPD_SendCommand(0x22);
        EPD_SendData(0xF7);
        EPD_SendCommand(0x20);
        EPD_WaitUntilIdle_high();
        EPD_Send_1(0X10, 0x03);
    }
    else
    {
        EPD_SendCommand(0x12); // DISPLAY_REFRESH
        delay(100);
        EPD_WaitUntilIdle();

        EPD_Send_1(0X50, 0xf7);
        EPD_SendCommand(0X02);  // power off
        EPD_WaitUntilIdle();    // waiting for the electronic paper IC to release the idle signal
        EPD_Send_1(0X07, 0xf7); // deep sleep
    }
}
