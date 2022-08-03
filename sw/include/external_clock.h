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

#ifndef EXTERNAL_CLOCK_H
#define EXTERNAL_CLOCK_H

#include "external_clock_types.h"
#include "param.h"
#include "subject.h"
#include "type_traits.h"
#include <stdint.h>


class ExternalClock : Subject<void>
{
    public:
    
    constexpr void clock()
    {
        notifyObserver();
    }
    
    constexpr uint8_t getClockDivider()
    {
        return s_baseClockDivider << static_cast<uint8_t>(m_ppqn.getValue());
    }
    
    // This has to go below the definition of numeric_limits <ExternalClock::Ppqn>
    constexpr Ppqn incPpqn()
    {
        return m_ppqn.increment();
    }

    // This has to go below the definition of numeric_limits <ExternalClock::Ppqn>
    constexpr Ppqn decPpqn()
    {
        return m_ppqn.decrement();
    }

    private:
    
    // Clock divider for dividing PPQN clock down to 16th clock. This clock divider value should be set in the used HW timer directly
    static constexpr uint8_t s_baseClockDivider = 6;
    Param<Ppqn> m_ppqn = Ppqn::MIN;
};



#endif