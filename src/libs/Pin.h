#ifndef PIN_H
#define PIN_H

#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "libs/LPC17xx/sLPC17xx.h" // smoothed mbed.h lib
#include "PinNames.h"

namespace mbed {
    class PwmOut;
    class InterruptIn;
}

class Multiplexer;


class Pin {
    public:
        Pin();

        Pin* from_string(std::string value);

        inline bool connected(){
            return this->valid;
        }

        inline bool equals(const Pin& other) const {
            return (this->pin == other.pin) && (this->port == other.port);
        }

        inline Pin* as_output(){
            if (this->valid)
                this->port->FIODIR |= 1<<this->pin;
            return this;
        }

        inline Pin* as_input(){
            if (this->valid)
                this->port->FIODIR &= ~(1<<this->pin);
            return this;
        }

        Pin* as_open_drain(void);

        Pin* as_repeater(void);

        Pin* pull_up(void);

        Pin* pull_down(void);

        Pin* pull_none(void);

        inline bool get(){
            if (!this->valid) return false;
            if (! multiplexed)
                return this->inverting ^ (( this->port->FIOPIN >> this->pin ) & 1);
            else
                return get_multiplexed();
        }

        inline void set(bool value)
        {
            if (!this->valid) return;
            if (! multiplexed) {
                if ( this->inverting ^ value )
                    this->port->FIOSET = 1 << this->pin;
                else
                    this->port->FIOCLR = 1 << this->pin;
            }
            else
                set_multiplexed(value);
        }

        mbed::PwmOut *hardware_pwm();

        mbed::InterruptIn *interrupt_pin();

        // these should be private, and use getters
        LPC_GPIO_TypeDef* port;

        unsigned char pin;
        char port_number;
        struct {
            bool inverting:1;
            bool valid:1;
            bool multiplexed:1;
        };

        void multiplexed_pin(char const* cp);	// for "from_string"

        bool get_multiplexed(void);				// read pin
        void set_multiplexed(bool value);       // write pin

        uint8_t get_mpx_index(void);            // returns the multiplexer channel for that pin
        //void set_mpx_index(uint8_t);

        //void set_mpx_channel(void);				// apply mpx channel on the pins

        bool is_multiplexed(void);				// check if pin is mpxed

        //unsigned get_mpx_num_chan(void);

        uint16_t get_adc_ch(void);
        void set_adc_ch(uint16_t ch);

        Multiplexer* get_mpx_ptr(void);

    private:
        /*
         * a pin may be multiplexed via analogue switch or shift register
         */
        Multiplexer* mpx;
        uint8_t idx;        // position in mpxer
        uint16_t  adc_ch;   // the corresponding adc channel
};




#endif

