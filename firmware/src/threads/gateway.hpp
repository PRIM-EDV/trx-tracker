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

#include "board/board.hpp"
#include "protocol/trx.pb.hpp"

#include "lib/buffer/message_buffer.hpp"
#include "lib/cobs/cobs.hpp"

using namespace modm;

template <typename Modem, typename Battery>
class GatewayThread : public modm::pt::Protothread, protected modm::NestedResumable<2>
{
public:
  GatewayThread(Modem &modem, Battery &battery) : modem(modem), battery(battery) {};

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
    bytes_decoded = cobs_decode((uint8_t *)message.data, message.size, decoded_buffer);
    stream = pb_istream_from_buffer(decoded_buffer, bytes_decoded);

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
      // RF_CALL(modem1.setMapEntity(trxMessage.message.request.request.setEntity.entity));
      break;
    default:
      break;
    }

    RF_END();
  }

private:
  MessageBuffer<128> message;
  Modem &modem;
  Battery &battery;

  uint8_t decoded_buffer[128];
};

#endif // GATEWAY_THREAD_HPP
