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

#ifndef ARPEGGIATOR_PARAM_ENUMS_H
#define ARPEGGIATOR_PARAM_ENUMS_H

#include <stdint.h>


enum class ArpeggiatorParam : uint8_t
{
    MODE = 0,
    SPEED,
    PATTERN,
    SCALE,
    VELOCITY,
    BAR_LENGTH,
    NOFENTRIES
};


/// @brief Arpeggiator mode
enum class ArpeggiatorMode : uint8_t
{
    NORMAL = 0,
    HOLD,
    OFF,
    NOFENTRIES,
    MIN = 0,
    MAX = NOFENTRIES-1
};

inline ArpeggiatorMode& operator++(ArpeggiatorMode & arg)
{
    return arg = static_cast<ArpeggiatorMode>(static_cast<uint8_t>(arg)+1);
}

inline ArpeggiatorMode& operator--(ArpeggiatorMode & arg)
{
    return arg = static_cast<ArpeggiatorMode>(static_cast<uint8_t>(arg)-1);
}

/**
@brief Numeric limits of ArpeggiatorMode
*/
template <>
struct numeric_limits <ArpeggiatorMode>
{
    /**
    @brief Maximum value
    @result Maximum value of ArpeggiatorMode
    */
    static constexpr ArpeggiatorMode max()
    {
        return ArpeggiatorMode::MAX;
    }
    
    /**
    @brief Minimum value
    @result Minimum value of ArpeggiatorMode
    */
    static constexpr ArpeggiatorMode min()
    {
        return ArpeggiatorMode::MIN;
    }
};

/// @brief Arpeggiator pattern
enum class ArpeggiatorPattern : uint8_t
{
    UP = 0,
    DOWN,
    UPDOWN,
    UPDOWN_HOLD,
    RANDOM,
    NOFENTRIES,
    MIN = UP,
    MAX = NOFENTRIES-1
};

inline ArpeggiatorPattern& operator++(ArpeggiatorPattern & arg)
{
    return arg = static_cast<ArpeggiatorPattern>(static_cast<uint8_t>(arg)+1);
}

inline ArpeggiatorPattern& operator--(ArpeggiatorPattern & arg)
{
    return arg = static_cast<ArpeggiatorPattern>(static_cast<uint8_t>(arg)-1);
}

/**
@brief Numeric limits of ArpeggiatorPattern
*/
template <>
struct numeric_limits <ArpeggiatorPattern>
{
    /**
    @brief Maximum value
    @result Maximum value of ArpeggiatorPattern
    */
    static constexpr ArpeggiatorPattern max()
    {
        return ArpeggiatorPattern::MAX;
    }
    
    /**
    @brief Minimum value
    @result Minimum value of ArpeggiatorPattern
    */
    static constexpr ArpeggiatorPattern min()
    {
        return ArpeggiatorPattern::MIN;
    }
};

#endif