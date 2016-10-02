/*
 * Thermocouple.cpp
 *
 *  Created on: 10.01.2016
 *      Author: chris
 *
 *      fabbster specific k-type thermocouple
 *
 *      can be adapted to other thermocouples
 */

#include "Thermocouple.h"
#include "libs/Kernel.h"
#include "libs/Pin.h"
#include "checksumm.h"
#include "Config.h"
#include "ConfigValue.h"
#include "Adc.h"
#include "StreamOutputPool.h"


#define THCP_K_REF_V      30    // volts * 10, fabbster uses a 3V reference


#define thermistor_pin_checksum     CHECKSUM("thermistor_pin")
#define ref_pin_checksum            CHECKSUM("ref_pin")
#define type_checksum               CHECKSUM("type")
#define corr_fact_checksum          CHECKSUM("corr_fact")


Thermocouple::Thermocouple()
{
    this->bad_config = false;
    min_temp= 999;
    max_temp= 0;

    correction_factor = 1000; // = 1.0
}


Thermocouple::~Thermocouple() {
}


void Thermocouple::UpdateConfig(uint16_t module_checksum, uint16_t name_checksum)
{
    // pin 0.3 on fabbster, multiplexed with a 4051
    this->thermistor_pin.from_string(THEKERNEL->config->value(module_checksum, name_checksum, thermistor_pin_checksum )->required()->as_string());
    THEKERNEL->adc->enable_pin(&thermistor_pin);

    // pin 0.2 on fabbster, LM35
    ref_pin.from_string(THEKERNEL->config->value(module_checksum, name_checksum, ref_pin_checksum )->required()->as_string());
    THEKERNEL->adc->enable_pin(&ref_pin);

    if (! thermistor_pin.connected() || ! ref_pin.connected()) {
        this->bad_config= true;
        return;
    }

    // assuming linar temperature drop from heater wire to nozzle
    correction_factor = THEKERNEL->config->value(module_checksum, name_checksum, corr_fact_checksum )->by_default(correction_factor)->as_number();
}


float Thermocouple::get_temperature()
{
    if (bad_config)
        return infinityf();

    float t = adc_value_to_temperature(new_thermocouple_reading());

    // keep track of min/max for M305
    if (t > max_temp)
        max_temp = t;

    if (t < min_temp)
        min_temp = t;

    return t;
}


void Thermocouple::get_raw()
{
    if(this->bad_config) {
       THEKERNEL->streams->printf("WARNING: The config is bad for this temperature sensor\n");
    }

    int adc_value = new_thermocouple_reading();
    float t = adc_value_to_temperature(adc_value);

    //const uint32_t max_adc_value = THEKERNEL->adc->get_max_value();

    THEKERNEL->streams->printf("adc= %d, temp= %.1f°C, min= %f, max= %f, delta= %f\n", adc_value, t, min_temp, max_temp, max_temp-min_temp);

    // reset the min/max
    min_temp = max_temp = t;
}


float Thermocouple::adc_value_to_temperature(uint32_t adc)
{
    // 0x0fff regular, 0x3ffc OVERSAMPLED
    const uint32_t max_adc_value = THEKERNEL->adc->get_max_value();

    float result;
    int ref, int_res, ref_res; // we do some integer arithmetic

    ref = THEKERNEL->adc->read(&ref_pin); // fabbster LM35 temp. reference

    /*
     * fabbster LM35 reference temperature.
     * We calculate the value without the div by 4095 and with
     * a factor of 10*1000 included (30*1000 instead of 3.0V)
     * real formula: (3,0V / 4095) * adc = volts
     * LM35: 10 mV / 1°C
     */
    ref_res = THCP_K_REF_V * ref * 1000;
    ref_res = ref_res / max_adc_value; // [mV*10]

    /*
     * fabbster multiplexed temperature input.
     * K-type thermocouple electronically compensated with
     * a 2nd thermocouple, the room temperature thermo voltage is
     * subtracted. The resulting voltage is amplified by a factor of
     * 200 via op amp. Formula: (3,0V / 4095) * adc = volt.
     * 8.138 mV / 1°C after amplification, 200°C = 8.138 mV k-type.
     * We get this up to 10 mV / °C by multiplying with 1.229
     * We assume linear behaviour of the thermocouple for now
     * taking the reference point at 200°C. That should give good
     * enough accuracy for PLA/ABS
     */

    int_res = THCP_K_REF_V * adc * 1229; // [mV*10] now 10mV/°C * 10
    int_res = int_res / 4095; // not oversampled !!! 4095

    //int_res *= 660; // correction factor to get the temperature at the nozzle
    int_res = int_res * correction_factor / 1000;

    result = (float)(int_res + ref_res) / 100; // [°C]

    return result;
}


int Thermocouple::new_thermocouple_reading()
{
    if(thermistor_pin.is_multiplexed())
        return THEKERNEL->adc->read_mpx(&thermistor_pin);
    else
        // filtering now done in ADC
        return THEKERNEL->adc->read(&thermistor_pin);
}


float Thermocouple::linearize(float t, float coefficients[], int n)
{
    // http://srdata.nist.gov/its90/download/type_k.tab
    return infinityf();
}

