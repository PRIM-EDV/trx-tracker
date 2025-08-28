// ----------------------------------------------------------------------------
/* Copyright (c) 2023, Lucas Moesch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------

#ifndef RLD_NODE_BOARD_HPP
#define RLD_NODE_BOARD_HPP

#include <modm/io.hpp>
#include <modm/platform.hpp>
#include <modm/platform/adc/adc.hpp>
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

		// Adc
		static constexpr uint32_t Adc1 = Apb;

		// Usart
		static constexpr uint32_t Usart1 = Apb;
		static constexpr uint32_t Usart2 = Apb;

		// Spi
		static constexpr int Spi1 = Frequency;
		static constexpr int Spi2 = Frequency;

		// Usb
		static constexpr uint32_t Usb = Ahb;

		static bool inline enable()
		{
			Rcc::enableInternalClock(); // 8 MHz
			const Rcc::PllFactors pllFactors{
					.pllMul = 12,
					.pllPrediv = 2,
					.usbPrediv = Rcc::UsbPrescaler::Div1};
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

	namespace battery
	{
		using VBatRef = GpioInputA6;
	}

	namespace led
	{
		using Red = GpioOutputA5;
		using Green = GpioOutputA1;
		using Blue = GpioOutputB10;

		using RedCh = GpioOutputA5::Ch1;
		using GreenCh = GpioOutputA1::Ch2;
		using BlueCh = GpioOutputB10::Ch3;
	}

	namespace lora
	{
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
		using Rx = GpioInputB7;
		using Tx = GpioOutputB6;

		using Uart = BufferedUart<UsartHal1, UartRxBuffer<8>, UartTxBuffer<8>>;
	}

	namespace bluetooth
	{
		using Rx = GpioInputA3;
		using Tx = GpioOutputA2;

		using Uart = BufferedUart<UsartHal2, UartRxBuffer<64>, UartTxBuffer<64>>;
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

		// Led
		led::Red::setOutput();
		led::Green::setOutput();
		led::Blue::setOutput();
		led::Red::set();
		led::Green::set();
		led::Blue::set();

		// Lora
		lora::Nss::setOutput();
		lora::Rst::setOutput();
		lora::RxEn::setOutput();
		lora::TxEn::setOutput();

		lora::RxEn::reset();
		lora::TxEn::reset();
		lora::Nss::set();
		lora::Rst::set();

		// Serial
		lora::Spi::connect<lora::Sck::Sck, lora::Mosi::Mosi, lora::Miso::Miso>();
		lora::Spi::initialize<SystemClock, 6000000ul>();

		gps::Uart::connect<gps::Tx::Tx, gps::Rx::Rx>();
		gps::Uart::initialize<SystemClock, 9600_Bd>();

		bluetooth::Uart::connect<bluetooth::Tx::Tx, bluetooth::Rx::Rx>();
		bluetooth::Uart::initialize<SystemClock, 9600_Bd>();

		usb::Device::initialize<SystemClock>(3);
		usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp>();

		// Battery
		modm::platform::Adc::connect<battery::VBatRef::In6>();
		modm::platform::Adc::initialize<Board::SystemClock, modm::platform::Adc::ClockMode::Synchronous, 12_MHz>();
	}

}

#include "board_impl.hpp"
#endif