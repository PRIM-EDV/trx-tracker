#include <tusb.h>

#include <modm/processing.hpp>
#include <modm/processing/protothread.hpp>

#include "board/board.hpp"
// #include "src/threads/lora.hpp"

using namespace Board;



int main()
{
    Board::initialize();
    tusb_init();

    while (true)
    {
        tud_task();
        usb::usb_stream << "Hello World from USB: " << modm::endl;
    }
}