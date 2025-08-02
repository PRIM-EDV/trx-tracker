/*
 * Copyright (c) 2025, Lucas Mösch
 * All Rights Reserved.
 */

#ifndef BATTERY_THREAD_HPP
#define BATTERY_THREAD_HPP

#include <modm/processing.hpp>
#include <modm/processing/protothread.hpp>

using namespace modm;

class BatteryThread:  public modm::pt::Protothread, private modm::NestedResumable<2>
{
public:

};

#endif // BATTERY_THREAD_HPP
