/*
 * AdcFilter.cpp
 *
 *  Created on: 03.10.2016
 *      Author: chris
 */

#include "AdcFilter.h"
#include <cstring>


AdcFilter::AdcFilter()
{
    reset();
}


AdcFilter::~AdcFilter()
{
}


void AdcFilter::reset(void)
{
    memset (buf, 0, sizeof(buf));
    sum = 0;
    oldest = buf;
}


uint16_t AdcFilter::read(void)
{
    uint16_t result;
    result = sum >> 3; // /8, depends on filter length, see ADC_FILTER_LENGTH
    return result;
}


void AdcFilter::write (uint16_t v)
{
    sum -= *oldest; // remove oldest sample
    *oldest = v;    // store new sample
    sum += v;       // add new sample

    oldest ++;      // points to new oldest sample

    if (oldest > buf + (ADC_FILTER_LENGTH - 1))
        oldest = buf;
}

