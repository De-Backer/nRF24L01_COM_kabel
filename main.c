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

#if !nRF_IRQ_is_avr_interupt
void nRF_IRQ_pin_triger();
#endif

#ifndef IC_master
uint8_t is_zender=0;
#endif

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
    /* init µc input Tri-state all */
    DDRA = 0x00;
    DDRB = 0x00;
    DDRC = 0x00;
    DDRD = 0x00;

    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;

    /* start setup code */

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
#ifdef cont_payload_bytes
    write_register(RX_PW_P0,cont_payload_bytes);
#else
    /*  Enhanced ShockBurst */
    write_register(FEATURE,0x06);/* enabled Dynamic Payload Length, enabled Payload with ACK */
    write_register(DYNPD,0x03); /* enabled Dynamic Payload Length */
#endif
    /* test
     * tijd 1CE - 2IQR 199µs @ 5bytes
     * tijd 1CE - 2IQR 192µs @ 3bytes*/
    write_register(SETUP_AW,0x01);

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
        write_register(NRF_CONFIG,NRF_CONFIG_zender);
    } else {
        /* ontvanger */
        write_register(NRF_CONFIG,NRF_CONFIG_ontvanger);
    }
#else
    /* ontvanger / zender als er data is */
    write_register(NRF_CONFIG,NRF_CONFIG_ontvanger);
#ifndef IC_master
    is_zender=0;
    uint8_t timer_delai_1=0;
    //uint8_t timer_delai_2=0;
#endif
#endif
    _delay_us(1500);
    /* staat nu in standby */

#if nRF_IRQ_is_avr_interupt
    /* setup interupt */
    MCUCR |=(1<<ISC11);//The falling edge of INT1 generates an interrupt request.
    GICR |=(1<<INT1);// External Interrupt Request 2 Enable


