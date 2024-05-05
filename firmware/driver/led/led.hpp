// ----------------------------------------------------------------------------
/* Copyright (c) 2024, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------

#ifndef LED_HPP
#define LED_HPP

#include <stdint.h>

// #include <modm/architecture/interface/spi_device.hpp>

// #include "sx127x_definitions.hpp"

namespace modm
{

template <typename RedCh, typename GreenCh, typename BlueCh, typename Timer>
class Led : protected NestedResumable<2>
{
public:
    Led();

    void 
    initialize();

    void
    setColor(uint8_t red, uint8_t green, uint8_t blue);

private:
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    uint16_t
    getGammaCorrectedBrightness(uint8_t brightness, uint16_t limit);
};

}

#include "led_impl.hpp"

#endif
