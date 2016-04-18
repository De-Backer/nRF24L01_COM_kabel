/* SPI.c
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
#ifndef _SPI_c__
#define _SPI_c__
#ifdef __cplusplus
extern "C"{
#endif

#include <SPI.h>

void setup_spi()
{
#ifdef debug_SPI
    transmit_string_USART("\n setup_spi DDR ");
#endif
    nRF_SPI_DDR  |= (1<<nRF_SCK) | (1<<nRF_MOSI);
    nRF_SPI_DDR  &= ~(1<<nRF_MISO);
#ifdef debug_SPI
    transmit_string_USART("PORT ");
#endif
    nRF_SPI_PORT |= (1<<nRF_MISO);

#ifdef debug_SPI
    transmit_string_USART("Registers ");
#endif
    //for atmega8535 @ 8MHz
    SPCR|=(1<<SPE)|(1<<MSTR)|(1<<SPI2X);
#ifdef debug_SPI
    transmit_string_USART("ok");
#endif
}

uint8_t send_spi(uint8_t out)
{
    SPDR = out;
    while (!(SPSR & (1<<SPIF)));
    return SPDR;
}

#ifdef __cplusplus
} // extern "C"
#endif


#endif
