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

#ifndef MIDI_OUT_H
#define MIDI_OUT_H

#include <stdint.h>
#include "MidiTypes.h"

/**
@brief MIDI output interface translating synthesizer events into MIDI messages
@tparam Output Output driver class implementing a static method put(uint8_t), e.g. USART
*/
template <typename Output>
class MidiOutput
{
    public:
    
    /**
    @brief Constructor
    */
    constexpr MidiOutput() = default;

    /**
    @brief Set MIDI channel for output. All MIDI messages (except SysEx) will be assigned to this channel
    @param midiChannel Selected MIDI channel for output
    */
    constexpr void setChannel(const MidiChannel midiChannel)
    {
        m_statusByte.midiChannel = midiChannel;
    }

    /**
    @brief Send MIDI control change message to output
    @param midiControlChange Structure containing the control change data
    */
    void write(const MidiControlChange & midiControlChange)
    {
        m_statusByte.midiCommand = static_cast<uint8_t>(MidiCommand::CONTROL_CHANGE),

        Output::put(m_statusByte.byte);
        Output::put(midiControlChange.controller);
        Output::put(midiControlChange.value);
    }

    private:
    
    // MIDI commands
    enum MidiCommand : uint8_t
    {
        NOTE_OFF = 0,
        NOTE_ON,
        POLY_AFTER_TOUCH,
        CONTROL_CHANGE,
        PROGRAM_CHANGE,
        CHANNEL_AFTER_TOUCH,
        PITCH_BEND_CHANGE,
        SYSEX_MESSAGE
    };

    // MIDI status byte
    union
    {
        uint8_t byte;

        struct
        {
            // MIDI channel
            uint8_t midiChannel : 4;

            // MIDI Command
            uint8_t midiCommand : 3;

            // MIDI status flag
            uint8_t : 1;
        };
    }
    m_statusByte {.byte = 0xFF}; // Set to 0xFF to make sure status flag is set
};

#endif