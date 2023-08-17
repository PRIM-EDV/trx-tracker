#include <nanopb/pb_encode.h>
#include <nanopb/pb_decode.h>
#include <modm/processing.hpp>
#include <modm/processing/protothread.hpp>

#include "board/board.hpp"
#include "lib/cobs/cobs.hpp"
// #include "lib/uuid/uuid.h"
#include "protocol/protocol.pb.hpp"
#include "src/threads/lora.hpp"

using namespace Board;



int main()
{
    Board::initialize();

    while (true)
    {

    }
}