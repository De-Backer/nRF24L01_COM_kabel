/* main.c
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
#ifndef _main_c__
#define _main_c__
#ifdef __cplusplus
extern "C"{
#endif

#include "nRF24L01.h"
#include "SPI.h"
#include "USART.h"
#include "RF24L01.h"
#include "device.h"



uint8_t *data;

void init_IO()
{
    //DDR
    nRF_CE_DDR   |= (1<<nRF_CE) ;
    nRF_CEN_DDR  |= (1<<nRF_CSN);

#ifdef IC_CONFIG
    IC_CONFIG_DDR &= ~(1<<IC_CONFIG);
#endif

#ifdef nRF_IRQ
    nRF_IRQ_DDR  &= ~(1<<nRF_IRQ);
#endif

#ifdef nRF_VDD
    nRF_VDD_DDR |=(1<<nRF_VDD);
#endif

    //PORT
    nRF_CEN_PORT |= (1<<nRF_CSN);

#ifdef nRF_IRQ
    nRF_IRQ_PORT |=(1<<nRF_IRQ);
#endif

#ifdef IC_CONFIG
    IC_CONFIG_PORT |=(1<<IC_CONFIG);
#endif
}
int main(void)
{
    /* setup DDR PORT */
    init_IO();
    /* setup USART */
    setup_USART();
    /* setup SPI */
    setup_spi();
    /* start up nRF24L01 */
    start_up_RF24L01();
    /* nRF24L01 Power Down Mode */
#ifndef nRF_VDD
    /* no Reset Values! => full setup nRF24L01 */
    full_reset_RF24L01();
#endif
    /* setup nRF24L01
     *
     * dit moet
     *
     * TX_ADDR == RX_ADDR_P0 => van de toestellen
     *
     * RX_PW_P0 <- standaard op 0 (not used)!! moet min. 1 max. 32
     * RX_PW_P1 <- standaard op 0 (not used)!! option
     * RX_PW_P2 <- standaard op 0 (not used)!! option
     * RX_PW_P3 <- standaard op 0 (not used)!! option
     * RX_PW_P4 <- standaard op 0 (not used)!! option
     * RX_PW_P5 <- standaard op 0 (not used)!! option
     *
     * option:
     * -RF_CH
     * -RF_SETUP
     * -EN_RXADDR <- standaard op 0x03
     * -SETUP_AW
     * --RX_ADDR_P0 == TX_ADDR
     * --RX_ADDR_P1
     * --RX_ADDR_P3
     * --RX_ADDR_P4
     * --RX_ADDR_P5
*/

    /* min. setup */
    uint8_t val[32]={32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    WriteToNrf(W,RX_PW_P0,val,1);/* Payload width to 32 */
    WriteToNrf(W,RX_PW_P1,val,1);/* Payload width to 32 */

    /*  Enhanced ShockBurst */
    val[0]=0x04;/* enabled Dynamic Payload Length */
    WriteToNrf(W,FEATURE,val,1);
    val[0]=0x03;/* enabled Dynamic Payload Length */
    WriteToNrf(W,DYNPD,val,1);
    /*Enable ‘Auto Acknowledgment’ => full_reset_RF24L01(); is standaard */
    val[0]=0x1f;/* Setup of Automatic Retransmission */
    WriteToNrf(W,SETUP_RETR,val,1);
    /* RX
     * option RX:
     *
     * als laatste de NRF_CONFIG
     * NRF_CONFIG <-standaard op TX, Power Down, interrupts on
     *
*/
    /* TX
     *
     * option TX:
     * -EN_AA <- standaard on
     * -SETUP_RETR <- standaard op 250µs, Re-Transmit 3
     * -RX_PW_P0
     *
     * als laatste de NRF_CONFIG
     * NRF_CONFIG <-standaard op TX, Power Down, interrupts on
*/

    /* start init RX or TX of nRF24L01 */

    //TBA
#if nRF_IRQ_is_avr_interupt
    /* setup interupt */
    GICR |=(1<<INT2);// External Interrupt Request 2 Enable
    GIFR |=(1<<INTF2);

    sei();
#endif
    for (;;)
    {


    }
    return 0;
}

#if nRF_IRQ_is_avr_interupt

ISR(INT2_vect)
{
    cli();
    //nRF_CE_PORT &=~(1 <<nRF_CE);/* reset pin (stop met zenden/ontvagen) */
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
            //data=WriteToNrf(R,R_RX_PAYLOAD,data,pl_lenth);
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
    }
    if(info&(1<<MAX_RT))/* Comm fale */
    {
        /* clear bit MAX_RT in NRF_STATUS */
    }

    sei();
}
#endif

#ifdef __cplusplus
} // extern "C"
#endif
#endif
