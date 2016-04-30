/* USART.c
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
#ifndef _USART_c__
#define _USART_c__
#ifdef __cplusplus
extern "C"{
#endif

#include <USART.h>

// setup USART
void setup_USART()
{
    RB_usart_RX_Start=0;
    RB_usart_RX_Stop=0;
    RB_usart_RX_lenkte=0;
    RB_usart_TX_Start=0;
    RB_usart_TX_Stop=0;
    RB_usart_TX_lenkte=0;

    /*Set baud rate */
    UBRRH =(BAUD_PRESCALE>>8);
    UBRRL = BAUD_PRESCALE;

    /*	Enable 'interrupt receiver' and transmitter and UDR BUFFER empty */
    UCSRB = (1 << RXCIE ) | (1 << RXEN ) | (1 << TXEN ) ;

    UCSRC = (1 << URSEL ) | (1 << UCSZ0 ) | (1 << UCSZ1 ) ; // Use 8 - bit character sizes
#ifdef debug_USART
    transmit_string_USART("\n setup_USART ... ok");
#endif
}

void transmit_USART(uint8_t data)
{
    do {} while (!(UCSRA & (1<<UDRE)));
    UDR = data;
}

void transmit_string_USART(char* data)
{
    do
    {
        do {} while (!(UCSRA & (1<<UDRE)));
        UDR = *data;
        ++data;
    } while (*data != 0x00);
}

ISR(USART_RX_vect)
{
    PORTC=0xff;/* report interupt start */

#ifdef RB_usart_masker
    if(RB_usart_RX_lenkte<RB_usart_masker)
#else
    if(RB_usart_RX_lenkte<255)
#endif
    {
        ++RB_usart_RX_Start;
#ifdef RB_usart_masker
        RB_usart_RX_Start &= RB_usart_masker;
#endif
        ++RB_usart_RX_lenkte;
        RB_usart_RX[RB_usart_RX_Start] = UDR;
    } else {
        /* groot probleem */
#ifdef debug_USART
        PORTC=0x00;/* report interupt stop */
        PORTC=0xff;/* report interupt start */
        transmit_string_USART("\n groot probleem");
#endif
    }
    PORTC=0x00;/* report interupt stop */
}

#ifdef __cplusplus
} // extern "C"
#endif
#endif
