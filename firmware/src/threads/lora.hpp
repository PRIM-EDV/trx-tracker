/*
 * Copyright (c) 2022, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------

#ifndef LORA_THREAD_HPP
#define LORA_THREAD_HPP

#include <modm/processing.hpp>
#include <modm/processing/protothread.hpp>

#include "board/board.hpp"
#include "driver/ai-thinker/sx127x.hpp"
#include "lib/cobs/cobs.hpp"
#include "lib/uuid/uuid.h"
#include "protocol/protocol.pb.hpp"

using namespace modm;

template <typename SpiMaster, typename Cs, typename D0>
class LoraThread : public modm::pt::Protothread, private modm::NestedResumable<2>
{
public:
    void
    initialize()
    {
        RF_CALL_BLOCKING(modem.setLora());
        RF_CALL_BLOCKING(modem.setCarrierFreq(0x6f, 0x00, 0x12));
        RF_CALL_BLOCKING(modem.setPaBoost());
        RF_CALL_BLOCKING(modem.setAgcAutoOn());
        RF_CALL_BLOCKING(modem.setExplicitHeaderMode());
        RF_CALL_BLOCKING(modem.setSpreadingFactor(sx127x::SpreadingFactor::SF9));
        RF_CALL_BLOCKING(modem.setBandwidth(sx127x::SignalBandwidth::Fr125kHz));
        RF_CALL_BLOCKING(modem.setCodingRate(sx127x::ErrorCodingRate::Cr4_5));
        RF_CALL_BLOCKING(modem.enablePayloadCRC());
        RF_CALL_BLOCKING(modem.setDio0Mapping(0));

        RF_CALL_BLOCKING(modem.setPayloadLength(4));

        // // Set output power to 10 dBm (boost mode)
        RF_CALL_BLOCKING(modem.setOutputPower(0x08));
        RF_CALL_BLOCKING(modem.setOperationMode(sx127x::Mode::RecvCont));
    };

    bool
    run()
    {
        PT_BEGIN();

        while (1) {
            // if(PT_WAIT_UNTIL(this->getMessage(this->data)))
            // {
            //     this->setRldInfo(this->data);
            // };
        };

        PT_END();
    };

    void
    setRldInfo(uint8_t* data)
    {
        char id[37];
        uuid::v4(id);

        // generate protobuf message
        RldInfo rld_info = RldInfo_init_zero;
        rld_info.id = data[0];
        rld_info.flags = data[1] >> 4;
        rld_info.px = ((data[1] & 0x0f) << 6) | ((data[2] & 0xfc) >> 2);
        rld_info.py = ((data[2] & 0x03) << 8) | data[3];

        RldMessage rld_message = RldMessage_init_zero;
        rld_message.id.arg = id;
        rld_message.message.request.request.set_rld_info.info = rld_info;

        pb_ostream_t pb_ostream = pb_ostream_from_buffer(message_bufer, sizeof(message_bufer));
        pb_encode(&pb_ostream, RldMessage_fields, &rld_message);
        cobs_encode(message_bufer, pb_ostream.bytes_written, encoding_buffer);

        Board::rpi::Uart::write(encoding_buffer, pb_ostream.bytes_written);
        Board::rpi::Uart::write('\0');
    };

    ResumableResult<uint8_t>
	getMessage(uint8_t* buffer)
    {
        RF_BEGIN();

        while(true) {
            RF_WAIT_UNTIL(D0::read());
            RF_CALL(modem.read(sx127x::Address::IrqFlags, status, 1));
            if(!(status[0] & (uint8_t) sx127x::RegIrqFlags::PayloadCrcError)) {
                RF_CALL(modem.getPayload(buffer, 4));
                RF_RETURN(4);
            }
        }

        RF_END_RETURN(0);
    };

private:
	uint8_t data[8];
    uint8_t status[1];
    uint8_t message_id = 0;
    uint8_t message_bufer[128];
    uint8_t encoding_buffer[128];

    SX127x<SpiMaster, Cs> modem;
};

#endif