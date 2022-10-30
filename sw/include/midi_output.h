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

#ifndef MIDI_OUTPUT_H
#define MIDI_OUTPUT_H

#include <stdint.h>
#include "midi_types.h"

/**
@brief MIDI output interface translating synthesizer events into MIDI messages
@tparam Output Output driver class implementing a static method put(uint8_t), e.g. USART
*/
template <typename Output>
class MidiOutput
{
    public:

    /**
    @brief Send MIDI note on message to output
    @param channel Selected MIDI channel for output
    @param message Structure containing the note on data
    */
    static void write(const MidiNoteOn & message)
    {
        Output::put(message.status.byte);
        Output::put(message.note);
        Output::put(message.velocity);
    }

    /**
    @brief Send MIDI note off message to output
    @param channel Selected MIDI channel for output
    @param midiControlChange Structure containing the control change data
    */
    static void write(const MidiNoteOff & message)
    {
        Output::put(message.status.byte);
        Output::put(message.note);
        Output::put(message.velocity);
    }

    /**
    @brief Send MIDI control change message to output
    @param status MIDI status byte
    @param controller MIDI controller index
    @param value MIDI controller value
    */
    static void writeControlChange(const uint8_t status, const uint8_t controller, const uint8_t value)
    {
        Output::put(status);
        Output::put(controller);
        Output::put(value);
    }

    /**
    @brief Send MIDI control change message to output
    @param channel Selected MIDI channel for output
    @param controller MIDI controller index
    @param value MIDI controller value
    */
    static void writeControlChange(const MidiChannel channel, const uint8_t controller, const uint8_t value)
    {        
        writeControlChange((MidiStatus {{.channel = channel, .command = MidiCommand::CONTROL_CHANGE, .statusFlag = 1}}).byte, controller, value);
    }

    /**
    @brief Send MIDI control change message to output
    @param channel Selected MIDI channel for output
    @param message Structure containing the control change data
    */
    static void write(const MidiControlChange & message)
    {
        writeControlChange(message.status.byte, message.controller, message.value);
    }

    /**
    @brief Send MIDI control change message to output
    @param channel Selected MIDI channel for output
    @param message Structure containing the control change data
    */
    static void write(const MidiProgramChange & message)
    {
        Output::put(message.status.byte);
        Output::put(message.program);
    }
    
    /**
    @brief Send MIDI control change message to output
    @param channel Selected MIDI channel for output
    @param message Structure containing the control change data
    */
    static void write(const MidiPolyAfterTouch & message)
    {
        Output::put(message.status.byte);
        Output::put(message.note);
        Output::put(message.velocity);
    }

    /**
    @brief Send MIDI control change message to output
    @param channel Selected MIDI channel for output
    @param message Structure containing the control change data
    */
    static void write(const MidiChannelAfterTouch & message)
    {
        Output::put(message.status.byte);
        Output::put(message.velocity);
    }

    /**
    @brief Send MIDI control change message to output
    @param channel Selected MIDI channel for output
    @param message Structure containing the control change data
    */
    static void write(const MidiPitchBend & message)
    {
        Output::put(message.status.byte);
        Output::put(message.LSB);
        Output::put(message.MSB);
    }

    /**
    @brief Send MIDI SysEx message to output
    @param channel Selected MIDI channel for output
    @param message Structure containing the control change data
    */
    static void write(const MidiSysEx & message)
    {
        Output::put(message.data);
    }
};

#endif