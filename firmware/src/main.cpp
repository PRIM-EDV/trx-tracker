#include <modm/processing.hpp>
#include <modm/processing/protothread.hpp>

#include "board/board.hpp"
#include "src/threads/lora.hpp"
#include "src/threads/tusb.hpp"
#include "src/threads/gps.hpp"


using namespace Board;

namespace Board::lora1 {
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

    shared::trackerId = 1;

    delay(1000ms); // Wait for BT-device
    bluetooth::ioStream << "AT+NAMETRX-" << shared::trackerId << "\r\n";

    lora1::thread.initialize();
    usb::thread.initialize();
    gps::thread.initialize();

    fiber::Scheduler::run();
}