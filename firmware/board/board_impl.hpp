// ----------------------------------------------------------------------------
/* Copyright (c) 2023, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------

#include "board.hpp"

namespace Board
{

namespace gps
{
    modm::IODeviceWrapper<Uart, modm::IOBuffer::DiscardIfFull> IODevice;
	modm::IOStream ioStream(IODevice);
}

namespace bluetooth
{
    modm::IODeviceWrapper<Uart, modm::IOBuffer::DiscardIfFull> IODevice;
	modm::IOStream ioStream(IODevice);
}

namespace usb 
{
    modm::IODeviceWrapper<UsbUart0, modm::IOBuffer::BlockIfFull> IODevice;
    modm::IOStream ioStream(IODevice);
}

}