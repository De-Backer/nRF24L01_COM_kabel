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
    Set_CSN_Low;

    send_spi( W_REGISTER | ( REGISTER_MASK & RF_CH ) );

    /* set_RF_channel */
    send_spi(frequency&0x7f);

    /* Set CSN High */
    Set_CSN_High;
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
        _delay_ms(10);
    }
    /* FLUSH TX FIFO */
    Set_CSN_Low;
    send_spi(FLUSH_TX);
    Set_CSN_High;

    /* FLUSH RX FIFO */
    Set_CSN_Low;
    send_spi(FLUSH_RX);
    Set_CSN_High;

    write_register(NRF_CONFIG,0x08);/* Configuration Register */
    write_register(EN_AA,0x3f);/* Enable ‘Auto Acknowledgment’ */
    write_register(EN_RXADDR,0x01);/* Enabled RX Addresses */
    write_register(SETUP_AW,0x03);/* Address Widths */
    write_register(SETUP_RETR,0x2f);/* Setup of Automatic Retransmission */
    write_register(RF_CH,0x02);/* RF Channel */
    write_register(RF_SETUP,0x0f);/* RF Setup Register */
    write_register(NRF_STATUS,0x7e);/* Status Register */

    Set_CSN_Low;
    /* Receive address data pipe 0 */
    send_spi( W_REGISTER | ( REGISTER_MASK & RX_ADDR_P0 ) );
    send_spi(0xe7);
    send_spi(0xe7);
    send_spi(0xe7);
    send_spi(0xe7);
    send_spi(0xe7);
    Set_CSN_High;

    Set_CSN_Low;
    /* Receive address data pipe 1 */
    send_spi( W_REGISTER | ( REGISTER_MASK & RX_ADDR_P1 ) );
    send_spi(0xc2);
    send_spi(0xc2);
    send_spi(0xc2);
    send_spi(0xc2);
    send_spi(0xc2);
    Set_CSN_High;

    write_register(RX_ADDR_P2,0xc3);/* Receive address data pipe 2 */
    write_register(RX_ADDR_P3,0xc4);/* Receive address data pipe 3 */
    write_register(RX_ADDR_P4,0xc5);/* Receive address data pipe 4 */
    write_register(RX_ADDR_P5,0xc6);/* Receive address data pipe 5 */

    Set_CSN_Low;
    /* Transmit address */
    send_spi( W_REGISTER | ( REGISTER_MASK & TX_ADDR ) );
    send_spi(0xe7);
    send_spi(0xe7);
    send_spi(0xe7);
    send_spi(0xe7);
    send_spi(0xe7);
    Set_CSN_High;

    write_register(RX_PW_P0,0x00);/* Number of bytes in RX payload in data pipe 0 */
    write_register(RX_PW_P1,0x00);/* Number of bytes in RX payload in data pipe 1 */
    write_register(RX_PW_P2,0x00);/* Number of bytes in RX payload in data pipe 2 */
    write_register(RX_PW_P3,0x00);/* Number of bytes in RX payload in data pipe 3 */
    write_register(RX_PW_P4,0x00);/* Number of bytes in RX payload in data pipe 4 */
    write_register(RX_PW_P5,0x00);/* Number of bytes in RX payload in data pipe 5 */
    write_register(DYNPD,0x00);/* Enable dynamic payload length */
    write_register(FEATURE,0x00);/* Feature Register */
}

