/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * MpxContainer.cpp
 *
 *  Created on: 10.01.2016
 *      Author: chris
 */

#include "MpxContainer.h"
#include "libs/Kernel.h"
#include "checksumm.h"
#include "Config.h"
#include "ConfigValue.h"

#include "Mpx595.h"
#include "Mpx4051.h"
#include "Mpx165.h"

#include "MpxContainerPublicAccess.h"
#include "PublicDataRequest.h"

/*
 * support for multiplexers in config
 *
 * supported types are:
 *
 *   mpx_74xx165    parallel in / serial out    (in)
 *   mpx_4051       analogue multiplexer        (in)
 *   mpx_74xx595    serial in / parallel out    (out)
 */

#define enable_checksum             CHECKSUM("enable")
#define type_checksum               CHECKSUM("type")


MpxContainer::MpxContainer()
{
}


MpxContainer::~MpxContainer()
{
}


void MpxContainer::on_module_loaded(void)
{
    vector<uint16_t> modules;
    THEKERNEL->config->get_module_list( &modules, multiplexer_checksum );

    if (modules.empty())
    {
      delete this; // no multiplexers configured
      return;
    }

    register_for_event(ON_GET_PUBLIC_DATA);
    on_config_reload(NULL);
}


void MpxContainer::on_config_reload(void* argument)
{
    vector<uint16_t> modules;
    std::unique_ptr<Multiplexer> mpx;

    multiplexers.clear(); // clean up in case of reload

    /*
     * this will fill the vector with the names (checksums) of all configured multiplexers
     */
    THEKERNEL->config->get_module_list( &modules, multiplexer_checksum );

    /*
     * ok, now we create or re-create the configured multiplexers
     */
    for( unsigned i = 0; i < modules.size(); ++i )
    {
        // If module is enabled
        if( THEKERNEL->config->value(multiplexer_checksum, modules[i], enable_checksum)->as_bool() == true )
        {
            std::string type = THEKERNEL->config->value(multiplexer_checksum, modules[i], type_checksum)->by_default("")->as_string();

            if (type == "mpx_74xx165") {
                std::unique_ptr<Multiplexer> p( new Mpx165() );
                swap (mpx, p);
            }
            else if (type == "mpx_4051") {
                std::unique_ptr<Multiplexer> p( new Mpx4051() );
                swap (mpx, p);
            }
            else if (type == "mpx_74xx595") {
                std::unique_ptr<Multiplexer> p( new Mpx595() );
                swap (mpx, p);
            }

            if (mpx) {
                mpx->UpdateConfig(multiplexer_checksum, modules[i]);

                /*
                 * add to map, keys are m1, m2, m3,...
                 */
                char key[4]; // to_string and stoi are not available
                snprintf(key, 4, "m%d", i+1); // std::string key = "m" + std::stoi(i+1);
                multiplexers.emplace(std::string(key), std::move(mpx)); // empties mpx
            }
        }
    }
}


void MpxContainer::on_get_public_data(void *argument)
{
    PublicDataRequest *pdr = static_cast<PublicDataRequest *>(argument);

    if(! pdr->starts_with(multiplexer_checksum))
        return;

    pdr->set_data_ptr(&multiplexers);
    pdr->set_taken();
}

