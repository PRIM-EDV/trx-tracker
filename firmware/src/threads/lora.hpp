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

#include "board/board.hpp"
#include "driver/cdebyte/e32-x00mx0s.hpp"
#include "shared/shared.hpp"

using namespace modm;

template <typename SpiMaster, typename Cs, typename D0, typename RxEn, typename TxEn>
class LoraThread : public modm::pt::Protothread, private modm::NestedResumable<3>
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

        timeout.restart(5s);
    };

    bool
    run()
    {
        PT_BEGIN();

        while (1) {
            PT_WAIT_UNTIL(timeout.isExpired() || messageAvailable());

            if (messageAvailable()) {
                RF_CALL(receiveMessage(data));
                
                Board::bluetooth::ioStream << data[0] << ":" << data[1]<< ":" << data[2]<< ":" << data[3] << endl;	
                Board::usb::ioStream << data[0] << ":" << data[1]<< ":" << data[2]<< ":" << data[3] << endl;
            }
            
            if(timeout.isExpired()){
                RF_CALL(sendMessage());
                Board::bluetooth::ioStream << data[0] << ":" << data[1]<< ":" << data[2]<< ":" << data[3] << endl;
                Board::usb::ioStream << data[0] << ":" << data[1]<< ":" << data[2]<< ":" << data[3] << endl;
                timeout.restart(5s);
            } 
        };

        PT_END();
    };

    ResumableResult<void>
	receiveMessage(uint8_t* buffer)
    {
        RF_BEGIN();

        RF_CALL(modem.read(sx127x::Address::IrqFlags, status, 1));
        if(!(status[0] & (uint8_t) sx127x::RegIrqFlags::PayloadCrcError)) {
            RF_CALL(modem.getPayload(buffer, 4));
        }
        RF_CALL(modem.write(sx127x::Address::IrqFlags, 0xff));

        RF_END();
    };

    ResumableResult<uint8_t>
	sendMessage()
    {
        RF_BEGIN();
        RF_CALL(modem.setOperationMode(sx127x::Mode::Transmit));

        buildPacket();

        RF_CALL(modem.setPayloadLength(4));
		RF_CALL(modem.sendPacket(data, 4));

		PT_CALL(modem.setOperationMode(sx127x::Mode::RecvCont));

        RF_END_RETURN(0);
    };

private:
	uint8_t data[8];
    uint8_t status[1];

    ShortTimeout timeout;
    E32x00Mx0s<SpiMaster, Cs, RxEn, TxEn> modem;

    void
	buildPacket(){
        uint16_t px = (shared::longitude != 0) ? shared::px : 0;
        uint16_t py = (shared::latitude != 0) ? shared::py : 0;

		data[0] = shared::trackerId;
		data[1] = (px >> 6) & 0x0F;
		data[2] = (px << 2) | ((py >>8) & 0x03);
		data[3] = (py) & 0xFF;
	};

    bool
    messageAvailable() 
    {
        return D0::read();
    }
};

#endif