#endif

    /* intrupt master flag */
    sei();


    /* is gedaan omdat µc vastloopt ( uit de for(;;) loopt ) fout nog niet gevonden
     * fout was interupt tijddens spi com => spi moet afgewerkt worden of interupt mag niet spi com starten
*/
    //wdt_enable(WDTO_250MS); // enable 250ms watchdog timer



    for (;;)
    {
        /* info buffers */
//        cli();
//        /* dit moet altijd gebeuren als ur spi data verstuurt word
//                 oftewel moet interupt nRF_IRQ geen spi gebruiken!!   */
//        if((RB_usart_RX_Start!=RB_usart_RX_Stop)|(RB_usart_TX_Start!=RB_usart_TX_Stop))
//        {
//            SPI_DATA_REGISTER = RB_usart_RX_Start;
//            do {} while (!SPI_WAIT);
//            SPI_DATA_REGISTER = RB_usart_RX_Stop;
//            do {} while (!SPI_WAIT);
//            SPI_DATA_REGISTER = RB_usart_RX_lenkte;
//            do {} while (!SPI_WAIT);
//            SPI_DATA_REGISTER = RB_usart_TX_Start;
//            do {} while (!SPI_WAIT);
//            SPI_DATA_REGISTER = RB_usart_TX_Stop;
//            do {} while (!SPI_WAIT);
//            SPI_DATA_REGISTER = RB_usart_TX_lenkte;
//            do {} while (!SPI_WAIT);
//        }
//        sei();

        /* is utart buffer leeg? en is utart klaar voor volgende byte */
        if((RB_usart_TX_lenkte>0)&&(UCSRA & (1<<UDRE)))
        {
            /* ATOMIC_BLOCK */
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
            {
            ++RB_usart_TX_Stop;
#ifdef RB_usart_masker_TX
            RB_usart_TX_Stop &= RB_usart_masker_TX;
#endif
            --RB_usart_TX_lenkte;
            }
            /* word getest in begin */
            //do {} while (!(UCSRA & (1<<UDRE)));
            UDR = RB_usart_TX[RB_usart_TX_Stop];
        }

        /* poll pin nRF_IRQ */
        if(!(nRF_IRQ_Pin&(1<<nRF_IRQ)))/* laag */
        {
#if !nRF_IRQ_is_avr_interupt
            nRF_IRQ_pin_triger();
#else
            ping_RF24L01();
#endif
        }
        //wdt_reset(); // keep the watchdog happy

        if(nRF_CE_PORT&(1<<nRF_CE))
        {
#ifndef nRF_IRQ
            /* no IRQ pin man status kontoleren */
            /* nog te testen */
            cli();

            Set_CSN_Low;
            SPI_DATA_REGISTER = NOP;
            uint8_t reg;
            do {} while (!SPI_WAIT);//R_Register --> Set to Reading Mode, "reg" --> The registry which will be read
            do {
                reg = SPI_DATA_REGISTER;
                Set_CSN_High;

                Set_CSN_Low;
                SPI_DATA_REGISTER = NOP;
                do {} while (!SPI_WAIT);//R_Register --> Set to Reading Mode, "reg" --> The registry which will be read
            } while(reg==SPI_DATA_REGISTER;)
            Set_CSN_High;

            sei();
            /* als reg en SPI_DATA_REGISTER gelijk zijn verwerk info status */


            if(reg&(1<<RX_DR))/* RX Data Ready (is er data => lees data uit) */
            {
                /* lees data en clear bit RX_DR in NRF_STATUS */
            }

            if(reg&(1<<TX_DS))/* Data sent (de zender geeft data succesvol verzonden)*/
            {
                /* clear bit TX_DS in NRF_STATUS en reset pin nRF_CE */

                /* is er nog data? */
            }

            if(reg&(1<<MAX_RT))/* Comm fail (geen ontvankst bevesteging van de ontvanger)*/
            {

                /* tba: afhandelen van Comm fail */
            }
#endif
#ifndef IC_master
            if(is_zender)
            {
                ++timer_delai_1;
                if(timer_delai_1==0){
                        /* is er data van ander µc? */
                        nRF_CE_PORT &=~(1 <<nRF_CE);/* reset pin (stop met ontvagen) */
                        /* zet in ontvangst modus */
                        write_register(NRF_CONFIG,NRF_CONFIG_ontvanger);
        #ifndef IC_master
            is_zender=0;
        #endif
                        nRF_CE_PORT|=(1<<nRF_CE);//Start ontvangen

                }
            } else {
                    /* als er data is moet men stopen met ontvangen en starten met zenden */
                    /* is er data te versturen? */
#ifdef cont_payload_bytes
                    if(RB_usart_RX_lenkte>cont_payload_bytes)
#else
                    if(RB_usart_RX_lenkte>0)
#endif
                    {
                        nRF_CE_PORT &=~(1 <<nRF_CE);/* reset pin (stop met ontvagen) */
                        timer_delai_1=0;
                    }
            }
#endif
        } else {

            /* niet aan get zenden of ontvangen */

            /* is er data te versturen? */
#ifdef cont_payload_bytes
            if(RB_usart_RX_lenkte>cont_payload_bytes)
#else
            if(RB_usart_RX_lenkte>0)
#endif
            {
                /* data from USART */

                Set_CSN_Low;

                SPI_DATA_REGISTER=W_TX_PAYLOAD;
#ifdef cont_payload_bytes
        uint8_t condition=cont_payload_bytes;
    #else
        uint8_t condition=RB_usart_RX_lenkte;
        if(condition>31)condition=31;
    #endif
                do {} while (!SPI_WAIT);
                SPI_DATA_REGISTER=0x00;/* dummie_of_instruxie */
                do {
                    do {} while (!SPI_WAIT);
                    SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];/* plaats in spi */

                    /* ATOMIC_BLOCK */
                    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
                    {
                        ++RB_usart_RX_Stop;/* verplaats stop */
    #ifdef RB_usart_masker_RX
                        RB_usart_RX_Stop &= RB_usart_masker_RX; /* zorg dat stop niet buiten buffer gaat */
    #endif
                        --RB_usart_RX_lenkte;
                    }
                } while (--condition);
                do {} while (!SPI_WAIT);

                Set_CSN_High;

                /* config voor zenden */
                write_register(NRF_CONFIG,NRF_CONFIG_zender);
#ifndef IC_master
            is_zender=1;/* yes */
#endif
                nRF_CE_PORT|=(1<<nRF_CE);//Start Transmitting

            } else {


#ifdef IC_master
                if(IC_master_Pin&(1<<IC_master)){
                    /* zender */

                    /* test no data verzenden voor data te krijgen */
                    Set_CSN_Low;

                    SPI_DATA_REGISTER=W_TX_PAYLOAD;
                    do {} while (!SPI_WAIT);
                    SPI_DATA_REGISTER=0x00;
                    do {} while (!SPI_WAIT);

                    Set_CSN_High;
                    /* config voor zenden */
                    write_register(NRF_CONFIG,NRF_CONFIG_zender);
                    //Start Transmitting

                    /* lees bit PWR_UP in NRF_CONFIG */

                } else {
                    /* ontvanger */
                    /* zet in ontvangst modus */
                    /* als er data is word dit gewist => aan te passen */
                    write_register(NRF_CONFIG,NRF_CONFIG_ontvanger);
                    //Start Transmitting
                }
                nRF_CE_PORT|=(1<<nRF_CE);
#else
                /* ontvanger als er geen data is */
                /* zet in ontvangst modus */
                write_register(NRF_CONFIG,NRF_CONFIG_ontvanger);
#ifndef IC_master
    is_zender=1;/* yes we foppen de if instruxi in main (na x tijd in main wordt dit tog 0) */
#endif
                nRF_CE_PORT|=(1<<nRF_CE);//Start Transmitting
#endif
            }
        }

    }
    return 0;
}

