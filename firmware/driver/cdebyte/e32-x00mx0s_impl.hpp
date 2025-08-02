// ----------------------------------------------------------------------------
/* Copyright (c) 2023, Lucas Mösch
 * All Rights Reserved.
 */
// ----------------------------------------------------------------------------
#include "e32-x00mx0s.hpp"

namespace modm
{

template <typename SpiMaster, typename Cs, typename RxEn, typename TxEn>
E32x00Mx0s<SpiMaster, Cs, RxEn, TxEn>::E32x00Mx0s()
{

}

template <typename SpiMaster, typename Cs, typename RxEn, typename TxEn>
ResumableResult<void> 
E32x00Mx0s<SpiMaster, Cs, RxEn, TxEn>::setOperationMode(sx127x::Mode mode)
{
    RF_BEGIN();

    if (mode == sx127x::Mode::RecvCont || mode == sx127x::Mode::RecvSingle)
    {
      TxEn::reset(); delay(100ms);
      RxEn::set(); 
      RF_CALL(SX127x<SpiMaster, Cs>::setDio0Mapping(0));
    } else if (mode == sx127x::Mode::Transmit)
    {
      RxEn::reset();delay(100ms);
      TxEn::set();
      RF_CALL(SX127x<SpiMaster, Cs>::setDio0Mapping(1));
    } else if (mode == sx127x::Mode::ChnActvDetect)
    {
      TxEn::reset(); delay(100ms);
      RxEn::set(); 
      RF_CALL(SX127x<SpiMaster, Cs>::setDio0Mapping(2));
    }

    RF_CALL(SX127x<SpiMaster, Cs>::setOperationMode(mode));

    RF_END();
};
}