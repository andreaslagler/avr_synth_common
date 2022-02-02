/*
Copyright (C) 2022  Andreas Lagler

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef ARPEGGIATOR_PARAM_TYPES_H
#define ARPEGGIATOR_PARAM_TYPES_H

#include "ParamType.h"

#include "ArpeggiatorParam_enums.h"
#include "lookup_table.h"
#include <stdint.h>

enum class ArpeggiatorParamType : uint8_t
{
    NONE,
    MODE,
    PATTERN,
    BPM,
    SCALE,
    NOFENTRIES
};

// Assign parameter type to parameter indices using a lookup-table stored in progmem.
// Default value is "no type", i.e. a bare unsigned char
inline ArpeggiatorParamType getParamType(const ArpeggiatorParam param)
{
    typedef SparseLUT<ArpeggiatorParamType, uint8_t, static_cast<uint8_t>(ArpeggiatorParam::NOFENTRIES)>::Entry Entry;

    constexpr static const PROGMEM SparseLUT<ArpeggiatorParamType, uint8_t, static_cast<uint8_t>(ArpeggiatorParam::NOFENTRIES)> paramType(
    ArpeggiatorParamType::NONE,
    Entry{static_cast<uint8_t>(ArpeggiatorParam::MODE), ArpeggiatorParamType::MODE},
    Entry{static_cast<uint8_t>(ArpeggiatorParam::PATTERN), ArpeggiatorParamType::PATTERN},
    Entry{static_cast<uint8_t>(ArpeggiatorParam::SPEED), ArpeggiatorParamType::BPM},
    Entry{static_cast<uint8_t>(ArpeggiatorParam::SCALE), ArpeggiatorParamType::SCALE}
    );

    return paramType.getP(static_cast<uint8_t>(param));
}



// Assign min value to parameter types using a lookup-table stored in progmem.
// Default value is 0
inline uint8_t getMinValue(const ArpeggiatorParamType /*patchParamType*/)
{
    //static const PROGMEM SparseLUT<uint8_t, uint8_t, static_cast<uint8_t>(ArpeggiatorParamType::NOFENTRIES)> LUT(
    //0);
    //
    //return LUT.getP(static_cast<uint8_t>(patchParamType));
    
    return 0;
}

// Assign max value to parameter types using a lookup-table stored in progmem.
// Default value is 255
inline uint8_t getMaxValue(const ArpeggiatorParamType paramType)
{
    typedef SparseLUT<uint8_t, uint8_t, static_cast<uint8_t>(ArpeggiatorParamType::NOFENTRIES)>::Entry Entry;

    static const PROGMEM SparseLUT<uint8_t, uint8_t, static_cast<uint8_t>(ArpeggiatorParamType::NOFENTRIES)> LUT(
    255,
    Entry{static_cast<uint8_t>(ArpeggiatorParamType::MODE), static_cast<uint8_t>(ArpeggiatorMode::MAX)},
    Entry{static_cast<uint8_t>(ArpeggiatorParamType::PATTERN), static_cast<uint8_t>(ArpeggiatorPattern::MAX)},
    Entry{static_cast<uint8_t>(ArpeggiatorParamType::SCALE), static_cast<uint8_t>(Scale::MAX)});

    return LUT.getP(static_cast<uint8_t>(paramType));
}
    
    
    


#endif