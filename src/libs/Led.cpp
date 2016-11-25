/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * Led.cpp
 *
 *  Created on: 24.11.2016
 *      Author: chris
 */

#include "Led.h"
#include "libs/Kernel.h"
#include "Config.h"
#include "ConfigValue.h"
#include "checksumm.h"

#define pin_checksum          CHECKSUM("pin")


Led::Led()
{
}

Led::~Led()
{
}


void Led::UpdateConfig(uint16_t module_checksum, uint16_t name_checksum)
{
    led_pin.from_string(THEKERNEL->config->value(module_checksum, name_checksum, pin_checksum)->by_default("nc")->as_string())->as_output();

    /*
     * set pin to initial state
     */
    led_pin.set(0);
}

