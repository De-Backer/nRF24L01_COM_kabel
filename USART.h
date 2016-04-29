/* USART.h
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
#ifndef _USART_h__
#define _USART_h__
#ifdef __cplusplus
extern "C"{
#endif

#include "device.h"
/*
 * 6+127+127byte = 260byte SRAM for de ring buffer
 * ander geen vars gemaakt
*/
//#define RB_usart_masker 0b11111111 /* 127byte masker */
uint8_t RB_usart_RX_Start;
uint8_t RB_usart_RX_Stop;
uint8_t RB_usart_RX_lenkte;
#ifdef RB_usart_masker
uint8_t RB_usart_RX[RB_usart_masker+1];
#else
uint8_t RB_usart_RX[256];
#endif

// setup USART
void setup_USART();

void transmit_USART(uint8_t data);
void transmit_string_USART(char* data);

ISR(USART_RX_vect);
ISR(USART_UDRE_vect);
#ifdef __cplusplus
} // extern "C"
#endif
#endif
