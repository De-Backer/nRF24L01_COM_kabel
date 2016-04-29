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

    debug_DDR=0xff;

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

    debug_PORT=0xff;
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
    full_read_registers(0);
    full_reset_RF24L01();
    full_read_registers(1);
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
    write_register(RX_PW_P0,cont_payload_bytes);

    /*  Enhanced ShockBurst */
    //val[0]=0x04;/* enabled Dynamic Payload Length */
    //WriteToNrf(W,FEATURE,val,1);
    //[0]=0x03;/* enabled Dynamic Payload Length */
    //WriteToNrf(W,DYNPD,val,1);
    /*Enable ‘Auto Acknowledgment’ => full_reset_RF24L01(); is standaard */
    write_register(SETUP_RETR,0x2f);
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
        write_register(NRF_CONFIG,0x4e);
    } else {
        /* ontvanger */
        write_register(NRF_CONFIG,0x3f);
    }
#else
    /* ontvanger */
    write_register(NRF_CONFIG,0x3f);
#endif
    _delay_us(1500);
    /* staat nu in standby */
    //transmit_string_USART("\n standby");

#if nRF_IRQ_is_avr_interupt
    /* setup interupt */
    MCUCR |=(1<<ISC11);//The falling edge of INT1 generates an interrupt request.
    GICR |=(1<<INT1);// External Interrupt Request 2 Enable

    //GICR |=(1<<INT2);// External Interrupt Request 2 Enable
    //MCUCSR &=(~(1<<ISC2));
    //GIFR |=(1<<INTF2);

    sei();
    full_read_registers(2);

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
            debug_PORT=0x00;
            nRF_IRQ_was=0;
            nRF_IRQ_pin_triger();
            debug_PORT=0xff;
        }
        if((nRF_IRQ_Pin&(1<<nRF_IRQ))&(nRF_IRQ_was==0))/* laag naar hoog */
        {
            nRF_IRQ_was=1;
        }
#endif

        if(nRF_CE_PORT&(1<<nRF_CE))
        {

            /* nRF_CE is al 5ms aan zonder interupt reset */
            /* kontroleer de status */
            if(IC_master_Pin&(1<<IC_master)){
                /* zender */

            } else {
                ping_RF24L01();
                _delay_ms(1);/* is iets kalmer */
            }


        } else {


            /* niet aan get zenden of ontvangen */

            if((USART_RX_lenkte_RB()>cont_payload_bytes))/* is er data te versturen? */
            {
                /* data from USART */

                Set_CSN_Low

                //send_spi(W_TX_PAYLOAD);


                SPI_DATA_REGISTER=W_TX_PAYLOAD;
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//01
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//02
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//03
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//04
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//05
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//06
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//07
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//08
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//09
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//10
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//11
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//12
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//13
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//14
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//15
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//16
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//17
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//18
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//19
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//20
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//21
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//22
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//23
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//24
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//25
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//26
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//27
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//28
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//29
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//30
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//31
                ++RB_usart_RX_Stop;
#ifdef RB_usart_masker
                    RB_usart_RX_Stop &= RB_usart_masker;
#endif
                    --RB_usart_RX_lenkte;
                while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];//32
                while (!SPI_WAIT);

                //uint8_t cont=0;
                //do {
                    /* sent data from USART */

                    /* dit werkt te traag 43 µs zonder verzending  kan naar 1µs gebracht worden */
//                    send_spi(USART_RX_RB());

                    /* 31 µs zonder verzending */
                    /* ATmel: Tip #10 – Unrolling loops 23µs */
//                    ++RB_usart_RX_Stop;
//#ifdef RB_usart_masker
//                    RB_usart_RX_Stop &= RB_usart_masker;
//#endif
//                    --RB_usart_RX_lenkte;
//                    send_spi(RB_usart_RX[RB_usart_RX_Stop]);
//
//                    ++cont;
//
//                } while (cont<cont_payload_bytes);
//                } while ((USART_RX_lenkte_RB()>0)&&cont<cont_payload_bytes);

                Set_CSN_High;

                asm ("nop");


                /* config voor zenden */
                write_register(NRF_CONFIG,0x4e);
                nRF_CE_PORT|=(1<<nRF_CE);//Start Transmitting

            } else {


#ifdef IC_master
                if(IC_master_Pin&(1<<IC_master)){
                    /* zender */

                } else {
                    /* ontvanger */
                    /* zet in ontvangst modus */
                    /* als er data is word dit gewist => aan te passen */
                    write_register(NRF_CONFIG,0x3f);
                    nRF_CE_PORT|=(1<<nRF_CE);//Start Transmitting
                }
#else
    /* ontvanger */
                /* zet in ontvangst modus */
                write_register(NRF_CONFIG,0x3f);
                nRF_CE_PORT|=(1<<nRF_CE);//Start Transmitting
#endif
            }
        }

    }
    return 0;
}

#if nRF_IRQ_is_avr_interupt

ISR(INT1_vect)
{
    /* interupt read */
    //debug_PORT&=0xfe;
    /* interupt time is 9µs */
    nRF_IRQ_pin_triger();
    //debug_PORT|=0x01;
}
#endif

#ifdef __cplusplus
} // extern "C"
#endif
#endif
