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

    nRF_CEN_PORT &=~(1 <<nRF_CSN);          //Set CSN Low
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
