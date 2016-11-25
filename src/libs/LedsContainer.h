/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * LedsContainer.h
 *
 *  Created on: 24.11.2016
 *      Author: chris
 */

#ifndef SRC_LEDSCONTAINER_H_
#define SRC_LEDSCONTAINER_H_

#include "libs/Module.h"
#include "Led.h"
#include <memory>
#include <vector>


class LedsContainer: public Module {
public:
    LedsContainer();
    virtual ~LedsContainer();

    void on_module_loaded(void);
    void on_config_reload(void* argument);

    Led& operator[](std::size_t idx) { return *mLeds[idx]; }
    const Led& operator[](std::size_t idx) const { return *mLeds[idx]; }

    std::vector<std::unique_ptr < Led > > mLeds;
};

#endif /* SRC_LEDSCONTAINER_H_ */

