/*
 * Copyright (c) 2023, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------
#ifndef GPS_THREAD_HPP
#define GPS_THREAD_HPP

#ifndef M_PI
    #define M_PI (double) 3.14159265358979323846
#endif

#include <modm/processing.hpp>
#include <modm/processing/protothread.hpp>

#include "driver/gps/beitian/bn280.hpp"
#include "shared/shared.hpp"

typedef struct {
    double latitude;
    double longitude;
} GeoCoordinates;

using namespace modm;
using namespace Beitian;

template <typename Uart>
class GPSThread : public modm::pt::Protothread, private modm::NestedResumable<2>
{
public:
    GeoCoordinates position;

    void
    initialize()
    {
        // setOrigin(52.382864, 11.818967);
        setOrigin(52.386332, 11.80533); // Set origin to current position
    };

    bool
    run()
    {
        PT_BEGIN();

        while (true) {
            PT_CALL(gps.update());

            setCartCoordinates();
        };

        PT_END();
    };

private:
	uint8_t data[8];
    double scaler = 1.0;

    GeoCoordinates origin;

    BN280<Uart> gps;

    void 
	setOrigin(double latitude, double longitude)
	{
		origin.latitude = latitude;
		origin.longitude = longitude;

		scaler = cos(latitude * M_PI / 180) * 111300;
	};

	void 
	setCartCoordinates()
	{
        double y = (origin.latitude - gps.latitude) * 111300;
        double x = (gps.longitude - origin.longitude) * scaler;

        shared::latitude = gps.latitude;
        shared::longitude = gps.longitude;
        shared::px = (uint16_t) x;
        shared::py = (uint16_t) y;
	}
};

#endif