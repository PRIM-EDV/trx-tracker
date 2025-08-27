#include "message_buffer.hpp"
/* Copyright (c) 2023, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------

template <uint8_t N>
MessageBuffer<N>::MessageBuffer()
{
}

template <uint8_t N>
inline void MessageBuffer<N>::clear()
{
    memset(data, 0, N);
    size = 0;
}

template <uint8_t N>
inline MessageBuffer<N>&
MessageBuffer<N>::operator+=(const char c)
{
    if(size < N) {
        data[size] = c;
        size++;
    }

    return *this;
}

template <uint8_t N>
MessageBuffer<N>&
MessageBuffer<N>::operator+=(const char *str)
{
    std::size_t len = std::min(std::strlen(str), (size_t) (N - size));
    std::strncpy(data[size], str, len);

    size += len;

    return *this;
}

template <uint8_t N>
uint8_t
MessageBuffer<N>::maxSize()
{
    return N;
}