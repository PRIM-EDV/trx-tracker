#ifndef LED_HPP
#define LED_HPP

// #include <modm/architecture/interface/spi_device.hpp>

// #include "sx127x_definitions.hpp"

namespace modm
{

template <typename SpiMaster, typename Cs>
class Led : public sx127x, public SpiDevice<SpiMaster>, protected NestedResumable<2>
{
public:
	Led();
}