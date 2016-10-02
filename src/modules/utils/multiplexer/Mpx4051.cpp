/*
 * Mpx4051.cpp
 *
 *  Created on: 09.01.2016
 *      Author: chris
 */

#include "Mpx4051.h"
#include "libs/Kernel.h"
#include "libs/Pin.h"
#include "checksumm.h"
#include "Config.h"
#include "ConfigValue.h"


Mpx4051::Mpx4051() {
    // TODO Auto-generated constructor stub
}


Mpx4051::~Mpx4051() {
    // TODO Auto-generated destructor stub
}


void Mpx4051::UpdateConfig(uint16_t module_checksum, uint16_t name_checksum)
{
    /*
     * update pins/channels from config data
     */
    Multiplexer::UpdateConfig(module_checksum, name_checksum);

    // default 0.0 on fabbster @todo put in config
    clock_s0.from_string(THEKERNEL->config->value(module_checksum, name_checksum, clock_pin_checksum)->by_default("nc")->as_string())->as_output();
    // default 0.1 on fabbster
    data_s1.from_string(THEKERNEL->config->value(module_checksum, name_checksum, data_pin_checksum)->by_default("nc")->as_string())->as_output();
    // default 0.10 on fabbster
    load_s2.from_string(THEKERNEL->config->value(module_checksum, name_checksum, load_data_pin_checksum)->by_default("nc")->as_string())->as_output();

    /*
     * set pins to initial state
     */
    clock_s0.set(0);
    data_s1.set(0);
    load_s2.set(0);
}


void Mpx4051::apply_index(void)
{
//    uint32_t pin_val, mask, new_val = 0;
//    uint32_t bit_s0, bit_s1, bit_s2;
//
//    if (clock_s0.port_number == data_s1.port_number && data_s1.port_number == load_s2.port_number)
//    {   // simultaneous set
//
//        bit_s0 = 1 << clock_s0.pin;
//        bit_s1 = 1 << data_s1.pin;
//        bit_s2 = 1 << load_s2.pin;
//
//        mask = bit_s0 | bit_s1 | bit_s2;
//
//        if (index&0x01)
//            new_val |= bit_s0;
//
//        if (index&0x02)
//            new_val |= bit_s1;
//
//        if (index&0x04)
//            new_val |= bit_s2;
//
//        if (clock_s0.valid && data_s1.valid && load_s2.valid) {
//            __disable_irq();
//            pin_val = clock_s0.port->FIOPIN;
//            pin_val &= ~mask; // clear
//            pin_val |= new_val; // set
//            clock_s0.port->FIOPIN = pin_val;
//            __enable_irq();
//        }
//
//    }
//    else { // consecutive set
    bool d0, d1, d2;

    d0 = index&0x01;
    d1 = index&0x02;
    d2 = index&0x04;

    //__disable_irq();
    clock_s0.set(d0);
    data_s1.set(d1);
    load_s2.set(d2);
    //__enable_irq();
//    }
}

