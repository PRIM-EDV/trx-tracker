/*
 * Copyright (c) 2023, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------

#ifndef TSUB_THREAD_HPP
#define TSUB_THREAD_HPP

#include <tusb.h>

#include <modm/processing.hpp>
#include <modm/processing/protothread.hpp>

using namespace modm;

class TusbThread : public modm::pt::Protothread
{
public:
    void
    initialize()
    {
        tusb_init();
    };

    bool
    run()
    {
        PT_BEGIN();

        while (1) {
            tud_task();
            PT_YIELD();
        };

        PT_END();
    };
};

#endif