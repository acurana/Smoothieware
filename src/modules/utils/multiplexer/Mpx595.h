/*
 * Mpx595.h
 *
 *  Created on: 09.01.2016
 *      Author: chris
 */

#ifndef SRC_MODULES_UTILS_MULTIPLEXER_MPX595_H_
#define SRC_MODULES_UTILS_MULTIPLEXER_MPX595_H_

#include "Multiplexer.h"


class Mpx595: public Multiplexer {
public:
    Mpx595();
    virtual ~Mpx595();

    virtual void UpdateConfig(uint16_t module_checksum, uint16_t name_checksum);
    virtual void apply_index(void);
};


#endif /* SRC_MODULES_UTILS_MULTIPLEXER_MPX595_H_ */

