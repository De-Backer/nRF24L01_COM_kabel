/* RF24L01.c
 *
 * Created: 20160418
 * Author:  De Backer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef _RF24L01_c__
#define _RF24L01_c__
#ifdef __cplusplus
extern "C"{
#endif

#include <RF24L01.h>

void start_up_RF24L01()
{
#ifdef nRF_VDD
    /* µc togel pin on */
#ifdef debug_RF24L01
    transmit_string_USART("\n nRF_VDD pin on");
#endif
    nRF_VDD_PORT |=(1<<nRF_VDD);
#endif
    /* on start nRF24L01 is Undefined */

#ifdef debug_RF24L01
    transmit_string_USART("\n Wait for start up");
#endif
    _delay_ms(100);/* 100ms */

    /* Power Down */
}

void shutdown_RF24L01()
{
#ifdef nRF_VDD
    /* µc togel pin off */
#ifdef debug_RF24L01
    transmit_string_USART("\n nRF_VDD pin off");
#endif
    nRF_VDD_PORT &=~(1<<nRF_VDD);
#endif

    /* nRF24L01 is Undefined */
#ifdef debug_RF24L01
    transmit_string_USART("\n nRF24L01 is Undefined");
#endif
}

void set_RF_channel_frequency(uint8_t frequency)
{
    /* resolution = 1MHz */

    /* Set CSN Low */
    nRF_CEN_PORT &=~(1 <<nRF_CSN);

    asm ("nop");

    send_spi(W_REGISTER+RF_CH);

    /* set_RF_channel */
    send_spi(frequency&0x7f);

    /* Set CSN High */
    nRF_CEN_PORT|=(1<<nRF_CSN);
}

void full_reset_RF24L01()
{
#ifdef debug_RF24L01
    transmit_string_USART("\n full_reset_RF24L01");
#endif
    if(nRF_CE_PORT&(1<<nRF_CE))
    {
        /* Set CE Low */
        nRF_CE_PORT &=~(1<<nRF_CE);
        /* Standby-1 or Power Down */
    }
    /* Set CSN Low */
    nRF_CEN_PORT &=~(1 <<nRF_CSN);

    asm ("nop");

    /* Configuration Register */
    send_spi(W_REGISTER+NRF_CONFIG);
    send_spi(0x08);
    /* Enable ‘Auto Acknowledgment’ */
    send_spi(W_REGISTER+EN_AA);
    send_spi(0x3f);
    /* Enabled RX Addresses */
    send_spi(W_REGISTER+EN_RXADDR);
    send_spi(0x03);
    /* Address Widths */
    send_spi(W_REGISTER+SETUP_AW);
    send_spi(0x03);
    /* Setup of Automatic Retransmission */
    send_spi(W_REGISTER+SETUP_RETR);
    send_spi(0x03);
    /* RF Channel */
    send_spi(W_REGISTER+RF_CH);
    send_spi(0x02);
    /* RF Setup Register */
    send_spi(W_REGISTER+RF_SETUP);
    send_spi(0x0f);
    /* Status Register */
    send_spi(W_REGISTER+NRF_STATUS);
    send_spi(0x7e);
    /* Receive address data pipe 0 */
    send_spi(W_REGISTER+RX_ADDR_P0);
    send_spi(0xe7);
    send_spi(0xe7);
    send_spi(0xe7);
    send_spi(0xe7);
    send_spi(0xe7);
    /* Receive address data pipe 1 */
    send_spi(W_REGISTER+RX_ADDR_P1);
    send_spi(0xc2);
    send_spi(0xc2);
    send_spi(0xc2);
    send_spi(0xc2);
    send_spi(0xc2);
    /* Receive address data pipe 2 */
    send_spi(W_REGISTER+RX_ADDR_P2);
    send_spi(0xc3);
    /* Receive address data pipe 3 */
    send_spi(W_REGISTER+RX_ADDR_P3);
    send_spi(0xc4);
    /* Receive address data pipe 4 */
    send_spi(W_REGISTER+RX_ADDR_P4);
    send_spi(0xc5);
    /* Receive address data pipe 5 */
    send_spi(W_REGISTER+RX_ADDR_P5);
    send_spi(0xc6);
    /* Transmit address */
    send_spi(W_REGISTER+TX_ADDR);
    send_spi(0xe7);
    send_spi(0xe7);
    send_spi(0xe7);
    send_spi(0xe7);
    send_spi(0xe7);
    /* Number of bytes in RX payload in data pipe 0 */
    send_spi(W_REGISTER+RX_PW_P0);
    send_spi(0x00);
    /* Number of bytes in RX payload in data pipe 1 */
    send_spi(W_REGISTER+RX_PW_P1);
    send_spi(0x00);
    /* Number of bytes in RX payload in data pipe 2 */
    send_spi(W_REGISTER+RX_PW_P2);
    send_spi(0x00);
    /* Number of bytes in RX payload in data pipe 3 */
    send_spi(W_REGISTER+RX_PW_P3);
    send_spi(0x00);
    /* Number of bytes in RX payload in data pipe 4 */
    send_spi(W_REGISTER+RX_PW_P4);
    send_spi(0x00);
    /* Number of bytes in RX payload in data pipe 5 */
    send_spi(W_REGISTER+RX_PW_P5);
    send_spi(0x00);
    /* Enable dynamic payload length */
    send_spi(W_REGISTER+DYNPD);
    send_spi(0x00);
    /* Feature Register */
    send_spi(W_REGISTER+FEATURE);
    send_spi(0x00);

    /* Set CSN High */
    nRF_CEN_PORT|=(1<<nRF_CSN);
}

