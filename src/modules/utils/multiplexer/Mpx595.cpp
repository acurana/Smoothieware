/*
 * Mpx595.cpp
 *
 *  Created on: 09.01.2016
 *      Author: chris
 */

#include "Mpx595.h"
#include "libs/Kernel.h"
#include "libs/Pin.h"
#include "checksumm.h"
#include "Config.h"
#include "ConfigValue.h"


Mpx595::Mpx595() {
    // TODO Auto-generated constructor stub
}


Mpx595::~Mpx595() {
    // TODO Auto-generated destructor stub
}


void Mpx595::UpdateConfig(uint16_t module_checksum, uint16_t name_checksum)
{
    /*
     * update pins/channels from config data
     */
    Multiplexer::UpdateConfig(module_checksum, name_checksum);

    // @todo add fabster default comments
    clock_s0.from_string(THEKERNEL->config->value(module_checksum, name_checksum, clock_pin_checksum)->by_default("nc")->as_string())->as_output();
    data_s1.from_string(THEKERNEL->config->value(module_checksum, name_checksum, data_pin_checksum)->by_default("nc")->as_string())->as_output();
    load_s2.from_string(THEKERNEL->config->value(module_checksum, name_checksum, load_data_pin_checksum)->by_default("nc")->as_string())->as_output();

    /*
     * set pins to initial state
     */
    clock_s0.set(0);    // data is clocked on rising edge, set to 0 initially
    data_s1.set(0);
    load_s2.set(0);     // rising edge will make data appear on outputs
}

/*
 * since shifting and then loading will change all bits we need to
 * shift the cached data fully through.
 */
void Mpx595::apply_index(void)
{
    int i;

    for (i=channels-1; i >= 0; --i) // @todo check generated code
    {
        data_s1.set(data & (0x01 << i)); // highest bit is shifted in 1st
        clock_s0.set(1); // clock next data bit to data pin
        clock_s0.set(0);
    }
    load_s2.set(1);     // low pulse will latch data into shift reg
    load_s2.set(0);     // low pulse will latch data into shift reg
}

