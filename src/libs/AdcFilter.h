/*
 * AdcFilter.h
 *
 *  Created on: 03.10.2016
 *      Author: chris
 */

#ifndef SRC_LIBS_ADCFILTER_H_
#define SRC_LIBS_ADCFILTER_H_

#include <stdint.h>


/*
 * moving average filter
 */

#define ADC_FILTER_LENGTH       8


class AdcFilter {
public:
    AdcFilter();
    virtual ~AdcFilter();

    uint16_t buf[ADC_FILTER_LENGTH];
    uint32_t sum;
    uint16_t* oldest;

    void reset(void);
    uint16_t read(void);
    void write (uint16_t v);
};

#endif /* SRC_LIBS_ADCFILTER_H_ */

