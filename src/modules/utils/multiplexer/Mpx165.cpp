/*
 * Mpx165.cpp
 *
 *  Created on: 09.01.2016
 *      Author: chris
 */

#include "Mpx165.h"
#include "libs/Kernel.h"
#include "libs/Pin.h"
#include "checksumm.h"
#include "Config.h"
#include "ConfigValue.h"


Mpx165::Mpx165()
{
}

Mpx165::~Mpx165()
{
}


// D7 = 5V1 on fabbster board, index 0
// ...
// D0 = 5V8

void Mpx165::UpdateConfig(uint16_t module_checksum, uint16_t name_checksum)
{
    /*
     * update pins/channels from config data
     */
    Multiplexer::UpdateConfig(module_checksum, name_checksum);

    // default 2.2 on fabbster
    clock_s0.from_string(THEKERNEL->config->value(module_checksum, name_checksum, clock_pin_checksum)->by_default("nc")->as_string())->as_output();
    // default 2.1 on fabbster
    load_s2.from_string(THEKERNEL->config->value(module_checksum, name_checksum, load_data_pin_checksum)->by_default("nc")->as_string())->as_output();

    /*
     * set pins to initial state
     */
    clock_s0.set(0);    // data is clocked on rising edge, set to 0 initially
    load_s2.set(1);     // low pulse will latch data into shift reg
}

/*
 * this function takes approx. 3 usec. for index 7 (measured with scope)
 */
void Mpx165::apply_index(void)
{
    uint8_t i;

    /*
     * get a snapshot of the bits at the parallel inputs
     */
    load_s2.set(0); // low pulse
    load_s2.set(1); // @todo measure time of pulse with scope, with compiler optimisations turned on

    // now 5V1 / D7 is present at the data pin, index 0 (fabbster)

    for (i=0; i<index; ++i) // will not execute if index = 0
    {
        clock_s0.set(1);    // clock next data bit to data pin
        clock_s0.set(0);
    }

    // now 5Vx, x=index is present at the data pin (fabbster)
}

