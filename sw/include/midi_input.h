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

#ifndef MIDI_INPUT_H
#define MIDI_INPUT_H

#include <stdint.h>
#include <stdbool.h>
#include "midi_types.h"
#include "lookup_table.h"
#include "subject.h"

/**
@brief MIDI input interface parsing a serial stream of bytes into MIDI messages
*/
class MidiInput
{
    public:

    /**
    @brief Constructor
    */
    constexpr MidiInput() = default;
    
    /**
    @brief Parse a received byte of MIDI data and trigger notifications on complete MIDI messages
    @param rxByte One byte of received MIDI data
    @todo sicher stellen, dass eine message ohne datenbyte nur einmal empfangen wird
    @todo SysEx status geht vermutlich verloren!!!
    */
    void parse(const uint8_t rxByte)
    {
        // Check if received byte is a status byte
        const MidiStatus status(rxByte);
        if (status.statusFlag)
        {
            parsestatus(status);
        }
        else
        {
            parseDataByte(rxByte);
        }
    }

    /**
    @brief Register an observer for MIDI SysEx real-time messages
    Notified MIDI SysEx real-time messages are:
    - MIDI clock
    - START command
    - CONTINUE command
    - STOP command
    - Active sensing
    - System reset
    @param observer Observer to be registered
    */
    constexpr void registerObserver(const typename Subject<const MidiSysExMessage &>::Observer& observer)
    {
        m_subjectSysExMessage.registerObserver(observer);
    }

    /**
    @brief Register an observer for MIDI note-off messages
    @param observer Observer to be registered
    */
    constexpr void registerObserver(const typename Subject<const MidiNoteOff&>::Observer& observer)
    {
        m_subjectNoteOff.registerObserver(observer);
    }

    /**
    @brief Register an observer for MIDI note-on messages
    @param observer Observer to be registered
    */
    constexpr void registerObserver(const typename Subject<const MidiNoteOn&>::Observer& observer)
    {
        m_subjectNoteOn.registerObserver(observer);
    }

    /**
    @brief Register an observer for MIDI polyphonic aftertouch messages
    @param observer Observer to be registered
    */
    constexpr void registerObserver(const typename Subject<const MidiPolyAfterTouch&>::Observer& observer)
    {
        m_subjectPolyAfterTouch.registerObserver(observer);
    }

    /**
    @brief Register an observer for MIDI control change messages
    @param observer Observer to be registered
    */
    constexpr void registerObserver(const typename Subject<const MidiControlChange&>::Observer& observer)
    {
        m_subjectControlChange.registerObserver(observer);
    }

    /**
    @brief Register an observer for MIDI program change messages
    @param observer Observer to be registered
    */
    constexpr void registerObserver(const typename Subject<const MidiProgramChange&>::Observer& observer)
    {
        m_subjectProgramChange.registerObserver(observer);
    }

    /**
    @brief Register an observer for MIDI channel aftertouch messages
    @param observer Observer to be registered
    */
    constexpr void registerObserver(const typename Subject<const MidiChannelAfterTouch&>::Observer& observer)
    {
        m_subjectChannelAfterTouch.registerObserver(observer);
    }

    /**
    @brief Register an observer for MIDI pitch-bend messages
    @param observer Observer to be registered
    */
    constexpr void registerObserver(const typename Subject<const MidiPitchBend&>::Observer& observer)
    {
        m_subjectPitchBend.registerObserver(observer);
    }

    /**
    @brief Register an observer for MIDI SysEx messages
    @param observer Observer to be registered
    */
    constexpr void registerObserver(const typename Subject<const MidiSysEx&>::Observer& observer)
    {
        m_subjectSysEx.registerObserver(observer);
    }
    
    private:

