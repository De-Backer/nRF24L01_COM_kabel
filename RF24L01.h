/* RF24L01.h
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
#ifndef _RF24L01_h__
#define _RF24L01_h__
#ifdef __cplusplus
extern "C"{
#endif

/*
 * void nRF_IRQ_pin_triger()
 * 1+1
 * uint8_t Power_Down()
 * 1
 * uint8_t *WriteToNrf(uint8_t ReadWrite, uint8_t reg, uint8_t *val, uint8_t antVal)
 * 32+1
 *
 * => 36byte aan vars
*/
#define W 1
#define R 0

#include "device.h"
#include "SPI.h"
#include "nRF24L01.h"

#include <util/delay.h>


// setup RF24L01
void start_up_RF24L01();
void shutdown_RF24L01();
void set_RF_channel_frequency(uint8_t frequency);
void full_reset_RF24L01();
void full_read_registers();
void nRF_IRQ_pin_triger();
uint8_t Power_Down();
uint8_t read_register(uint8_t reg);
void write_register(uint8_t reg, uint8_t value);
uint8_t *WriteToNrf(uint8_t ReadWrite, uint8_t reg, uint8_t *val, uint8_t antVal);


#ifdef __cplusplus
} // extern "C"
#endif
#endif
