/* device.h
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
#ifndef _device_h__
#define _device_h__
#ifdef __cplusplus
extern "C"{
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
/* te bekijken: <util/atomic.h>
 * In order to implement atomic access to multi-byte objects,
 * consider using the macros from <util/atomic.h>, rather than
 * implementing them manually with cli() and sei().
*/
//#define debug_main
//#define debug_SPI
#define debug_USART
//#define debug_RF24L01

/* clock */
#define F_CPU 14745600UL //8000000UL  // 8 MHz

/* nRF24L01 pin
 *  ____________________________________
 * |#              16MHz                |
 * |######  #                           |
 * |     #  #           8 IRQ   7 MISO  |
 * |######  #   IC      6 MOSI  5 SCK   |
 * |#       #           4 CSN   3 CE    |
 * |###########         2 VDD   1 GND   |
 * |____________________________________|
 * */

/* IO */
/* debug pinen
 * te verwijderen na korekt werking
*/
#define debug_PORT PORTC
#define debug_DDR  DDRC

/* zender */
#define IC_master_PORT PORTD
#define IC_master_DDR  DDRD
#define IC_master_Pin  PIND
#define IC_master  2


/* IC_CONFIG for setup RF24L01 on USART and USART_BAUDRATE, oder
 * TBA...
 * */
#define IC_CONFIG_PORT PORTB
#define IC_CONFIG_DDR  DDRB
#define IC_CONFIG_Pin  PINB
//#define IC_CONFIG  0

#define nRF_VDD_PORT PORTB
#define nRF_VDD_DDR  DDRB
//#define nRF_VDD  1      /* voeding nRF24L01 */
#define nRF_CEN_PORT PORTB
#define nRF_CEN_DDR  DDRB
#define nRF_CSN  4      /* CSN info
 * CSN  <> SS µc The nRF starts listening for commands when the CSN-pin goes low.
 * ON-LOW
 * Timing infomation:
 * stel: SPI clock 10MHz => 100ns per tik
 * 116ns to data valid MAX
 * de funsie _delay_ms(); or _delay_us(); is niet nodeg.
 * wel  asm ("nop"); voor aan 116ns te graken
 * zet SPI clock op 0MHz-4MHz is ok.
 * dan is SPI clock 4MHz => 250ns per tik
 */
#define nRF_CE_PORT PORTB
#define nRF_CE_DDR  DDRB
#define nRF_CE   3      /* CE info
 * is used when to either send the data (transmitter) or start receive data (receiver)
 * ON-HIGH
 * Timing infomation:
 * Power Down   to Standby mode  4.5ms
 * Standby mode to TX/RX   mode  130µs
 * Minimum CE high               10µs
 *
 */
/* interupt */
#define nRF_IRQ_PORT PORTD //PORTB
#define nRF_IRQ_DDR  DDRD  //DDRB
#define nRF_IRQ_Pin  PIND  //PINB
#define nRF_IRQ      3     //2      // HIGH to LOW INT2 interrupt
#define nRF_IRQ_is_avr_interupt 1 // 1=yes 0=no

/* SPI */
#define nRF_SPI_PORT PORTB
#define nRF_SPI_DDR  DDRB
#define nRF_SCK  7      // SCK  <> SCK µc
#define nRF_MOSI 5      // MOSI <> MOSI µc
#define nRF_MISO 6      // MISO <> MISO µc
//#define Set_CSN_Low {nRF_CEN_PORT &=~(1 <<nRF_CSN);asm ("nop");} /* Set CSN Low */
#define Set_CSN_Low {nRF_CEN_PORT &=~(1 <<nRF_CSN);} /* Set CSN Low */
#define Set_CSN_High (nRF_CEN_PORT|=(1 <<nRF_CSN)) /* Set CSN High */

#define SPI_DATA_REGISTER SPDR // SPI DATA REGISTER van de µc

/* USART */
#define USART_BAUDRATE 115200 //115200 //500000
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
uint8_t *data;

#define cont_payload_bytes 32

#ifdef __cplusplus
} // extern "C"
#endif
#endif
