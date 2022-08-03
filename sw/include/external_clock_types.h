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

#ifndef EXTERNAL_CLOCK_TYPES_H
#define EXTERNAL_CLOCK_TYPES_H

#include "type_traits.h"
#include <stdint.h>

//
enum class Ppqn : uint8_t
{
    _24 = 0,
    _48 = 1,
    NOFENTRIES,
    MIN = 0,
    MAX = NOFENTRIES - 1
};


constexpr Ppqn& operator++(Ppqn & arg)
{
    return arg = static_cast<Ppqn>(static_cast<uint8_t>(arg)+1);
}

constexpr Ppqn& operator--(Ppqn & arg)
{
    return arg = static_cast<Ppqn>(static_cast<uint8_t>(arg)-1);
}

/**
@brief Numeric limits of Ppqn
*/
template <>
struct numeric_limits <Ppqn>
{
    /**
    @brief Maximum value
    @result Maximum value of Ppqn
    */
    static constexpr Ppqn max()
    {
        return Ppqn::MAX;
    }
    
    /**
    @brief Minimum value
    @result Minimum value of Ppqn
    */
    static constexpr Ppqn min()
    {
        return Ppqn::MIN;
    }
};

#endif