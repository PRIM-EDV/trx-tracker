/*
 * Copyright (c) 2023, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/processing/fiber.hpp>

class Thread : public modm::Fiber< 2048 >
{
public:
	Thread(modm::fiber::Start start=modm::fiber::Start::Now)
	:	Fiber([this](modm::fiber::stop_token stoken)
			  { while(not stoken.stop_requested() and update()) modm::this_fiber::yield(); }, start)
	{}

	void restart() { this->start(); }
	void stop() { this->request_stop(); }
	// isRunning() is implemented in fiber::Task

	// The run() function name was never enforced by the Protothread interface
	virtual bool run() { return false; };
	// Instead update() was often chosen to align it more with other parts of
	// modm that use an update() function to update their state periodically.
	// Therefore we cover both here to not have to change too much code.
	virtual bool update() { return run(); };
};