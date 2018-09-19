/*
 * AdcFiltMedOvers.cpp
 *
 *  Created on: 24.10.2017
 *      Author: chris
 */

#include "AdcFiltMedOvers.h"
#include "Adc.h"
#include <algorithm>
#include <cstring>


AdcFiltMedOvers::AdcFiltMedOvers()
{
    reset();
}


AdcFiltMedOvers::~AdcFiltMedOvers()
{
}


void AdcFiltMedOvers::reset(void)
{
    memset (buf, 0, sizeof(buf));
    //memset (ave_buf, 0, sizeof(ave_buf));
    ptr = &buf[0];
}


uint16_t AdcFiltMedOvers::read(void)
{
    uint16_t median_buffer[FILTER_LENGTH];

    // needs atomic access TODO maybe be able to use std::atomic here or some lockless mutex
    __disable_irq();
    memcpy(median_buffer, buf, sizeof(median_buffer)); // @todo check assembler code for atomicity
    __enable_irq();

#ifdef USE_MEDIAN_FILTER
    // returns the median value of the last 8 samples
    return median_buffer[quick_median(median_buffer, num_samples)];
#elif defined(OVERSAMPLE)

    // Oversample to get 2 extra bits of resolution
    // weed out top and bottom worst values then oversample the rest
    std::sort(median_buffer, median_buffer + FILTER_LENGTH);
    uint32_t sum = 0;
    for (int i = FILTER_LENGTH / 4; i < (FILTER_LENGTH - (FILTER_LENGTH / 4)); ++i) {
        sum += median_buffer[i]; // 12 samples bei 24 buf len
    }

    sum /= (((FILTER_LENGTH - FILTER_LENGTH / 4) - FILTER_LENGTH / 4) >> OVERSAMPLE); // div 3 bei 24 buf len

    return sum;

//    // this slows down the rate of change a little bit
//    for (int i=FILTER_AVE_LENGTH-1; i>0; --i) {
//        ave_buf[i]= ave_buf[i-1];
//    }
//    ave_buf[0]= sum >> OVERSAMPLE; // sum of 16 values divided by 4
//
//    for (int i=0, sum=0; i<FILTER_AVE_LENGTH; ++i)
//        sum += ave_buf[i];
//
//    return (uint16_t) roundf((float)sum/4.0); // oversampled value

#else
    // sort the 8 readings and return the average of the middle 4
    std::sort(median_buffer, median_buffer + num_samples);
    int sum = 0;
    for (int i = num_samples / 4; i < (num_samples - (num_samples / 4)); ++i) {
        sum += median_buffer[i];
    }
    return sum / (num_samples / 2);

#endif
}


void AdcFiltMedOvers::write (uint16_t v)
{
    *ptr = v;

    ptr ++;

    if (ptr > buf + (FILTER_LENGTH - 1))
        ptr = &buf[0];
}

