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

#ifndef MIDI_TYPES_H
#define MIDI_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#include <variant.h>

// MIDI types used for both MIDI input and output

/// @brief MIDI channel
enum class MidiChannel : uint8_t
{
    _1 = 0,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7,
    _8,
    _9,
    _10,
    _11,
    _12,
    _13,
    _14,
    _15,
    _16
};

/// @brief MIDI channel mode
enum class MidiChannelMode : uint8_t
{
    OMNI = 0
};

// MIDI commands
enum class MidiCommand : uint8_t
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


/**
@brief
*/
enum class MidiSysExMessage : uint8_t
{
    BEGIN = 0xF0,
    MTC_SHORT_MESSAGE = 0xF1,
    SONG_POSITION = 0xF2,
    SONG_SELECT = 0xF3,
    TUNE_REQUEST = 0xF6,
    END = 0xF7,
    TIMING_CLOCK = 0xF8,
    START = 0xFA,
    CONTINUE = 0xFB,
    STOP = 0xFC,
    ACTIVE_SENSE = 0xFE,
    RESET = 0xFF
};

union MidiStatus
{
    struct
    {
        // MIDI channel
        MidiChannel channel : 4;

        // MIDI Command
        MidiCommand command : 3;

        // MIDI status flag
        uint8_t statusFlag : 1;
    };
    
    uint8_t byte;
    MidiSysExMessage sysExMessage;
};

/// @brief MIDI message NOTE OFF
struct MidiNoteOff
{
    MidiNoteOff() = default;
    
    MidiNoteOff(
    const MidiChannel _channel,
    const uint8_t _note,
    const uint8_t _velocity)
    :
    status(MidiStatus {{.channel = _channel, .command = MidiCommand::NOTE_OFF, .statusFlag = 1}}),
    note(_note),
    velocity(_velocity)
    {}
    
    MidiStatus status;
    uint8_t note;
    uint8_t velocity;
};

/// @brief MIDI message NOTE ON
struct MidiNoteOn
{
    MidiNoteOn() = default;
    
    MidiNoteOn(
    const MidiChannel _channel,
    const uint8_t _note,
    const uint8_t _velocity)
    :
    status(MidiStatus {{.channel = _channel, .command = MidiCommand::NOTE_ON, .statusFlag = 1}}),
    note(_note),
    velocity(_velocity)
    {}
        
    MidiStatus status;
    uint8_t note;
    uint8_t velocity;
};

/// @brief MIDI message POLYPHONIC AFTERTOUCH
struct MidiPolyAfterTouch
{
    MidiStatus status;
    uint8_t note;
    uint8_t velocity;
};

/// @brief MIDI message CONTROL CHANGE
struct MidiControlChange
{
    MidiControlChange() = default;
    
    MidiControlChange(
    const MidiChannel _channel,
    const uint8_t _controller,
    const uint8_t _value)
    :
    status(MidiStatus {{.channel = _channel, .command = MidiCommand::CONTROL_CHANGE, .statusFlag = 1}}),
    controller(_controller),
    value(_value)
    {}

    MidiStatus status;
    uint8_t controller;
    uint8_t value;
};

/// @brief MIDI message PROGRAM CHANGE
struct MidiProgramChange
{
    MidiProgramChange() = default;
    
    MidiProgramChange(
    const MidiChannel _channel,
    const uint8_t _program)
    :
    status(MidiStatus {{.channel = _channel, .command = MidiCommand::PROGRAM_CHANGE, .statusFlag = 1}}),
    program(_program)
    {}
        
    MidiStatus status;
    uint8_t program;
};

/// @brief MIDI message CHANNEL AFTERTOUCH
struct MidiChannelAfterTouch
{
    MidiStatus status;
    uint8_t velocity;
};

/// @brief MIDI message PITCH BEND
struct MidiPitchBend
{
    MidiStatus status;
    uint8_t LSB;
    uint8_t MSB;
};

/// @brief MIDI message SYSEX
struct MidiSysEx
{
    MidiSysEx() = default;
    
    MidiSysEx(
    const uint8_t _data)
    :
    data(_data)
    {}
        
    uint8_t data;
};

using MidiMessage = Variant<
MidiNoteOff,
MidiNoteOn,
MidiPolyAfterTouch,
MidiControlChange,
MidiProgramChange,
MidiChannelAfterTouch,
MidiPitchBend,
MidiSysExMessage,
MidiSysEx>;

#endif