void nRF_IRQ_pin_triger()
{
    nRF_CE_PORT &=~(1 <<nRF_CE);/* reset pin (stop met zenden/ontvagen) */

    uint8_t info = GetReg(NRF_STATUS);/* waarom un interupt? */
    if(info&(1<<RX_DR))/* RX Data Ready */
    {
        /* lees data en clear bit RX_DR in NRF_STATUS */
        data[0]=32;
        if(GetReg(FEATURE)&0x04)/* is enabled Dynamic Payload Length on? */
        {
            data=WriteToNrf(R,R_RX_PL_WID,data,1);
        }
        if(data[0]>32)/* flush RX FIFO */
        {
            WriteToNrf(W,FLUSH_RX,data,1);
        } else {
            /* data naar USART sturen */
            /* maak un funxie met data buffer TBA */

            /* Set CSN Low */
            nRF_CEN_PORT &=~(1<<nRF_CSN);
            asm ("nop");
            send_spi(R_RX_PAYLOAD);
            uint8_t var;
            for (var = 0; var < data[0]; ++var) {
                transmit_USART(send_spi(NOP));
            }
            /* Set CSN High */
            nRF_CEN_PORT|=(1<<nRF_CSN);

        }
        data[0]=(1<<RX_DR);/* clear bit RX_DR in NRF_STATUS */
        WriteToNrf(W,NRF_STATUS,data,1);
    }
    if(info&(1<<TX_DS))/* Data sent */
    {
        /* clear bit TX_DS in NRF_STATUS en reset pin nRF_CE */
        data[0]=(1<<TX_DS);/* clear bit TX_DS in NRF_STATUS */
        WriteToNrf(W,NRF_STATUS,data,1);
    }
    if(info&(1<<MAX_RT))/* Comm fail */
    {
        /* clear bit MAX_RT in NRF_STATUS */
        data[0]=(1<<MAX_RT);/* clear bit MAX_RT in NRF_STATUS */
        WriteToNrf(W,NRF_STATUS,data,1);
    }
}

uint8_t Power_Down()
{
    /* Set CSN Low */
    nRF_CEN_PORT &=~(1<<nRF_CSN);

    uint8_t reg=0;

    send_spi(R_REGISTER + NRF_CONFIG);

    reg = send_spi(NOP);

    reg &= ~(1<<PWR_UP);

    send_spi(W_REGISTER + NRF_CONFIG);

    reg=send_spi(reg);

    /* Set CSN High */
    nRF_CEN_PORT|=(1<<nRF_CSN);
    /* Return NRF_CONFIG */
    return reg;
}

uint8_t GetReg(uint8_t reg)
{
    /* Set CSN Low */
    nRF_CEN_PORT &=~(1<<nRF_CSN);
    asm ("nop");
    send_spi(R_REGISTER + reg);	//R_Register --> Set to Reading Mode, "reg" --> The registry which will be read
    reg = send_spi(NOP);		//Send DUMMY BYTE[NOP] to receive first byte in 'reg' register
    /* Set CSN High */
    nRF_CEN_PORT|=(1<<nRF_CSN);
    return reg;							//Return the registry read
}

uint8_t *WriteToNrf(uint8_t ReadWrite, uint8_t reg, uint8_t *val, uint8_t antVal)
{
    //ReadWrite --> "R" or "W", reg --> 'register', *val --> array with package, antVal --> number of int in array
    if(ReadWrite == W)//If it is in READMODE, then addr is already 0x00
    {
        reg = W_REGISTER + reg;
    }
    static uint8_t ret[32];	//Array to be returned in the end

    /* Set CSN Low */
    nRF_CEN_PORT &=~(1 <<nRF_CSN);
    asm ("nop");
    send_spi(reg);				//"reg" --> Set nRf to write or read mode

    uint8_t i=0;
    for(; i<antVal; i++)
    {
        if(ReadWrite == R && reg != W_TX_PAYLOAD)
        {
            //READ A REGISTRY
            ret[i] = send_spi(NOP);		//Send dummy Byte to read data
        }
        else
        {
            //Write to nRF
            send_spi(val[i]);			//Send command one at a time
        }
    }
    nRF_CEN_PORT|=(1<<nRF_CSN);		//nRf into IDLE with CSN HIGH
    return ret;					//Return the data read
}

#ifdef __cplusplus
} // extern "C"
#endif
#endif