void full_read_registers(uint8_t debug_nr)
{
    /* zent 10 keer 0xff data is maar 5 bytes */
    transmit_USART(0xff);
    transmit_USART(0xff);
    transmit_USART(0xff);
    transmit_USART(0xff);
    transmit_USART(0xff);
    transmit_USART(0xff);
    transmit_USART(0xff);
    transmit_USART(0xff);
    transmit_USART(0xff);
    transmit_USART(0xff);
    /* debug nr (lockasie) */
    transmit_USART(debug_nr);
    transmit_USART(read_register(NRF_CONFIG));
    transmit_USART(read_register(EN_AA));
    transmit_USART(read_register(EN_RXADDR));
    transmit_USART(read_register(SETUP_AW));
    transmit_USART(read_register(SETUP_RETR));
    transmit_USART(read_register(RF_CH));
    transmit_USART(read_register(RF_SETUP));
    /* test */
    transmit_USART(read_status());
    //transmit_USART(read_register(NRF_STATUS));
    transmit_USART(read_register(OBSERVE_TX));
    transmit_USART(read_register(RPD));

    Set_CSN_Low;
    send_spi( R_REGISTER | (REGISTER_MASK & RX_ADDR_P0));
    transmit_USART(send_spi(NOP));
    transmit_USART(send_spi(NOP));
    transmit_USART(send_spi(NOP));
    transmit_USART(send_spi(NOP));
    transmit_USART(send_spi(NOP));
    Set_CSN_High;

    Set_CSN_Low;
    send_spi( R_REGISTER | (REGISTER_MASK & RX_ADDR_P1));
    transmit_USART(send_spi(NOP));
    transmit_USART(send_spi(NOP));
    transmit_USART(send_spi(NOP));
    transmit_USART(send_spi(NOP));
    transmit_USART(send_spi(NOP));
    Set_CSN_High;

    transmit_USART(read_register(RX_ADDR_P2));
    transmit_USART(read_register(RX_ADDR_P3));
    transmit_USART(read_register(RX_ADDR_P4));
    transmit_USART(read_register(RX_ADDR_P5));

    Set_CSN_Low;
    send_spi( R_REGISTER | (REGISTER_MASK & TX_ADDR));
    transmit_USART(send_spi(NOP));
    transmit_USART(send_spi(NOP));
    transmit_USART(send_spi(NOP));
    transmit_USART(send_spi(NOP));
    transmit_USART(send_spi(NOP));
    Set_CSN_High;

    transmit_USART(read_register(RX_PW_P0));
    transmit_USART(read_register(RX_PW_P1));
    transmit_USART(read_register(RX_PW_P2));
    transmit_USART(read_register(RX_PW_P3));
    transmit_USART(read_register(RX_PW_P4));
    transmit_USART(read_register(RX_PW_P5));
    transmit_USART(read_register(FIFO_STATUS));
    transmit_USART(read_register(DYNPD));
    transmit_USART(read_register(FEATURE));
}

void nRF_IRQ_pin_triger()
{
    /* waarom un interupt? */
    Set_CSN_High;/* reset spi com */
    ping_RF24L01();
}

