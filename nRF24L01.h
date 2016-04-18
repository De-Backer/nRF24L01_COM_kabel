/*
    Copyright (c) 2007 Stefan Engelke <mbox@stefanengelke.de>
	Portions Copyright (C) 2011 Greg Copeland

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/
#ifndef _nRF24L01_h__
#define _nRF24L01_h__
/* Memory Map */
#define NRF_CONFIG  0x00
/*      Reserved    7    only 0 allowed */
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1 /* 0: Power Down    1: Power up */
#define PRIM_RX     0

#define EN_AA       0x01 /* P != non-P */
/*      Reserved    7    only 0 allowed */
/*      Reserved    6    only 0 allowed */
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0

#define EN_RXADDR   0x02
/*      Reserved    7    only 0 allowed */
/*      Reserved    6    only 0 allowed */
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0

#define SETUP_AW    0x03
/*      Reserved    7    only 0 allowed */
/*      Reserved    6    only 0 allowed */
/*      Reserved    5    only 0 allowed */
/*      Reserved    4    only 0 allowed */
/*      Reserved    3    only 0 allowed */
/*      Reserved    2    only 0 allowed */
#define AW          0 /* 11:5_bytes 10:4_bytes 01:3_bytes */

#define SETUP_RETR  0x04
#define ARD         4 /* Auto Retransmit Delay */
#define ARC         0 /* Auto Retransmit Count */

#define RF_CH       0x05
/*      Reserved    7    only 0 allowed */
/*      RF_CH */

#define RF_SETUP    0x06 /* P != non-P */
#define CONT_WAVE   7
/*      Reserved    6    only 0 allowed */
#define RF_DR_LOW   5 /* P model bit Mnemonics */
#define PLL_LOCK    4
#define RF_DR_HIGH  3 /* P model bit Mnemonics */
#define RF_PWR_HIGH 2 /* P model bit Mnemonics */
#define RF_PWR_LOW  1 /* P model bit Mnemonics */
/*      Obsolete    0    Dont't care  was LNA_HCURR "Setup LNA gain" */
#define LNA_HCURR   0 /* Non-P omissions */

#define NRF_STATUS  0x07
/*      Reserved    7    only 0 allowed */
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1
#define TX_FULL     0

#define OBSERVE_TX  0x08
#define PLOS_CNT    4
#define ARC_CNT     0

/*  model memory Map */
#define CD          0x09
/* P model memory Map */
#define RPD         0x09

/*  model memory Map */
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10

#define RX_PW_P0    0x11
/*      Reserved    7    only 0 allowed */
/*      Reserved    6    only 0 allowed */

#define RX_PW_P1    0x12
/*      Reserved    7    only 0 allowed */
/*      Reserved    6    only 0 allowed */
#define RX_PW_P2    0x13
/*      Reserved    7    only 0 allowed */
/*      Reserved    6    only 0 allowed */
#define RX_PW_P3    0x14
/*      Reserved    7    only 0 allowed */
/*      Reserved    6    only 0 allowed */
#define RX_PW_P4    0x15
/*      Reserved    7    only 0 allowed */
/*      Reserved    6    only 0 allowed */
#define RX_PW_P5    0x16
/*      Reserved    7    only 0 allowed */
/*      Reserved    6    only 0 allowed */
#define FIFO_STATUS 0x17
/*      Reserved    7    only 0 allowed */
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
/*      Reserved    3    only 0 allowed */
/*      Reserved    2    only 0 allowed */
#define RX_FULL     1
#define RX_EMPTY    0
#define DYNPD	    0x1C
/*      Reserved    7    only 0 allowed */
/*      Reserved    6    only 0 allowed */
#define DPL_P5	    5
#define DPL_P4	    4
#define DPL_P3	    3
#define DPL_P2	    2
#define DPL_P1	    1
#define DPL_P0	    0

#define FEATURE	    0x1D
/*      Reserved    7    only 0 allowed */
/*      Reserved    6    only 0 allowed */
/*      Reserved    5    only 0 allowed */
/*      Reserved    4    only 0 allowed */
/*      Reserved    3    only 0 allowed */
#define EN_DPL	    2
#define EN_ACK_PAY  1
#define EN_DYN_ACK  0

/* Instruction Mnemonics */
#define R_REGISTER          0x00
#define W_REGISTER          0x20
#define REGISTER_MASK       0x1F
#define R_RX_PAYLOAD        0x61
#define W_TX_PAYLOAD        0xA0
#define FLUSH_TX            0xE1
#define FLUSH_RX            0xE2
#define REUSE_TX_PL         0xE3
#define ACTIVATE            0x50 /* non-P model memory Map */
#define R_RX_PL_WID         0x60
#define W_ACK_PAYLOAD       0xA8
#define W_TX_PAYLOAD_NO_ACK 0xB0 /* P model memory Map */
#define NOP                 0xFF

#endif
