/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * LedsContainer.cpp
 *
 *  Created on: 24.11.2016
 *      Author: chris
 */

#include "LedsContainer.h"
#include "checksumm.h"
#include "libs/Kernel.h"
#include "Config.h"
#include "ConfigValue.h"


#define enable_checksum             CHECKSUM("enable")
#define led_checksum                CHECKSUM("led")


LedsContainer::LedsContainer()
{
}


LedsContainer::~LedsContainer()
{
}


void LedsContainer::on_module_loaded(void)
{
    std::vector<uint16_t> modules;
    THEKERNEL->config->get_module_list( &modules, led_checksum );

    if (modules.empty())
    {
      delete this; // no leds configured
      return;
    }

    on_config_reload(NULL);
}


void LedsContainer::on_config_reload(void* argument)
{
    vector<uint16_t> modules;
    std::unique_ptr<Led> led;

    mLeds.clear(); // clean up in case of reload

    /*
     * this will fill the vector with the names (checksums) of all configured leds
     */
    THEKERNEL->config->get_module_list( &modules, led_checksum );

    /*
     * ok, now we create or re-create the configured leds
     */
    for( unsigned i = 0; i < modules.size(); ++i )
    {
        // if module is enabled
        if( THEKERNEL->config->value(led_checksum, modules[i], enable_checksum)->as_bool() == true )
        {
            std::unique_ptr<Led> p( new Led() );
            swap (led, p);

            if (led) {
                led->UpdateConfig(led_checksum, modules[i]);

                /*
                 * add to led vector
                 */
                mLeds.push_back(std::move(led));
            }
        }
    }
}