void ping_RF24L01()
{
    /* tba: idm als interupt nRF_IRQ */
    /* polling of nRF24L01 */
    cli();/* dit moet altijd gebeuren als ur spi data verstuurt word
             oftewel moet interupt nRF_IRQ geen spi gebruiken!!   */

    Set_CSN_Low;
    SPI_DATA_REGISTER = NOP;
    uint8_t info;
    do {} while (!SPI_WAIT);
    info = SPI_DATA_REGISTER;
    Set_CSN_High;

    sei();

    if(info&(1<<RX_DR))/* RX Data Ready (is er data => lees data uit) */
    {

        nRF_CE_PORT &=~(1 <<nRF_CE);/* reset pin (stop met zenden/ontvagen) */

        /* lees data en clear bit RX_DR in NRF_STATUS */
#ifdef cont_payload_bytes
        uint8_t var=cont_payload_bytes;
    #else
        uint8_t var=0;
    #endif
        if(read_register(FEATURE)&0x04)/* is enabled Dynamic Payload Length on? */
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
            /* data naar USART sturen */
            /* maak un funxie met data buffer TBA */

            Set_CSN_Low;

            SPI_DATA_REGISTER = R_RX_PAYLOAD;
            do {} while (!SPI_WAIT);
            do {
                SPI_DATA_REGISTER = NOP;
                do {} while (!SPI_WAIT);
                do {} while (!(UCSRA & (1<<UDRE)));
                UDR = SPI_DATA_REGISTER;
            } while (--var);

            Set_CSN_High;

        }
        write_register(NRF_STATUS,(1<<RX_DR));/* clear bit RX_DR in NRF_STATUS */
    }

    if(info&(1<<TX_DS))/* Data sent (de zender geeft data succesvol verzonden)*/
    {
        nRF_CE_PORT &=~(1 <<nRF_CE);/* reset pin (stop met zenden/ontvagen) */

        /* clear bit TX_DS in NRF_STATUS en reset pin nRF_CE */
        write_register(NRF_STATUS,(1<<TX_DS));/* clear bit TX_DS in NRF_STATUS */
        /* is er nog data? */

    }

    if(info&(1<<MAX_RT))/* Comm fail (geen ontvankst bevesteging van de ontvanger)*/
    {

        nRF_CE_PORT &=~(1 <<nRF_CE);/* reset pin (stop met zenden/ontvagen) */

        /* tba: afhandelen van Comm fail */

        /* clear bit MAX_RT in NRF_STATUS */
        write_register(NRF_STATUS,(1<<MAX_RT));/* clear bit MAX_RT in NRF_STATUS */
    }

    if(!((1<<TX_EMPTY)&read_register(FIFO_STATUS)))
    {
        /* if not empty */
        /* is not Transmiter */
        if(((1<<PRIM_RX)&read_register(NRF_CONFIG)))
        {
            /* set as Transmiter */
            write_register(NRF_CONFIG,NRF_CONFIG_zender);
        }

        /* start Transmitting */
        nRF_CE_PORT|=(1<<nRF_CE);
    } else {
        /* else in standby */
        nRF_CE_PORT &=~(1 <<nRF_CE);/* reset pin (stop met zenden/ontvagen) */
    }

    if(((1<<PRIM_RX)&read_register(NRF_CONFIG)))
    {
        nRF_CE_PORT &=~(1 <<nRF_CE);/* reset pin (stop met zenden/ontvagen) */
        asm ("nop");
        asm ("nop");
        /* start ontvanger */
        nRF_CE_PORT|=(1<<nRF_CE);
    }
}

uint8_t Power_Down()
{
    Set_CSN_Low;

    uint8_t reg=0;

    send_spi( R_REGISTER | (REGISTER_MASK & NRF_CONFIG));

    reg = send_spi(NOP);

    reg &= ~(1<<PWR_UP);

    send_spi( W_REGISTER | (REGISTER_MASK & NRF_CONFIG));

    reg=send_spi(reg);

    Set_CSN_High;
    /* Return NRF_CONFIG */
    return reg;
}

/* leest de status byte
 * is de eerst byte bij spi com na Set_CSN_Low
*/
uint8_t read_status()
{
    cli();

    Set_CSN_Low;
    SPI_DATA_REGISTER = NOP;
    do {} while (!SPI_WAIT);//R_Register --> Set to Reading Mode, "reg" --> The registry which will be read
    uint8_t reg = SPI_DATA_REGISTER;
    Set_CSN_High;

    sei();
    return reg;
}

uint8_t read_register(uint8_t reg)
{
    /* als interupt nRF_IRQ aktif word is er un fout.... => reset µc */
    cli();

    Set_CSN_Low;

    SPI_DATA_REGISTER = REGISTER_MASK & reg;
    do {} while (!SPI_WAIT);//R_Register --> Set to Reading Mode, "reg" --> The registry which will be read
    SPI_DATA_REGISTER = NOP;//Send DUMMY BYTE[NOP] to receive first byte in 'reg' register
    do {} while (!SPI_WAIT);
    reg = SPI_DATA_REGISTER;

    Set_CSN_High;

    sei();
    return reg;							//Return the registry read
}

void write_register(uint8_t reg, uint8_t value)
{
    cli();
    Set_CSN_Low;

    SPI_DATA_REGISTER = ( W_REGISTER | ( REGISTER_MASK & reg ) );
    do {} while (!SPI_WAIT);
    SPI_DATA_REGISTER = value;
    do {} while (!SPI_WAIT);

    Set_CSN_High;
    sei();
}

#ifdef __cplusplus
} // extern "C"
#endif
#endif
