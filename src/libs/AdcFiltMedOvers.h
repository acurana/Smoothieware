/*
 * AdcFiltMedOvers.h
 *
 *  Created on: 24.10.2017
 *      Author: chris
 */

#ifndef SRC_LIBS_ADCFILTMEDOVERS_H_
#define SRC_LIBS_ADCFILTMEDOVERS_H_

#include <stdint.h>

#define FILTER_LENGTH       24/*32*/  // sample buffer length
//#define FILTER_AVE_LENGTH   4   // used for final averaging


class AdcFiltMedOvers {
public:
    AdcFiltMedOvers();
    virtual ~AdcFiltMedOvers();

    uint16_t buf[FILTER_LENGTH];
//    uint16_t ave_buf[FILTER_AVE_LENGTH];
    uint16_t* ptr; // ptr to next write position

    void reset(void);
    uint16_t read(void);
    void write (uint16_t v);
};

#endif /* SRC_LIBS_ADCFILTMEDOVERS_H_ */

