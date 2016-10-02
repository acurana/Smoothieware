/*
 * Mpx165.h
 *
 *  Created on: 09.01.2016
 *      Author: chris
 */

#ifndef SRC_MODULES_UTILS_MULTIPLEXER_MPX165_H_
#define SRC_MODULES_UTILS_MULTIPLEXER_MPX165_H_

#include "Multiplexer.h"


class Mpx165: public Multiplexer {
public:
    Mpx165();
    virtual ~Mpx165();

    virtual void UpdateConfig(uint16_t module_checksum, uint16_t name_checksum);
    virtual void apply_index(void);
};

#endif /* SRC_MODULES_UTILS_MULTIPLEXER_MPX165_H_ */

