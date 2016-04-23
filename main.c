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

#include "device.h"
#include "nRF24L01.h"
#include "SPI.h"
#include "USART.h"
#include "RF24L01.h"



void init_IO()
{
    //DDR
    nRF_CE_DDR   |= (1<<nRF_CE) ;
    nRF_CEN_DDR  |= (1<<nRF_CSN);

#ifdef IC_CONFIG
    IC_CONFIG_DDR &= ~(1<<IC_CONFIG);
#endif

#ifdef IC_master
    IC_master_DDR &= ~(1<<IC_master);/* zender */
#endif

#ifdef nRF_IRQ
    nRF_IRQ_DDR  &= ~(1<<nRF_IRQ);/* input */
#endif

#ifdef nRF_VDD
    nRF_VDD_DDR |=(1<<nRF_VDD);
#endif

    //PORT
    nRF_CEN_PORT |= (1<<nRF_CSN);

#ifdef nRF_IRQ
    nRF_IRQ_PORT |=(1<<nRF_IRQ);/* pul-up */
#endif

#ifdef IC_CONFIG
    IC_CONFIG_PORT |=(1<<IC_CONFIG);/* pul-up */
#endif

#ifdef IC_master
    IC_master_PORT |=(1<<IC_master);/* pul-up */
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
    full_read_registers();
    full_reset_RF24L01();
    full_read_registers();
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
    uint8_t val[32]={cont_payload_bytes,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    WriteToNrf(W,RX_PW_P0,val,1);/* Payload width to cont_payload_bytes */
    WriteToNrf(W,RX_PW_P1,val,1);/* Payload width to cont_payload_bytes */

    /*  Enhanced ShockBurst */
    //val[0]=0x04;/* enabled Dynamic Payload Length */
    //WriteToNrf(W,FEATURE,val,1);
    //[0]=0x03;/* enabled Dynamic Payload Length */
    //WriteToNrf(W,DYNPD,val,1);
    /*Enable ‘Auto Acknowledgment’ => full_reset_RF24L01(); is standaard */
    val[0]=0xff;/* Setup of Automatic Retransmission */
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

#ifdef IC_master
    if(IC_master_Pin&(1<<IC_master)){
        /* zender */
        val[0]=0x0e;/* config voor zender */
    } else {
        /* ontvanger */
        val[0]=0x0f;/* config voor ontvangen */
    }
#else
    /* ontvanger */
    val[0]=0x0f;/* config voor ontvangen */
#endif
    WriteToNrf(W,NRF_CONFIG,val,1);
    _delay_us(1500);
    /* staat nu in standby */
    //transmit_string_USART("\n standby");

#if nRF_IRQ_is_avr_interupt
    /* setup interupt */
    GICR |=(1<<INT2);// External Interrupt Request 2 Enable
    GIFR |=(1<<INTF2);

    sei();
    full_read_registers();

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

#ifdef debug_main
        transmit_USART(0xff);//start
        transmit_USART(0x01);//debug nr
        transmit_USART(nRF_CE_PORT);//01011000
#endif

        if(nRF_CE_PORT&(1<<nRF_CE))
        {
#ifdef debug_main
            transmit_USART(0xff);//start
            transmit_USART(0x02);//debug nr
            transmit_USART(nRF_CE_PORT);//01011100
#endif

            /* nRF_CE is al 5ms aan zonder interupt reset */
            nRF_CE_PORT &=~(1 <<nRF_CE);/* reset pin (stop met zenden/ontvagen) */

            _delay_us(20);
            //full_read_registers();
            _delay_us(20);

            /* reset status nRF24L01 */
            /* Set CSN Low */
            nRF_CEN_PORT &=~(1 <<nRF_CSN);

            asm ("nop");

            send_spi(W_REGISTER+NRF_STATUS);
            send_spi(0x70);

            /* Set CSN High */
            nRF_CEN_PORT|=(1<<nRF_CSN);

            _delay_us(20);
            //full_read_registers();
            _delay_us(20);

#ifdef debug_main
            transmit_USART(0xff);//start
            transmit_USART(0x03);//debug nr
            transmit_USART(nRF_CE_PORT);//01011000
#endif
        } else {

#ifdef debug_main
            transmit_USART(0xff);//start
            transmit_USART(0x04);//debug nr
            transmit_USART(USART_RX_lenkte_RB());
#endif

            /* niet aan get zenden of ontvangen */

            if((USART_RX_lenkte_RB()>cont_payload_bytes))/* is er data te versturen? */
            {
                /* data from USART */


                /* Set CSN Low */
                nRF_CEN_PORT &=~(1 <<nRF_CSN);

                asm ("nop");

                send_spi(W_TX_PAYLOAD);

                uint8_t cont=0;
                do {
                    /* sent data from USART */
                    send_spi(USART_RX_RB());
                    transmit_USART(cont);
                    ++cont;

                } while ((USART_RX_lenkte_RB()>0)&&cont<cont_payload_bytes);

                /* Set CSN High */
                nRF_CEN_PORT|=(1<<nRF_CSN);

                asm ("nop");

                val[0]=0b00001110;/* config voor zenden */
                WriteToNrf(W,NRF_CONFIG,val,1);

#ifdef debug_main
                transmit_USART(0xff);//start
                transmit_USART(0x05);//debug nr
                transmit_USART(read_register(NRF_CONFIG));
#endif

            } else {

#ifdef debug_main
                transmit_USART(0xff);//start
                transmit_USART(0x06);//debug nr
                transmit_USART(read_register(NRF_CONFIG));//0xf
#endif

#ifdef IC_master
                if(IC_master_Pin&(1<<IC_master)){
                    /* zender */

                } else {
                    /* ontvanger */
                    /* zet in ontvangst modus */
                    val[0]=0x0f;/* config voor ontvangen */
                    WriteToNrf(W,NRF_CONFIG,val,1);
                }
#else
    /* ontvanger */
                /* zet in ontvangst modus */
                val[0]=0x0f;/* config voor ontvangen */
                WriteToNrf(W,NRF_CONFIG,val,1);
#endif

#ifdef debug_main
                transmit_USART(0xff);//start
                transmit_USART(0x07);//debug nr
                transmit_USART(read_register(NRF_CONFIG));
#endif
            }

            nRF_CE_PORT|=(1<<nRF_CE);//Start Transmitting
            _delay_ms(10);/* Moet blijkbaar min. 5ms Simon
                           * waarom?
                           **/
#ifdef debug_main
            transmit_USART(0xff);//start
            transmit_USART(0x08);//debug nr
            transmit_USART(nRF_CE_PORT);//0101 1100
#endif
        }

    }
    return 0;
}

#if nRF_IRQ_is_avr_interupt

ISR(INT2_vect)
{
    cli();
    transmit_string_USART("\n nRF_IRQ");
    nRF_IRQ_pin_triger();
    sei();
}
#endif

#ifdef __cplusplus
} // extern "C"
#endif
#endif
