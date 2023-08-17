// ----------------------------------------------------------------------------
/* Copyright (c) 2022, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------

#include "board.hpp"

namespace Board
{

namespace rpi
{
    modm::IODeviceWrapper<Uart, modm::IOBuffer::DiscardIfFull> rpiIODevice;
	modm::IOStream ioStream(rpiIODevice);
}

}