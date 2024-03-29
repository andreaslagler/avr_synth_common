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

#ifndef INTERNAL_CLOCK_H
#define INTERNAL_CLOCK_H

#include "subject.h"
#include <stdint.h>

// Internal clock needs correct CPU clock for proper timing
#ifndef F_CPU
#warning "internal_clock.h : F_CPU not defined, using default F_CPU = 16MHz"
#define F_CPU 16000000UL
#endif

/**
@brief Implementation of a bpm clock to be used with a 16bit timer in CTC mode
This implementation divides the CPU clock by a 16bit and an 8bit clock divider, wherein
- the 16bit clock division is realized using the output compare match interrupt of a 16bit timer in CTC mode
- the 8bit clock division is done in software
*/
class InternalClock : public Subject<void>
{
    public:
    
    /**
    @brief Callback for clock event, e.g. interrupt of the associated timer
    */
    constexpr void clock()
    {
        // Clock division by 8bit clock divider
        if (0 == --m_clock)
        {
            notifyObserver();
            m_clock = m_clockPostDivider;
        }
    }
           
    /**
    @brief Set bpm value
    */
    constexpr void setBpmParam(const uint8_t bpmParam)
    {
        // Minimum bpm without clock prescaling @ 20 MHz CPU clock, 24 ppqn clock resolution, 16 bit timer resolution
        // 20e6 * (60 / 24) / 65536 = 763 bpm
        constexpr uint8_t ppqn = 24;
        constexpr uint32_t scaledCpuClock = static_cast<uint32_t>((F_CPU * 60) / ppqn);

        // Initial value for bpm counter
        const uint16_t bpm = bpmParam + s_minBpm;
        uint32_t clockDivider = scaledCpuClock / bpm;
        
        // Split 32bit clock divider into a 16bit clock pre divider and an 8bit clock post divider
        m_clockPostDivider = (clockDivider >> 16) + 1;
        m_clockPreDivider = static_cast<uint16_t>(clockDivider / m_clockPostDivider);
        
        // Reset clock
        m_clock = m_clockPostDivider;
    }
    
    private:
    
    uint16_t m_clockPreDivider = 1;
    uint8_t m_clockPostDivider = 1;
    uint8_t m_clock = 1;
    static constexpr uint16_t s_minBpm = 45;
};

#endif