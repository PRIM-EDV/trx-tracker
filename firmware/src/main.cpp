#include <modm/processing.hpp>
#include <modm/processing/protothread.hpp>

#include "board/board.hpp"
#include "src/threads/lora.hpp"
#include "src/threads/tusb.hpp"
#include "src/threads/gps.hpp"


using namespace Board;

namespace Board::lora {
    LoraThread<Spi, Nss, D0, RxEn, TxEn> thread;
}

namespace Board::usb {
    TusbThread thread;
}

namespace Board::gps {
    GPSThread<Uart> thread;
}

int main()
{
    Board::initialize();

    shared::trackerId = 7;

    GpioOutputA1::setOutput(); 
    GpioOutputA1::set();

    // Timer2::connect<GpioOutputA1::Ch2>();
    // Timer2::connect<GpioOutputA5::Ch1>();
    // Timer2::connect<GpioOutputB10::Ch3>();



	// Timer2::enable();
	// Timer2::setMode(Timer2::Mode::UpCounter);

	// // 80 MHz / 800 / 2^16 ~ 1.5 Hz
	// // Timer2::setPrescaler(320);
	// Timer2::setOverflow(65535);


	// Timer2::configureOutputChannel<GpioOutputA1::Ch2>(Timer2::OutputCompareMode::Pwm, 500);
	// Timer2::configureOutputChannel<GpioOutputA5::Ch1>(Timer2::OutputCompareMode::Pwm, 450);
	// Timer2::configureOutputChannel<GpioOutputB10::Ch3>(Timer2::OutputCompareMode::Pwm, 350);


	// Timer2::applyAndReset();

	// Timer2::start();

    delay(5000ms); // Wait for BT-device

    
    // bluetooth::ioStream << "AT+NAMETRX-" << shared::trackerId << "\r\n";

    lora::thread.initialize();
    usb::thread.initialize();
    gps::thread.initialize();

    fiber::Scheduler::run();
}