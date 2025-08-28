/*
 * Copyright (c) 2025, Lucas Mösch
 * All Rights Reserved.
 */

#ifndef GATEWAY_THREAD_HPP
#define GATEWAY_THREAD_HPP

#include <modm/platform/adc/adc.hpp>
#include <modm/processing.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/processing/timer.hpp>

#include <pb_decode.h>
#include <pb_encode.h>

#include "board/board.hpp"
#include "protocol/trx.pb.hpp"

#include "lib/buffer/message_buffer.hpp"
#include "lib/cobs/cobs.hpp"
#include "lib/uuid/uuid.h"
#include "lib/thread/thread.hpp"

using namespace modm;

template <typename Modem>
class GatewayThread : public Thread, protected modm::NestedResumable<2>
{
public:
  GatewayThread(Modem &modem) : modem(modem) {};

  void
  initialize() {

  };

  bool
  run()
  {
    PT_BEGIN();

    uint8_t c;

    while (1)
    {
      PT_WAIT_UNTIL(Board::bluetooth::Uart::read(c) || Board::bluetooth::Uart::hasError());

      if (Board::bluetooth::Uart::hasError())
      {
        Board::bluetooth::Uart::clearError();
        message.clear();
      }
      else
      {
        do
        {
          if (message.size > 1 && c == '\0')
          {
            PT_CALL(handleMessage());
            message.clear();
          }
          else if (c == '\0')
          {
            message.clear();
          }
          else if (message.size > 127)
          {
            message.clear();
          }
          else
          {
            message += (char)c;
          }
        } while (Board::bluetooth::Uart::read(c));
      }
    };

    PT_END();
  };

  ResumableResult<void>
  handleMessage()
  {
    uint8_t bytes_decoded;
    pb_istream_t stream;
    TrxMessage trxMessage = TrxMessage_init_zero;

    RF_BEGIN();
    bytes_decoded = cobs_decode((uint8_t *)message.data, message.size, shared::decoded_buffer);
    stream = pb_istream_from_buffer(shared::decoded_buffer, bytes_decoded);

    if (pb_decode(&stream, TrxMessage_fields, &trxMessage))
    {
      switch (trxMessage.which_message)
      {
      case TrxMessage_request_tag:
        RF_CALL(handleRequest(trxMessage));
      case TrxMessage_response_tag:
      case TrxMessage_error_tag:
        break;
      default:
        break;
      }
    }

    RF_END();
  };

  ResumableResult<void>
  handleRequest(TrxMessage &trxMessage)
  {
    RF_BEGIN();
    switch (trxMessage.message.request.which_request)
    {
    case Request_getBatteryStatus_tag:
      RF_CALL(respondGetBatteryStatus());
      break;
    default:
      break;
    }

    RF_END();
  }

private:
  MessageBuffer<128> message;
  Modem &modem;

  ResumableResult<void>
  respondGetBatteryStatus()
  {
    RF_BEGIN();

    // generate UUID
    uuid::v4(shared::uuid_buffer);

    // generate protobuf message
    TrxMessage trx_message = TrxMessage_init_zero;
    trx_message.id.arg = shared::uuid_buffer;
    trx_message.id.funcs.encode = &(shared::encode_string);

    trx_message.which_message = TrxMessage_response_tag;
    trx_message.message.response.which_response = Response_getBatteryStatus_tag;
    trx_message.message.response.response.getBatteryStatus = GetBatteryStatus_Response_init_default;
    trx_message.message.response.response.getBatteryStatus.battery_level = shared::batteryLevel;

    pb_ostream_t pb_ostream = pb_ostream_from_buffer(shared::message_buffer, sizeof(shared::message_buffer));
    pb_encode(&pb_ostream, TrxMessage_fields, &trx_message);
    uint8_t bytes_encoded = cobs_encode(shared::message_buffer, pb_ostream.bytes_written, shared::encoding_buffer);

    // Board::bluetooth::Uart::write(shared::encoding_buffer, bytes_encoded);
    // Board::bluetooth::Uart::write('\0');

    RF_END();
  }
};

#endif // GATEWAY_THREAD_HPP
