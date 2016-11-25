/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * Led.h
 *
 *  Created on: 24.11.2016
 *      Author: chris
 */

#ifndef SRC_LIBS_LED_H_
#define SRC_LIBS_LED_H_

#include <stdint.h>
#include "libs/Pin.h"


class Led {
public:
    Led();
    virtual ~Led();

    // Load config parameters using provided "base" names.
    virtual void UpdateConfig(uint16_t module_checksum, uint16_t name_checksum);

    Led& operator=(const int& other) { led_pin.set(other); return *this; };
    Led& operator=(int& other) {led_pin.set(other); return *this; }

    Pin led_pin;
};

#endif /* SRC_LIBS_LED_H_ */