#if nRF_IRQ_is_avr_interupt
ISR(INT1_vect)/*  */
#else
void nRF_IRQ_pin_triger()/*  */
#endif
{
    cli();
    /* aanpasing:
     * ipv: de status register uit te lezen en dan te reseten
     *  reset status register en lees de voreg status
     * besparing: 5.6µs op ontvanger, op zender zelfs 10.1µs
 */

    nRF_CE_PORT &=~(1 <<nRF_CE);/* reset pin (stop met zenden/ontvagen) */

    Set_CSN_High;/* reset spi com */

    Set_CSN_Low;
    SPI_DATA_REGISTER = ( W_REGISTER | ( REGISTER_MASK & NRF_STATUS ) );
    uint8_t info;/* verplaatst omdat we hier wachten */
    do {} while (!SPI_WAIT);
    info = SPI_DATA_REGISTER;/* lees status register waarom un interupt? */
    SPI_DATA_REGISTER = (1<<RX_DR)|(1<<TX_DS)|(1<<MAX_RT);/* reset status register */
    do {} while (!SPI_WAIT);
    Set_CSN_High;

    if(info&(1<<RX_DR))/* RX Data Ready (is er data => lees data uit) */
    {


        /* lees data en clear bit RX_DR in NRF_STATUS */

        Set_CSN_Low;
        SPI_DATA_REGISTER = (REGISTER_MASK & FEATURE);
        /* verplaatst omdat we hier wachten */
#ifdef cont_payload_bytes
        uint8_t var=cont_payload_bytes;
    #else
        uint8_t var=0;
    #endif
        do {} while (!SPI_WAIT);
        SPI_DATA_REGISTER = NOP;
        do {} while (!SPI_WAIT);
        Set_CSN_High;
        if(SPI_DATA_REGISTER & 0x04)
        {
            Set_CSN_Low;

            SPI_DATA_REGISTER = R_RX_PL_WID;
            do {} while (!SPI_WAIT);
            SPI_DATA_REGISTER = NOP;
            do {} while (!SPI_WAIT);
            var=SPI_DATA_REGISTER;

            Set_CSN_High;
        }
        if(var>32)/* flush RX FIFO */
        {
            Set_CSN_Low;

            SPI_DATA_REGISTER = FLUSH_RX;
            do {} while (!SPI_WAIT);

            Set_CSN_High;
        } else {
            /* data naar USART buffer sturen */

            Set_CSN_Low;

            SPI_DATA_REGISTER = R_RX_PAYLOAD;
            do {} while (!SPI_WAIT);
            SPI_DATA_REGISTER= NOP;/* dummie_of_instruxie */
            --var;
            do {} while (!SPI_WAIT);
            if(var>0)
            {
                do {
                    SPI_DATA_REGISTER = NOP;
                    ++RB_usart_TX_Start;
    #ifdef RB_usart_masker_TX
                    RB_usart_TX_Start &= RB_usart_masker_TX;
    #endif
                    ++RB_usart_TX_lenkte;
                    do {} while (!SPI_WAIT);
                    RB_usart_TX[RB_usart_TX_Start]= SPI_DATA_REGISTER;
                } while (--var);
            }

            Set_CSN_High;

        }
    }

    //if(info&(1<<TX_DS))/* Data sent (de zender geeft data succesvol verzonden)*/
    //{
        /* clear bit TX_DS in NRF_STATUS en reset pin nRF_CE */

        /* is er nog data? */
    //}

    if(info&(1<<MAX_RT))/* Comm fail (geen ontvankst bevesteging van de ontvanger)*/
    {

        /* tba: afhandelen van Comm fail */

        /* zet in ontvangst modus */
        write_register(NRF_CONFIG,NRF_CONFIG_ontvanger);
        #ifndef IC_master
        is_zender=2;
        #endif
        nRF_CE_PORT|=(1<<nRF_CE);//Start Transmitting
        return;
    }

    Set_CSN_Low;
    SPI_DATA_REGISTER = (REGISTER_MASK & FIFO_STATUS);
    do {} while (!SPI_WAIT);
    SPI_DATA_REGISTER = NOP;
    do {} while (!SPI_WAIT);
    Set_CSN_High;
    info=SPI_DATA_REGISTER;

    /* test als er data in TX FIFO is */
    if(!((1<<TX_EMPTY)&info))
    {
        /* if not empty */
        /* is not Transmiter */
        Set_CSN_Low;
        SPI_DATA_REGISTER = (REGISTER_MASK & NRF_CONFIG);
        do {} while (!SPI_WAIT);
        SPI_DATA_REGISTER = NOP;
        do {} while (!SPI_WAIT);
        Set_CSN_High;
        if(SPI_DATA_REGISTER & (1<<PRIM_RX))
        {
            /* set as Transmiter */
            Set_CSN_Low;
            SPI_DATA_REGISTER = ( W_REGISTER | ( REGISTER_MASK & NRF_CONFIG ) );
            do {} while (!SPI_WAIT);
            SPI_DATA_REGISTER = NRF_CONFIG_zender;
#ifndef IC_master
            is_zender=1;/* yes */
#endif
            do {} while (!SPI_WAIT);
            Set_CSN_High;
        }
        /* start Transmitting */
        nRF_CE_PORT|=(1<<nRF_CE);
    } else {
        /* test als er data in RX FIFO is */
        if(!((1<<RX_EMPTY)&info)){
            /* er is data in en fout in com spi */


            /* lees data en clear bit RX_DR in NRF_STATUS */
#ifdef cont_payload_bytes
        uint8_t var=cont_payload_bytes;
    #else
        uint8_t var=0;
    #endif

            Set_CSN_Low;
            SPI_DATA_REGISTER = (REGISTER_MASK & FEATURE);
            do {} while (!SPI_WAIT);
            SPI_DATA_REGISTER = NOP;
            do {} while (!SPI_WAIT);
            Set_CSN_High;
            if(SPI_DATA_REGISTER & 0x04)
            {
                Set_CSN_Low;

                SPI_DATA_REGISTER = R_RX_PL_WID;
                do {} while (!SPI_WAIT);
                SPI_DATA_REGISTER = NOP;
                do {} while (!SPI_WAIT);
                var=SPI_DATA_REGISTER;

                Set_CSN_High;
            }
            if(var>32)/* flush RX FIFO */
            {
                Set_CSN_Low;

                SPI_DATA_REGISTER = FLUSH_RX;
                do {} while (!SPI_WAIT);

                Set_CSN_High;
            } else {
                /* data naar USART buffer sturen */

                Set_CSN_Low;

                SPI_DATA_REGISTER = R_RX_PAYLOAD;
                do {} while (!SPI_WAIT);
                SPI_DATA_REGISTER= NOP;/* dummie_of_instruxie */
                --var;
                do {} while (!SPI_WAIT);
                if(var>0)
                {
                    do {
                        SPI_DATA_REGISTER = NOP;
                        ++RB_usart_TX_Start;
        #ifdef RB_usart_masker_TX
                        RB_usart_TX_Start &= RB_usart_masker_TX;
        #endif
                        ++RB_usart_TX_lenkte;
                        do {} while (!SPI_WAIT);
                        RB_usart_TX[RB_usart_TX_Start]= SPI_DATA_REGISTER;
                    } while (--var);
                }

                Set_CSN_High;

            }
        }
        /* else in standby */
        nRF_CE_PORT &=~(1 <<nRF_CE);/* reset pin (stop met zenden/ontvagen) */
    }


#ifdef IC_master
            /* zender en ontvager zijn bepaalt */

    Set_CSN_Low;
    SPI_DATA_REGISTER = (REGISTER_MASK & NRF_CONFIG);
    do {} while (!SPI_WAIT);
    SPI_DATA_REGISTER = NOP;
    do {} while (!SPI_WAIT);
    Set_CSN_High;
    if(SPI_DATA_REGISTER & (1<<PRIM_RX))
    {

        /* is er data te versturen? */
#ifdef cont_payload_bytes
        if(RB_usart_RX_lenkte>cont_payload_bytes)
#else
        if(RB_usart_RX_lenkte>0)
#endif
        {
            /* data from USART */

            Set_CSN_Low;
            SPI_DATA_REGISTER=W_ACK_PAYLOAD;/* + PIPE ex: 0x00 = addr P0 */
#ifdef cont_payload_bytes
    uint8_t condition=cont_payload_bytes;
#else
    uint8_t condition=RB_usart_RX_lenkte;
    if(condition>31)condition=31;
#endif
            do {} while (!SPI_WAIT);
            SPI_DATA_REGISTER=0x00;/* dummie_of_instruxie */
            do {
                do {} while (!SPI_WAIT);
                SPI_DATA_REGISTER=RB_usart_RX[RB_usart_RX_Stop];/* plaats in spi */
                ++RB_usart_RX_Stop;/* verplaats stop */
#ifdef RB_usart_masker_RX
                    RB_usart_RX_Stop &= RB_usart_masker_RX; /* zorg dat stop niet buiten buffer gaat */
#endif
                    --RB_usart_RX_lenkte;
            } while (--condition);
            do {} while (!SPI_WAIT);

            Set_CSN_High;

        }
        /* start ontvanger */
        nRF_CE_PORT|=(1<<nRF_CE);
    }
#else
            /* zijn all als ontvager-zender */
#endif
    sei();
}

#ifdef __cplusplus
} // extern "C"
#endif
#endif