    // Parse a status byte
    void parsestatus(const MidiStatus status)
    {
        m_currentMidiData.status = status;
        
        // Change parser state according to received MIDI command
        switch (status.command)
        {
            case MidiCommand::NOTE_OFF:
            m_state = NOTE_OFF_STATUS_RECEIVED;
            break;
            
            case MidiCommand::NOTE_ON:
            m_state = NOTE_ON_STATUS_RECEIVED;
            break;
            
            case MidiCommand::POLY_AFTER_TOUCH:
            m_state = POLY_AFTER_TOUCH_STATUS_RECEIVED;
            break;
            
            case MidiCommand::CONTROL_CHANGE:
            m_state = CONTROL_CHANGE_STATUS_RECEIVED;
            break;
            
            case MidiCommand::PROGRAM_CHANGE:
            m_state = PROGRAM_CHANGE_STATUS_RECEIVED;
            break;
            
            case MidiCommand::CHANNEL_AFTER_TOUCH:
            m_state = CHANNEL_AFTER_TOUCH_STATUS_RECEIVED;
            break;
            
            case MidiCommand::PITCH_BEND_CHANGE:
            m_state = PITCH_BEND_CHANGE_STATUS_RECEIVED;
            break;
            
            case MidiCommand::SYSEX_MESSAGE:
            {
                // Filter SysEx real-time messages
                // These types of messages may be transmitted anytime and must not affect the parser output
                switch (status.sysExMessage)
                {
                    case MidiSysExMessage::TIMING_CLOCK: // Midi Clock
                    case MidiSysExMessage::START: // Start Command
                    case MidiSysExMessage::CONTINUE: // Continue Command
                    case MidiSysExMessage::STOP: // Stop Command
                    case MidiSysExMessage::ACTIVE_SENSE: // Active Sensing
                    case MidiSysExMessage::RESET: // System Reset
                    m_subjectSysExMessage.notifyObserver(status.sysExMessage);
                    break;
                    
                    default:
                    m_state = SYSEX_MESSAGE_RECEIVED;
                    break;
                }
            }
            break;
            
            default:
            m_state = IDLE;
            break;
        }
    }
    
    // Parse a data byte
    void parseDataByte(const uint8_t byte)
    {
        // Received byte is a data byte
        switch (m_state)
        {
            case NOTE_OFF_STATUS_RECEIVED: // Parser has received the status byte of a note-off message
            m_currentMidiData.data[0] = byte;
            m_state = NOTE_OFF_DATA_RECEIVED;
            break;

            case NOTE_OFF_DATA_RECEIVED: // Parser has received the first data byte of a note-off message
            m_currentMidiData.data[1] = byte;
            m_subjectNoteOff.notifyObserver(m_currentMidiData.noteOff);
            m_state = NOTE_OFF_STATUS_RECEIVED; // running status
            break;

            case NOTE_ON_STATUS_RECEIVED: // Parser has received the status byte of a note-on message
            m_currentMidiData.data[0] = byte;
            m_state = NOTE_ON_DATA_RECEIVED;
            break;

            case NOTE_ON_DATA_RECEIVED: // Parser has received the first data byte of a note-on message
            m_currentMidiData.data[1] = byte;
            m_subjectNoteOn.notifyObserver(m_currentMidiData.noteOn);
            m_state = NOTE_ON_STATUS_RECEIVED; // running status
            break;

            case POLY_AFTER_TOUCH_STATUS_RECEIVED: // Parser has received the status byte of a polyphonic aftertouch message
            m_currentMidiData.data[0] = byte;
            m_state = POLY_AFTER_TOUCH_DATA_RECEIVED;
            break;

            case POLY_AFTER_TOUCH_DATA_RECEIVED: // Parser has received the first data byte of a polyphonic aftertouch message
            m_currentMidiData.data[1] = byte;
            m_subjectPolyAfterTouch.notifyObserver(m_currentMidiData.polyAftertouch);
            m_state = POLY_AFTER_TOUCH_STATUS_RECEIVED;
            break;

            case CONTROL_CHANGE_STATUS_RECEIVED: // Parser has received the status byte of a control change message
            m_currentMidiData.data[0] = byte;
            m_state = CONTROL_CHANGE_DATA_RECEIVED;
            break;

            case CONTROL_CHANGE_DATA_RECEIVED: // Parser has received the first data byte of a control change message
            m_currentMidiData.data[1] = byte;
            m_subjectControlChange.notifyObserver(m_currentMidiData.controlChange);
            m_state = CONTROL_CHANGE_STATUS_RECEIVED;
            break;

            case PROGRAM_CHANGE_STATUS_RECEIVED: // Parser has received the status byte of a program change message
            m_currentMidiData.data[0] = byte;
            m_subjectProgramChange.notifyObserver(m_currentMidiData.programChange);
            break;

            case CHANNEL_AFTER_TOUCH_STATUS_RECEIVED: // Parser has received the status byte of a channel aftertouch message
            m_currentMidiData.data[0] = byte;
            m_subjectChannelAfterTouch.notifyObserver(m_currentMidiData.channelAftertouch);
            break;

            case PITCH_BEND_CHANGE_STATUS_RECEIVED: // Parser has received the status byte of a pitch-bend message
            m_currentMidiData.data[0] = byte;
            m_state = PITCH_BEND_CHANGE_DATA_RECEIVED;
            break;

            case PITCH_BEND_CHANGE_DATA_RECEIVED: // Parser has received the first data byte of a pitch-bend message
            m_currentMidiData.data[1] = byte;
            m_subjectPitchBend.notifyObserver(m_currentMidiData.pitchBend);
            m_state = PITCH_BEND_CHANGE_STATUS_RECEIVED;
            break;

            case SYSEX_MESSAGE_RECEIVED: // Parser has received a SysEx message
            /// @todo Support SysEx messages
            break;

            default:
            break;
        }
    }
    
