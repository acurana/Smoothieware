/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * MpxContainer.h
 *
 *  Created on: 10.01.2016
 *      Author: chris
 */

#ifndef SRC_MODULES_UTILS_MULTIPLEXER_MPXCONTAINER_H_
#define SRC_MODULES_UTILS_MULTIPLEXER_MPXCONTAINER_H_

#include "libs/Module.h"
#include <memory>
#include <string>
#include <map>

class Multiplexer;

/*
 * map to hold the configured multiplexers
 * key: m1, m2, m3, ...
 * unique_ptr takes care of releasing the memory
 */
typedef std::map< std::string, std::unique_ptr < Multiplexer > > mpx_map_t;


class MpxContainer: public Module {
public:
    MpxContainer();
    virtual ~MpxContainer();

    void on_module_loaded(void);
    void on_config_reload(void* argument);
    void on_get_public_data(void *argument);

    mpx_map_t multiplexers;
};

#endif /* SRC_MODULES_UTILS_MULTIPLEXER_MPXCONTAINER_H_ */

