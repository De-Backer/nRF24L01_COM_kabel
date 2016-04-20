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

#if !nRF_IRQ_is_avr_interupt
        /* toestand pin nRF_IRQ */
    uint8_t nRF_IRQ_was=1;
#endif
    for (;;)
    {
#if !nRF_IRQ_is_avr_interupt
        /* poll pin nRF_IRQ */
        if((nRF_IRQ_Pin&(~(1<<nRF_IRQ)))&nRF_IRQ_was)/* hoog naar laag */
        {
            nRF_IRQ_was=0;
            nRF_IRQ_pin_triger();
        }
        if((nRF_IRQ_Pin&(1<<nRF_IRQ))&(nRF_IRQ_was==0))/* laag naar hoog */
        {
            nRF_IRQ_was=1;
        }
#endif

    }
    return 0;
}

#if nRF_IRQ_is_avr_interupt

ISR(INT2_vect)
{
    cli();
    nRF_IRQ_pin_triger();
    sei();
}
#endif

#ifdef __cplusplus
} // extern "C"
#endif
#endif
