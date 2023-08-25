/*
 * Copyright (c) 2023, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------
#ifndef E32_X00MX0S_HPP
#define E32_X00MX0S_HPP

#include "../lora/semtec/sx127x/sx127x.hpp"

namespace modm
{

template <typename SpiMaster, typename Cs, typename RxEn, typename TxEn>
class E32x00Mx0s : public SX127x<SpiMaster, Cs>
{
public:
	E32x00Mx0s();

    ResumableResult<void>
    setOperationMode(sx127x::Mode mode);
};

}

#include "e32-x00mx0s_impl.hpp"

#endif
