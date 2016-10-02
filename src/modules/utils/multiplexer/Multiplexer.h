/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * Multiplexer.h
 *
 *  Created on: 09.01.2016
 *      Author: chris
 */

#ifndef SRC_MODULES_UTILS_MULTIPLEXER_MULTIPLEXER_H_
#define SRC_MODULES_UTILS_MULTIPLEXER_MULTIPLEXER_H_

#include "libs/Pin.h"
#include "checksumm.h"

#define clock_pin_checksum          CHECKSUM("clock_pin")
#define data_pin_checksum           CHECKSUM("data_pin")
#define load_data_pin_checksum      CHECKSUM("load_data_pin")


/*!
 * A multiplexer can be based on a 74xx165 shift register (8 bit parallel in), on
 * 74xx595 shift register (8 bit parallel out) or on an 4051 analogue multiplexer
 * (analogue in) chip.
 *
 * These 3 types of multiplexer are used on the fabbster LPC1768 CPU board.
 * There might be other boards where they can be useful.
 *
 * Currently the mpx is limited to 8 channels, but the code can be easily
 * extended.
 */

#define MPX_MAX_CH      8


class Multiplexer {
public:
    Multiplexer();
    virtual ~Multiplexer();

    typedef enum {
        CH_0 = 0x01,
        CH_1 = 0x02,
        CH_2 = 0x04,
        CH_3 = 0x08,
        CH_4 = 0x10,
        CH_5 = 0x20,
        CH_6 = 0x40,
        CH_7 = 0x80,
    }
    active_channel_t;

    // Load config parameters using provided "base" names.
    virtual void UpdateConfig(uint16_t module_checksum, uint16_t name_checksum);

    void set_index (uint8_t i);
    inline uint8_t get_index (void) {
        return index; };

    void inc_index(void);

    unsigned get_channels(void);
    void set_active(active_channel_t ac);

    inline void* & pointer(int i) {
        return ptr[i];
    }

    inline unsigned & counter(int i) {
        return cnt[i];
    }

    /*
     * set data bit in cache as per current index
     */
    virtual void set_data(bool bit);

    /*
     * sets the index bits in hardware or clocks the desired
     * data to the multiplexed pin. Depends on chip used, therefore
     * add this method as a specialization.
     *
     * Note: on the "in" multiplexer all bits are latched and then the desired
     * bit is shifted to the port pin where it can be read. Lower bits will use
     * fewer clock cycles
     *
     * On the "out" multiplexer the data byte needs to be cached and if a bit
     * was changed the entire byte needs to be shifted out in order to keep
     * the unchanged bits untouched.
     */
    virtual void apply_index(void) = 0;

protected:
    uint8_t index;          // current index
    uint8_t channels;       // num channels, currently limited to 8
    uint16_t active_chan;    // bit coded active channels
    uint16_t data;          // data cache for output bits

    /*
     * used as clock/data/load pins or as channel index
     */
    Pin clock_s0;   // clock pin / bit0
    Pin data_s1;    // real pin to write or to be read / bit1
    Pin load_s2;    // load/shift on serial variant bit / bit2

    /*
     * a pointer and counter, per channel, that can be used as necessary
     */
    void* ptr[MPX_MAX_CH];
    unsigned cnt[MPX_MAX_CH];
};

#endif /* SRC_MODULES_UTILS_MULTIPLEXER_MULTIPLEXER_H_ */

