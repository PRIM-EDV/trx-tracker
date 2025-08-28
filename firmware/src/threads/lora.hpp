/*
 * Copyright (c) 2022, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------

#ifndef LORA_THREAD_HPP
#define LORA_THREAD_HPP

#include <modm/processing.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/processing/timer.hpp>

#include <pb_encode.h>

#include "board/board.hpp"
#include "driver/cdebyte/e32-x00mx0s.hpp"
#include "lib/random/random.hpp"
#include "lib/thread/thread.hpp"
#include "shared/shared.hpp"
#include "lib/cobs/cobs.hpp"
#include "lib/uuid/uuid.h"

#include "protocol/trx.pb.hpp"

using namespace modm;

template <typename SpiMaster, typename Cs, typename D0, typename RxEn, typename TxEn>
class LoraThread : public Thread, protected modm::NestedResumable<2>
{
public:
    void
    initialize()
    {
        RF_CALL_BLOCKING(modem.setLora());
        RF_CALL_BLOCKING(modem.setCarrierFreq(0xd9, 0x5d, 0x9a)); // 869.465 MHz - FSTEP = 61.035 Hz
        RF_CALL_BLOCKING(modem.setHighFrequencyMode());
        RF_CALL_BLOCKING(modem.setLnaBoostHf());
        RF_CALL_BLOCKING(modem.setPaBoost());
        RF_CALL_BLOCKING(modem.setAgcAutoOn());
        RF_CALL_BLOCKING(modem.setExplicitHeaderMode());
        RF_CALL_BLOCKING(modem.setSpreadingFactor(sx127x::SpreadingFactor::SF12));
        RF_CALL_BLOCKING(modem.setBandwidth(sx127x::SignalBandwidth::Fr250kHz));
        // RF_CALL_BLOCKING(modem.setCodingRate(sx127x::ErrorCodingRate::Cr4_5));
        RF_CALL_BLOCKING(modem.enablePayloadCRC());
        RF_CALL_BLOCKING(modem.setPayloadLength(4));
        RF_CALL_BLOCKING(modem.setDio0Mapping(0));

        // // Set output power to 10 dBm (boost mode)
        RF_CALL_BLOCKING(modem.setOutputPower(0x0f));
        RF_CALL_BLOCKING(modem.setOperationMode(sx127x::Mode::RecvCont));

        timeout.restart(10s);
    };

    bool
    run()
    {
        PT_BEGIN();

        data[0] = 0;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;

        while (1)
        {
            PT_WAIT_UNTIL(timeout.isExpired() || messageAvailable());

            if (messageAvailable())
            {
                PT_CALL(receiveMessage(data));
                setEntity(data);
                // Board::bluetooth::ioStream << stack_usage() << endl;
                // Board::usb::ioStream << data[0] << ":" << data[1]<< ":" << data[2]<< ":" << data[3] << "|" << data[4] << "|" << data[5] << endl;
            }

            if (timeout.isExpired())
            {
                PT_CALL(sendMessage());
                data[0] = 0;
                setEntity(data);
                // Board::bluetooth::ioStream << data[0] << ":" << data[1] << ":" << data[2] << ":" << data[3] << endl;
                // Board::usb::ioStream << data[0] << ":" << data[1]<< ":" << data[2]<< ":" << data[3] << endl;
                timeout.restart(10s);
            }
        };

        PT_END();
    };

    ResumableResult<void>
    csma()
    {
        RF_BEGIN();

        while (true)
        {
            RF_CALL(modem.setOperationMode(sx127x::Mode::ChnActvDetect));
            RF_WAIT_UNTIL(cadDone());

            RF_CALL(modem.read(sx127x::Address::IrqFlags, status, 1));
            RF_CALL(modem.write(sx127x::Address::IrqFlags, 0xff));

            if (!(status[0] & (uint8_t)sx127x::RegIrqFlags::CadDetected))
            {
                RF_RETURN();
            }
            else
            {
                uint8_t rand = random::lfsr8();
                csmaTimeout.restart(20ms * rand);
                RF_WAIT_UNTIL(csmaTimeout.isExpired());
            }
        }

        RF_END();
    };

    ResumableResult<void>
    receiveMessage(uint8_t *buffer)
    {
        RF_BEGIN();

        RF_CALL(modem.read(sx127x::Address::IrqFlags, status, 1));
        if (!(status[0] & (uint8_t)sx127x::RegIrqFlags::PayloadCrcError))
        {
            RF_CALL(modem.getPayload(buffer, 4));
        }
        RF_CALL(modem.write(sx127x::Address::IrqFlags, 0xff));

        RF_END();
    };

    ResumableResult<uint8_t>
    sendMessage()
    {
        RF_BEGIN();

        buildPacket();

        RF_CALL(modem.setPayloadLength(4));
        RF_CALL(modem.sendPacket(data, 4));
        timeout2.restart(1s);
        RF_WAIT_UNTIL(messageSent() || timeout2.isExpired());
        RF_CALL(modem.write(sx127x::Address::IrqFlags, 0xff));
        RF_CALL(modem.setOperationMode(sx127x::Mode::RecvCont));

        RF_END_RETURN(0);
    };

private:
    uint8_t data[8];
    uint8_t status[1];

    Timeout timeout;
    Timeout timeout2;
    Timeout csmaTimeout;
    E32x00Mx0s<SpiMaster, Cs, RxEn, TxEn> modem;

    void
    buildPacket()
    {
        uint16_t px = (shared::longitude != 0) ? shared::px : 0;
        uint16_t py = (shared::latitude != 0) ? shared::py : 0;

        data[0] = shared::trackerId;
        data[1] = (px >> 6) & 0x0F;
        data[2] = (px << 2) | ((py >> 8) & 0x03);
        data[3] = (py) & 0xFF;
    };

    bool
    cadDone()
    {
        return D0::read();
    }

    bool
    messageAvailable()
    {
        return D0::read();
    }

    bool
    messageSent()
    {
        return D0::read();
    }

    void setEntity(uint8_t *data)
    {
        // generate UUID
        uuid::v4(shared::uuid_buffer);

        // generate protobuf message
        Entity entity = Entity_init_default;
        entity.id = data[0];
        entity.has_position = true;
        entity.position = Position_init_default;
        entity.position.x = ((data[1] & 0x0f) << 6) | ((data[2] & 0xfc) >> 2);
        entity.position.y = ((data[2] & 0x03) << 8) | data[3];
        entity.type = Type_SQUAD;
        entity.size = 1;

        TrxMessage trx_message = TrxMessage_init_zero;
        trx_message.id.arg = shared::uuid_buffer;
        trx_message.id.funcs.encode = &(shared::encode_string);

        trx_message.which_message = TrxMessage_request_tag;
        trx_message.message.request.which_request = Request_setEntity_tag;
        trx_message.message.request.request.setEntity = SetEntity_Request_init_default;
        trx_message.message.request.request.setEntity.has_entity = true;
        trx_message.message.request.request.setEntity.entity = entity;

        pb_ostream_t pb_ostream = pb_ostream_from_buffer(shared::message_buffer, sizeof(shared::message_buffer));
        pb_encode(&pb_ostream, TrxMessage_fields, &trx_message);
        uint8_t bytes_encoded = cobs_encode(shared::message_buffer, pb_ostream.bytes_written, shared::encoding_buffer);

        Board::bluetooth::Uart::write(shared::encoding_buffer, bytes_encoded);
        Board::bluetooth::Uart::write('\0');
    }
};

#endif