    // State of MIDI parser
    enum
    {
        IDLE = 0, // Parser is in idle state after a real-time SysEx Message has been received
        NOTE_OFF_STATUS_RECEIVED, // Parser has received the status byte of a note-off message
        NOTE_OFF_DATA_RECEIVED, // Parser has received the first data byte of a note-off message
        NOTE_ON_STATUS_RECEIVED, // Parser has received the status byte of a note-on message
        NOTE_ON_DATA_RECEIVED, // Parser has received the first data byte of a note-on message
        POLY_AFTER_TOUCH_STATUS_RECEIVED, // Parser has received the status byte of a polyphonic aftertouch message
        POLY_AFTER_TOUCH_DATA_RECEIVED, // Parser has received the first data byte of a polyphonic aftertouch message
        CONTROL_CHANGE_STATUS_RECEIVED, // Parser has received the status byte of a control change message
        CONTROL_CHANGE_DATA_RECEIVED, // Parser has received the first data byte of a control change message
        PROGRAM_CHANGE_STATUS_RECEIVED, // Parser has received the status byte of a program change message
        CHANNEL_AFTER_TOUCH_STATUS_RECEIVED, // Parser has received the status byte of a channel aftertouch message
        PITCH_BEND_CHANGE_STATUS_RECEIVED, // Parser has received the status byte of a pitch-bend message
        PITCH_BEND_CHANGE_DATA_RECEIVED, // Parser has received the first data byte of a pitch-bend message
        SYSEX_MESSAGE_RECEIVED // Parser has received a SysEx message
    }
    m_state {IDLE};

    // MIDI data collected during parsing the serial stream
    union
    {
        struct
        {
            // MIDI channel
            MidiStatus status;
            
            // Up to two data bytes
            uint8_t data[2] = {0,0}; // Make sure MSB is always zero
        };
        
        MidiNoteOff noteOff;
        MidiNoteOn noteOn;
        MidiPolyAfterTouch polyAftertouch;
        MidiControlChange controlChange;
        MidiProgramChange programChange;
        MidiChannelAfterTouch channelAftertouch;
        MidiPitchBend pitchBend;
        MidiSysEx sysEx;
    }
    m_currentMidiData;
    
    // Subjects for all possible MIDI messages
    Subject<const MidiSysExMessage&> m_subjectSysExMessage;
    Subject<const MidiNoteOff&> m_subjectNoteOff;
    Subject<const MidiNoteOn&> m_subjectNoteOn;
    Subject<const MidiPolyAfterTouch&> m_subjectPolyAfterTouch;
    Subject<const MidiControlChange&> m_subjectControlChange;
    Subject<const MidiProgramChange&> m_subjectProgramChange;
    Subject<const MidiChannelAfterTouch&> m_subjectChannelAfterTouch;
    Subject<const MidiPitchBend&> m_subjectPitchBend;
    Subject<const MidiSysEx&> m_subjectSysEx;
};

#endif