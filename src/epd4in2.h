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

    EPD_SendCommand(0x12); // SW RESET (resets commands and params to default after a wake)
    EPD_WaitUntilIdle_high();

    EPD_Send_2(0x21, 0x00, 0x00); // DISPLAY UPDATE CONTROL (don't invert the reds nor blacks)
    EPD_Send_1(0x3C, 0x05);       // BORDER WAVEFORM CONTROL (i don't completely understand this yet)
    EPD_Send_1(0x11, 0x03);       // DATA ENTRY MODE (automatically increment both X and Y)

    EPD_Send_2(0x44, 0x00, (byte)(400 / 8 - 1)); // SET X POSITION (start = 0, end = 49 for some reason)
    EPD_Send_4(0x45, 0x00, 0x00, 0x2B, 0x01);    // SET Y POSITION (start = 0, end = 299)

    EPD_Send_1(0x4E, 0x00);       // INITIAL X COUNTER (set to 0)
    EPD_Send_2(0x4F, 0x00, 0x00); // INITIAL Y COUNTER (set to 0)
    EPD_WaitUntilIdle_high();

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
