#include "Pin.h"
#include "utils.h"

// mbed libraries for hardware pwm
#include "PwmOut.h"
#include "InterruptIn.h"
#include "PinNames.h"
#include "port_api.h"

#include "PublicData.h"
#include "Multiplexer.h"
#include "MpxContainer.h"
#include "MpxContainerPublicAccess.h"

// chris debug
#include "libs/Kernel.h"
#include "StreamOutputPool.h"


Pin::Pin()
    : mpx(NULL), idx(0), adc_ch(0xffff)
{
    this->inverting= false;
    this->valid= false;
    multiplexed = false;
    this->pin= 32;
    this->port= nullptr;
}

// Make a new pin object from a string
Pin* Pin::from_string(std::string value){
    if(value == "nc") {
        this->valid= false;
        return this; // optimize the nc case
    }

    LPC_GPIO_TypeDef* gpios[5] ={LPC_GPIO0,LPC_GPIO1,LPC_GPIO2,LPC_GPIO3,LPC_GPIO4};

    // cs is the current position in the string
    const char* cs = value.c_str();
    // cn is the position of the next char after the number we just read
    char* cn = NULL;
    valid= true;

    // grab first integer as port. pointer to first non-digit goes in cn
    this->port_number = strtol(cs, &cn, 10);
    // if cn > cs then strtol read at least one digit
    if ((cn > cs) && (port_number <= 4)){
        // translate port index into something useful
        this->port = gpios[(unsigned int) this->port_number];
        // if the char after the first integer is a . then we should expect a pin index next
        if (*cn == '.'){
            // move pointer to first digit (hopefully) of pin index
            cs = ++cn;

            // grab pin index.
            this->pin = strtol(cs, &cn, 10);

            // if strtol read some numbers, cn will point to the first non-digit
            if ((cn > cs) && (pin < 32)){
                this->port->FIOMASK &= ~(1 << this->pin);

                // now check for modifiers:-
                // ! = invert pin
                // o = set pin to open drain
                // ^ = set pin to pull up
                // v = set pin to pull down
                // - = set pin to no pull up or down
                // @ = set pin to repeater mode
                for (;*cn;cn++) {
                    switch(*cn) {
                        case '!':
                            this->inverting = true;
                            break;
                        case 'o':
                            as_open_drain();
                            break;
                        case '^':
                            pull_up();
                            break;
                        case 'v':
                            pull_down();
                            break;
                        case '-':
                            pull_none();
                            break;
                        case '@':
                            as_repeater();
                            break;
                        case 'm': // pin is multiplexed
                            multiplexed_pin(cn);
                            break;
                        default:
                            // skip any whitespace following the pin index
                            if (!is_whitespace(*cn))
                                return this;
                    }
                }
                return this;
            }
        }
    }

    // from_string failed. TODO: some sort of error
    valid= false;
    port_number = 0;
    port = gpios[0];
    pin = 32;
    inverting = false;
    return this;
}

// Configure this pin as OD
Pin* Pin::as_open_drain(){
    if (!this->valid) return this;
    if( this->port_number == 0 ){ LPC_PINCON->PINMODE_OD0 |= (1<<this->pin); }
    if( this->port_number == 1 ){ LPC_PINCON->PINMODE_OD1 |= (1<<this->pin); }
    if( this->port_number == 2 ){ LPC_PINCON->PINMODE_OD2 |= (1<<this->pin); }
    if( this->port_number == 3 ){ LPC_PINCON->PINMODE_OD3 |= (1<<this->pin); }
    if( this->port_number == 4 ){ LPC_PINCON->PINMODE_OD4 |= (1<<this->pin); }
    pull_none(); // no pull up by default
    return this;
}


// Configure this pin as a repeater
Pin* Pin::as_repeater(){
    if (!this->valid) return this;
    // Set the two bits for this pin as 01
    if( this->port_number == 0 && this->pin < 16  ){ LPC_PINCON->PINMODE0 |= (1<<( this->pin*2)); LPC_PINCON->PINMODE0 &= ~(2<<( this->pin    *2)); }
    if( this->port_number == 0 && this->pin >= 16 ){ LPC_PINCON->PINMODE1 |= (1<<( this->pin*2)); LPC_PINCON->PINMODE1 &= ~(2<<((this->pin-16)*2)); }
    if( this->port_number == 1 && this->pin < 16  ){ LPC_PINCON->PINMODE2 |= (1<<( this->pin*2)); LPC_PINCON->PINMODE2 &= ~(2<<( this->pin    *2)); }
    if( this->port_number == 1 && this->pin >= 16 ){ LPC_PINCON->PINMODE3 |= (1<<( this->pin*2)); LPC_PINCON->PINMODE3 &= ~(2<<((this->pin-16)*2)); }
    if( this->port_number == 2 && this->pin < 16  ){ LPC_PINCON->PINMODE4 |= (1<<( this->pin*2)); LPC_PINCON->PINMODE4 &= ~(2<<( this->pin    *2)); }
    if( this->port_number == 3 && this->pin >= 16 ){ LPC_PINCON->PINMODE7 |= (1<<( this->pin*2)); LPC_PINCON->PINMODE7 &= ~(2<<((this->pin-16)*2)); }
    if( this->port_number == 4 && this->pin >= 16 ){ LPC_PINCON->PINMODE9 |= (1<<( this->pin*2)); LPC_PINCON->PINMODE9 &= ~(2<<((this->pin-16)*2)); }
    return this;
}

