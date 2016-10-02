/*
 * Thermocouple.h
 *
 *  Created on: 10.01.2016
 *      Author: chris
 */

#ifndef SRC_MODULES_TOOLS_TEMPERATURECONTROL_THERMOCOUPLE_H_
#define SRC_MODULES_TOOLS_TEMPERATURECONTROL_THERMOCOUPLE_H_

#include "TempSensor.h"
#include "libs/Pin.h"


/*
 * thermocouple used in fabbster 3D printer.
 * - thermo voltage is amplified by factor 200
 * - cold junction temperature is subtracted in hardware
 * - reference voltage is provided by an LM35 on board
 */

class Thermocouple: public TempSensor {
public:
    Thermocouple();
    virtual ~Thermocouple();

    void UpdateConfig(uint16_t module_checksum, uint16_t name_checksum);
    float get_temperature();
    void get_raw();

private:
    int new_thermocouple_reading();
    float adc_value_to_temperature(uint32_t adc_value);
    float linearize(float t, float coefficients[], int n);

    Pin thermistor_pin; // pin to read
    Pin ref_pin;        // pin with reference temperature (LM35)

    float min_temp, max_temp;
    struct {
        bool bad_config:1;
    };

    /*
     * // linear correction factor heater wire -> nozzle
     * value of 1000 = 1.0
     */
    int correction_factor; // linear correction factor heater wire -> nozzle
};

#endif /* SRC_MODULES_TOOLS_TEMPERATURECONTROL_THERMOCOUPLE_H_ */

