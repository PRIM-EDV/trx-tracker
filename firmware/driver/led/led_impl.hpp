
    // Timer2::connect<GpioOutputA1::Ch2>();
    // Timer2::connect<GpioOutputA5::Ch1>();
    // Timer2::connect<GpioOutputB10::Ch3>();



	// Timer2::enable();
	// Timer2::setMode(Timer2::Mode::UpCounter);

	// // 48 MHz / 2 / 2^16 ~ 366 Hz
	// // Timer2::setPrescaler(2);
	// Timer2::setOverflow(65535);


	Timer2::configureOutputChannel<GpioOutputA1::Ch2>(Timer2::OutputCompareMode::Pwm, 500);
	// Timer2::configureOutputChannel<GpioOutputA5::Ch1>(Timer2::OutputCompareMode::Pwm, 450);
	// Timer2::configureOutputChannel<GpioOutputB10::Ch3>(Timer2::OutputCompareMode::Pwm, 350);


	// Timer2::applyAndReset();

	// Timer2::start();