// ----------------------------------------------------------------------------
/* Copyright (c) 2023, Lucas Moesch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------

#ifndef RLD_NODE_BOARD_HPP
#define RLD_NODE_BOARD_HPP

#include <modm/io.hpp>
#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>

using namespace modm::platform;

namespace Board
{

using namespace modm::literals;

struct SystemClock
{
	// core and bus frequencys
	static constexpr uint32_t Frequency = 48_MHz;
    static constexpr uint32_t Hsi = 8_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb = Frequency;

	// Usart
	static constexpr uint32_t Usart1 = Apb;
	static constexpr uint32_t Usart2 = Apb;

	// Spi
	static constexpr int Spi1 = Frequency;
	static constexpr int Spi2 = Frequency;

    // Usb
    static constexpr uint32_t Usb = Ahb;

	static bool inline
	enable()
	{
		Rcc::enableInternalClock(); // 8 MHz
        const Rcc::PllFactors pllFactors{
			.pllMul = 12,
			.pllPrediv = 2,
            .usbPrediv = Rcc::UsbPrescaler::Div1
		};
        Rcc::enablePll(Rcc::PllSource::InternalClock, pllFactors);
		// set flash latency for 48MHz
		Rcc::setFlashLatency<Frequency>();
		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApbPrescaler(Rcc::ApbPrescaler::Div1);
		// update frequencies for busy-wait delay functions
        Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};

namespace lora2 {
	using Rst = GpioOutputA8;
	using D0 = GpioInputB11;

	using Nss = GpioOutputB12;
	using Sck = GpioOutputB13;
	using Miso = GpioInputB14;
	using Mosi = GpioOutputB15;

    using RxEn = GpioOutputA9;
    using TxEn = GpioOutputA10;

	using Spi = SpiMaster2;
}

namespace gps
{
	using Rx = GpioInputA10;
	using Tx = GpioOutputA9;

	using Uart = Usart1;
}

namespace bluetooth
{
	using Rx = GpioInputA3;
	using Tx = GpioOutputA2;

	using Uart = Usart2;
}

namespace usb
{
    using Dm = GpioA11;
    using Dp = GpioA12;

    using Device = UsbFs;
}

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	lora2::Nss::setOutput();
	lora2::Rst::setOutput();

	lora2::Nss::set();
	lora2::Rst::set();

	lora2::Spi::connect<lora2::Sck::Sck, lora2::Mosi::Mosi, lora2::Miso::Miso>();
	lora2::Spi::initialize<SystemClock, 6000000ul>();

	gps::Uart::connect<gps::Tx::Tx, gps::Rx::Rx>();
	gps::Uart::initialize<SystemClock, 9600_Bd>();

    bluetooth::Uart::connect<bluetooth::Tx::Tx, bluetooth::Rx::Rx>();
    bluetooth::Uart::initialize<SystemClock, 9600_Bd>();

    usb::Device::initialize<SystemClock>(3);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp>();
}

}

#include "board_impl.hpp"
#endif