// Configure this pin as no pullup or pulldown
Pin* Pin::pull_none(){
	if (!this->valid) return this;
	// Set the two bits for this pin as 10
	if( this->port_number == 0 && this->pin < 16  ){ LPC_PINCON->PINMODE0 |= (2<<( this->pin*2)); LPC_PINCON->PINMODE0 &= ~(1<<( this->pin    *2)); }
	if( this->port_number == 0 && this->pin >= 16 ){ LPC_PINCON->PINMODE1 |= (2<<( this->pin*2)); LPC_PINCON->PINMODE1 &= ~(1<<((this->pin-16)*2)); }
	if( this->port_number == 1 && this->pin < 16  ){ LPC_PINCON->PINMODE2 |= (2<<( this->pin*2)); LPC_PINCON->PINMODE2 &= ~(1<<( this->pin    *2)); }
	if( this->port_number == 1 && this->pin >= 16 ){ LPC_PINCON->PINMODE3 |= (2<<( this->pin*2)); LPC_PINCON->PINMODE3 &= ~(1<<((this->pin-16)*2)); }
	if( this->port_number == 2 && this->pin < 16  ){ LPC_PINCON->PINMODE4 |= (2<<( this->pin*2)); LPC_PINCON->PINMODE4 &= ~(1<<( this->pin    *2)); }
	if( this->port_number == 3 && this->pin >= 16 ){ LPC_PINCON->PINMODE7 |= (2<<( this->pin*2)); LPC_PINCON->PINMODE7 &= ~(1<<((this->pin-16)*2)); }
	if( this->port_number == 4 && this->pin >= 16 ){ LPC_PINCON->PINMODE9 |= (2<<( this->pin*2)); LPC_PINCON->PINMODE9 &= ~(1<<((this->pin-16)*2)); }
	return this;
}

// Configure this pin as a pullup
Pin* Pin::pull_up(){
    if (!this->valid) return this;
    // Set the two bits for this pin as 00
    if( this->port_number == 0 && this->pin < 16  ){ LPC_PINCON->PINMODE0 &= ~(3<<( this->pin    *2)); }
    if( this->port_number == 0 && this->pin >= 16 ){ LPC_PINCON->PINMODE1 &= ~(3<<((this->pin-16)*2)); }
    if( this->port_number == 1 && this->pin < 16  ){ LPC_PINCON->PINMODE2 &= ~(3<<( this->pin    *2)); }
    if( this->port_number == 1 && this->pin >= 16 ){ LPC_PINCON->PINMODE3 &= ~(3<<((this->pin-16)*2)); }
    if( this->port_number == 2 && this->pin < 16  ){ LPC_PINCON->PINMODE4 &= ~(3<<( this->pin    *2)); }
    if( this->port_number == 3 && this->pin >= 16 ){ LPC_PINCON->PINMODE7 &= ~(3<<((this->pin-16)*2)); }
    if( this->port_number == 4 && this->pin >= 16 ){ LPC_PINCON->PINMODE9 &= ~(3<<((this->pin-16)*2)); }
    return this;
}

// Configure this pin as a pulldown
Pin* Pin::pull_down(){
    if (!this->valid) return this;
    // Set the two bits for this pin as 11
    if( this->port_number == 0 && this->pin < 16  ){ LPC_PINCON->PINMODE0 |= (3<<( this->pin    *2)); }
    if( this->port_number == 0 && this->pin >= 16 ){ LPC_PINCON->PINMODE1 |= (3<<((this->pin-16)*2)); }
    if( this->port_number == 1 && this->pin < 16  ){ LPC_PINCON->PINMODE2 |= (3<<( this->pin    *2)); }
    if( this->port_number == 1 && this->pin >= 16 ){ LPC_PINCON->PINMODE3 |= (3<<((this->pin-16)*2)); }
    if( this->port_number == 2 && this->pin < 16  ){ LPC_PINCON->PINMODE4 |= (3<<( this->pin    *2)); }
    if( this->port_number == 3 && this->pin >= 16 ){ LPC_PINCON->PINMODE7 |= (3<<((this->pin-16)*2)); }
    if( this->port_number == 4 && this->pin >= 16 ){ LPC_PINCON->PINMODE9 |= (3<<((this->pin-16)*2)); }
    return this;
}

