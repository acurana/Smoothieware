/*
 * Mpx4051.h
 *
 *  Created on: 09.01.2016
 *      Author: chris
 */

#ifndef SRC_MODULES_UTILS_MULTIPLEXER_MPX4051_H_
#define SRC_MODULES_UTILS_MULTIPLEXER_MPX4051_H_

#include "Multiplexer.h"


class Mpx4051: public Multiplexer {
public:
    Mpx4051();
    virtual ~Mpx4051();

    virtual void UpdateConfig(uint16_t module_checksum, uint16_t name_checksum);
    virtual void apply_index(void);
};

#endif /* SRC_MODULES_UTILS_MULTIPLEXER_MPX4051_H_ */

