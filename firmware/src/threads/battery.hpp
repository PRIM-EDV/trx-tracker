/*
 * Copyright (c) 2025, Lucas Mösch
 * All Rights Reserved.
 */

#ifndef BATTERY_THREAD_HPP
#define BATTERY_THREAD_HPP

#include <modm/platform/adc/adc.hpp>
#include <modm/processing.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/processing/timer.hpp>

#include "board/board.hpp"

using namespace modm;

class BatteryThread:  public modm::pt::Protothread, protected modm::NestedResumable<2>
{
public:

   void
    initialize()
    {
      modm::platform::Adc::setPinChannel<Board::battery::VBatRef::In6>();
      modm::platform::Adc::setResolution(modm::platform::Adc::Resolution::Bits12);
      modm::platform::Adc::setRightAdjustResult();
      modm::platform::Adc::setSampleTime(modm::platform::Adc::SampleTime::Cycles239_5);

      timeout.restart(10s);
    };

    bool
    run()
    {
        PT_BEGIN();

        while (true) {
          PT_WAIT_UNTIL(timeout.isExpired());
          modm::platform::Adc::startConversion();
          PT_WAIT_UNTIL(modm::platform::Adc::isConversionFinished());

          timeout.restart(10s);
        };

        PT_END();
    };
  private:
    Timeout timeout;

    
};

#endif // BATTERY_THREAD_HPP
