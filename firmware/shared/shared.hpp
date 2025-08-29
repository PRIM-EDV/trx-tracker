#ifndef SHARED_HPP
#define SHARED_HPP

#include <pb_encode.h>
#include <pb_decode.h>

namespace shared
{
    uint8_t trackerId = 1;

    char uuid_buffer[38];
    uint8_t message_buffer[128];
    uint8_t encoding_buffer[128];
    uint8_t decoded_buffer[128];

    double latitude;
    double longitude;

    int px;
    int py;

    int batteryLevel = 0;

    char fwVersion[8] = "0.1.0";

    bool encode_string(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
    {
        const char *str = (const char *)(*arg);

        if (!pb_encode_tag_for_field(stream, field))
            return false;

        return pb_encode_string(stream, (uint8_t *)str, strlen(str));
    }

    bool decode_string(pb_istream_t *stream, const pb_field_t *field, void **arg)
    {
        const char *str = (const char *)(*arg);

        if (stream->bytes_left > sizeof(str) - 1)
            return false;

        if (!pb_read(stream, (uint8_t *)str, stream->bytes_left))
            return false;

        return true;
    }
}

#endif