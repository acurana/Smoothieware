/*
 * This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
 * Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Multiplexer.cpp
 *
 *  Created on: 09.01.2016
 *      Author: chris
 */

#include "Multiplexer.h"
#include <stdint.h>
#include <cassert>

#include "libs/Kernel.h"
#include "Config.h"
#include "ConfigValue.h"
#include "StreamOutputPool.h"


#define channels_checksum           CHECKSUM("channels")


Multiplexer::Multiplexer()
    : index(0), channels(0), active_chan(0), data(0)
{
    memset (cnt, 0, sizeof(cnt));
    memset (ptr, 0, sizeof(ptr));
}


Multiplexer::~Multiplexer()
{
}


void Multiplexer::UpdateConfig(uint16_t module_checksum, uint16_t name_checksum)
{
    channels = THEKERNEL->config->value(module_checksum, name_checksum, channels_checksum)->by_default(0)->as_number();
    if (channels > MPX_MAX_CH) {
        channels = MPX_MAX_CH;
        THEKERNEL->streams->printf("NOTE: mpx channels limited to %d\r\n", MPX_MAX_CH);
    }
}


void Multiplexer::set_index (uint8_t i)
{
    assert(i < channels);
    index = i;
}


void Multiplexer::set_active(active_channel_t ac)
{
    active_chan |= ac;
}


void Multiplexer::inc_index(void)
{
    for (int i=0; i<channels; ++i) // check MPX_MAX_CH channels total
    {
        int look_at = (index + 1 + i) % MPX_MAX_CH; // start at index+1, end at chan

        if (active_chan & (0x01 << look_at)) // check if channel "look_at" is active
        {
            index = look_at; // next active channel
            break;
        }
    }
}


void Multiplexer::set_data(bool bit)
{
    if (bit)
        data |= (uint16_t)(0x01 << index);    // or in the bit into the data cache
    else
        data &= (uint16_t)~(0x01 << index);   // clear the bit in the data cache
}


unsigned Multiplexer::get_channels(void)
{
    return channels;
}

