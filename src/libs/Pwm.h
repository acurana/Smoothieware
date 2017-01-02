#ifndef _PWM_H
#define _PWM_H

#include <stdint.h>

#include "Pin.h"
#include "Module.h"

class Pwm : public Module, public Pin {
public:
    Pwm();

    void     on_module_load(void);
    uint32_t on_tick(uint32_t);

    Pwm*     max_pwm(int);
    int      max_pwm(void);

    void     pwm(int);
    int      get_pwm() const { return _pwm; }
    void     set(bool);

    void set_hw_pwm(bool on);

private:
    int  _max;
    int  _pwm;
    int  _sd_accumulator;
    bool _sd_direction;

    mbed::PwmOut* hw_pwm;
    bool inverted;          // set if pwm output is inverted
};

#endif /* _PWM_H */