// If available on this pin, return mbed hardware pwm class for this pin
mbed::PwmOut* Pin::hardware_pwm()
{
    if (port_number == 1)
    {
        if (pin == 18) { return new mbed::PwmOut(P1_18); }
        if (pin == 20) { return new mbed::PwmOut(P1_20); }
        if (pin == 21) { return new mbed::PwmOut(P1_21); }
        if (pin == 23) { return new mbed::PwmOut(P1_23); }
        if (pin == 24) { return new mbed::PwmOut(P1_24); }
        if (pin == 26) { return new mbed::PwmOut(P1_26); }
    }
    else if (port_number == 2)
    {
        if (pin == 0) { return new mbed::PwmOut(P2_0); }
        if (pin == 1) { return new mbed::PwmOut(P2_1); }
        if (pin == 2) { return new mbed::PwmOut(P2_2); }
        if (pin == 3) { return new mbed::PwmOut(P2_3); }
        if (pin == 4) { return new mbed::PwmOut(P2_4); }
        if (pin == 5) { return new mbed::PwmOut(P2_5); }
    }
    else if (port_number == 3)
    {
        if (pin == 25) { return new mbed::PwmOut(P3_25); }
        if (pin == 26) { return new mbed::PwmOut(P3_26); }
    }
    return nullptr;
}

mbed::InterruptIn* Pin::interrupt_pin()
{
    if(!this->valid) return nullptr;

    // set as input
    as_input();

    if (port_number == 0 || port_number == 2) {
        PinName pinname = port_pin((PortName)port_number, pin);
        return new mbed::InterruptIn(pinname);

    }else{
        this->valid= false;
        return nullptr;
    }
}

/*
 * If a pin is multiplexed we expect cp to point to the 'm' modifier.
 * after that we expect a dot, then exactly two integers with a dot inbetween.
 * 1st integer is the identifier of the multiplexer, then the dot, then the
 * 2nd integer which is the index within the multiplexer.
 *
 * Example:
 *
 *     2.2m.1.7 (Port 2, Pin2, multiplexed, mpx type/id 1, mpx line 7
 * or
 *     2.2om.3.7 (Port 2, Pin2, open drain, multiplexed, mpx type/id 3, mpx line 7
 */
void Pin::multiplexed_pin(char const* cp)
{
    unsigned mpx_id, mpx_idx;
    int err = 0;
    mpx_map_t* mm;

    for ( ; ; ) // poor man's exception handling
    {
        if (! PublicData::get_value(multiplexer_checksum, &mm )) // get ptr to mpx map
            { err = 0x01; break; }  // cannot get public data

        int res = sscanf(cp, "m.%d.%d", &mpx_id, &mpx_idx); // cost: 32 bytes in -O 0

        if (res != 2 ||                                 // invalid number of values
            (mpx_id < 1 || mpx_id > mm->size()))        // id out of range
            { err = 0x02; break; }  // parse error

        char key[4]; // to_string and stoi are not available
        snprintf(key, 4, "m%d", mpx_id); // key are m1, m2, m3,...

        /*
         * ok, now we can assign a proper multiplexer and index to the pin
         */
        mpx_map_t::iterator it;
        it = mm->find(key); // find corresponding multiplexer in map

        if (it == mm->end())
            { err = 0x03; break; }  // mpx not found

        unsigned num_chan = (*it).second->get_channels();
        if (mpx_idx >= num_chan)
            { err = 0x04; break; }  // channel index out of range

        mpx = (*it).second.get();   // assign the multiplexer
        idx = mpx_idx;              // assign the index within the mpx

        // enable multiplexer channel
        mpx->set_active((Multiplexer::active_channel_t)(0x01 << idx));

        // chris debug remove @todo
        THEKERNEL->streams->printf("assigned multiplexer m%d.%d (%p) to pin P%d_%d\r\n", mpx_id, mpx_idx, mpx, port_number, pin);

        multiplexed = true;
        break;
    }
    if (err) {
        valid = false; // valid was set true in from_string(...)
        THEKERNEL->streams->printf("ERROR:unable to assign mpx %s to pin, error code %d\r\n", cp, err);
    }
}

/*
 * special getter for the multiplexed case
 * measured with scope: approx. 14usec until pin is read, mpx index 7
 */
bool Pin::get_multiplexed(void)
{
    mpx->set_index(idx);    // store new index in multiplexer
    mpx->apply_index();     // make multiplexed data present at (input) pin

    return inverting ^ (( port->FIOPIN >> pin ) & 1);
}

/*
 * special setter for the multiplexed case
 */
void Pin::set_multiplexed(bool value)
{
    mpx->set_index(idx);    // store new index in multiplexer

    if ( this->inverting ^ value )
        mpx->set_data(true);
    else
        mpx->set_data(false);

    mpx->apply_index();     // make multiplexed data present at (multiplexer) output pin
}


uint8_t Pin::get_mpx_index(void)
{
    return idx;
}


//void Pin::set_mpx_index(uint8_t i)
//{
//    idx = i;
//}


// @todo remove
//void Pin::set_mpx_channel(void)
//{
//    mpx->set_index(idx);    // set multiplexer index to this pin's index
//    mpx->apply_index();     // apply this index in hardware
//}


bool Pin::is_multiplexed(void)
{
    return multiplexed;
}


//unsigned Pin::get_mpx_num_chan(void)
//{
//    return mpx->get_channels();
//}


uint16_t Pin::get_adc_ch(void)
{
    return adc_ch;
}


void Pin::set_adc_ch(uint16_t ch)
{
    adc_ch = ch;
}


Multiplexer* Pin::get_mpx_ptr(void)
{
    return mpx;
}

