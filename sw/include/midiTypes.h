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

/// @brief MIDI message NOTE OFF
typedef struct
{
    uint8_t note;
    uint8_t velocity;
} MidiNoteOff;

/// @brief MIDI message NOTE ON
typedef struct
{
    uint8_t note;
    uint8_t velocity;
} MidiNoteOn;

/// @brief MIDI message POLYPHONIC AFTERTOUCH
typedef struct
{
    uint8_t note;
    uint8_t velocity;
} MidiPolyAfterTouch;

/// @brief MIDI message CONTROL CHANGE
typedef struct
{
    uint8_t controller;
    uint8_t value;
} MidiControlChange;

/// @brief MIDI message PROGRAM CHANGE
typedef struct
{
    uint8_t program;
} MidiProgramChange;

/// @brief MIDI message CHANNEL AFTERTOUCH
typedef struct
{
    uint8_t velocity;
} MidiChannelAfterTouch;

/// @brief MIDI message PITCH BEND
typedef struct
{
    uint8_t LSB;
    uint8_t MSB;
} MidiPitchBend;

/// @brief MIDI message SYSEX
typedef struct
{
    uint8_t data;
} MidiSysEx;

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

#endif