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

#ifndef ARPEGGIATOR_H
#define ARPEGGIATOR_H

#include "ArpeggiatorParam_enums.h"
#include "ArpeggiatorParamTypes.h"
#include "subject.h"
//#include "MidiTypes.h"
#include "Param.h"
#include "vector_sorted.h"
#include "clock_divider.h"
#include "lookup_table.h"
#include <stdlib.h> // rand()


static constexpr void updatePatternUP(const auto& keys, auto& notes)
{
    notes = keys;
}

static constexpr void updatePatternDOWN(const auto& keys, auto& notes)
{
    notes.resize(keys.length());
    auto dst = notes.rbegin();
    for (const auto key : keys)
    {
        *dst++ = key;
    }
}

static constexpr void updatePatternUPDOWN(const auto& keys, auto& notes)
{
    const uint8_t nofKeys = keys.length();
    switch (nofKeys)
    {
        case 0:
        case 1:
        case 2:
        // 0 Notes held --> 0 steps
        // 1 Note held --> 1 step
        // 2 Notes held --> 2 steps
        notes = keys;
        break;
        
        default:
        // N Notes held --> 2 * (N-1) steps, e.g. 4 Notes --> 1 2 3 4 3 2 = 6 steps
        {
            notes.resize((nofKeys-1) << 1);
            auto dst = notes.begin();
            auto src = keys.begin();
            
            for (;src < keys.end(); ++src, ++dst)
            {
                *dst = *src;
            }
            
            for (--src, --src; src > keys.begin(); --src, ++dst)
            {
                *dst = *src;
            }
        }
        break;
    }
}

static constexpr void updatePatternUPDOWNNHOLD(const auto& keys, auto& notes)
{
    const uint8_t nofKeys = keys.length();
    switch (nofKeys)
    {
        case 0:
        case 1:
        // 0 Notes held --> 0 steps
        // 1 Note held --> 1 step
        notes = keys;
        break;
        
        default:
        // N Notes held --> 2 * N steps, e.g. 4 Notes --> 1 2 3 4 4 3 2 1 = 8 steps
        {
            notes.resize(nofKeys << 1);
            auto dst = notes.begin();
            auto src = keys.begin();
            
            for (; src < keys.end(); ++src, ++dst)
            {
                *dst = *src;
            }
            
            for (--src; src >= keys.begin(); --src, ++dst)
            {
                *dst = *src;
            }
        }
        break;
    }
}






/**
@brief Arpeggiator parameters
This class is also used to load and save arpeggiator parameters as part of a scene
*/
class ArpeggiatorParams
{
    public:
    
    /**
    @brief Size of serialized data in bytes
    */
    static constexpr uint8_t size()
    {
        return 5;
    }
    
    void setParam(const ArpeggiatorParam param, const uint8_t value)
    {
        m_params[static_cast<uint8_t>(param)] = value;
        
        switch (param)
        {
            case ArpeggiatorParam::SPEED:
            case ArpeggiatorParam::SCALE:
            updateBPMClock();
            break;
            default:
            break;
        }
        
        notify(param);
    }

    void incParam(const ArpeggiatorParam param)
    {
        // Get parameter type
        const ArpeggiatorParamType paramType = getParamType(param);

        // Perform the actual increment operation considering the parameter limits
        const uint8_t maxValue = getMaxValue(paramType);
        
        m_params[static_cast<uint8_t>(param)].increment(maxValue);
        
        switch (param)
        {
            case ArpeggiatorParam::SPEED:
            case ArpeggiatorParam::SCALE:
            updateBPMClock();
            break;
            default:
            break;
        }
        
        notify(param);
    }

    void decParam(const ArpeggiatorParam param)
    {
        // Get parameter type
        const ArpeggiatorParamType paramType = getParamType(param);

        // Perform the actual increment operation considering the parameter limits
        const uint8_t minValue = getMinValue(paramType);

        m_params[static_cast<uint8_t>(param)].decrement(minValue);
        
        switch (param)
        {
            case ArpeggiatorParam::SPEED:
            case ArpeggiatorParam::SCALE:
            updateBPMClock();
            break;
            default:
            break;
        }
        
        notify(param);
    }

    // Increment parameter and roll-over (e.g. on push button event)
    void toggleParam(const ArpeggiatorParam param)
    {
        // Get parameter type
        const ArpeggiatorParamType paramType = getParamType(param);

        // Perform the actual increment operation considering the parameter limits
        const uint8_t minValue = getMinValue(paramType);
        const uint8_t maxValue = getMaxValue(paramType);
        m_params[static_cast<uint8_t>(param)].incrementRollover(minValue, maxValue);
        
        switch (param)
        {
            case ArpeggiatorParam::SPEED:
            case ArpeggiatorParam::SCALE:
            updateBPMClock();
            break;
            default:
            break;
        }
        
        notify(param);
    }

    [[nodiscard]] constexpr uint8_t getParam(const ArpeggiatorParam param) const
    {
        return m_params[static_cast<uint8_t>(param)];
    }
    
    // Get clock interval in milliseconds
    [[nodiscard]] constexpr uint16_t getClock() const
    {
        return m_clock;
    }
    

    void registerParamObserver(const typename Subject<ArpeggiatorParam, uint8_t>::Observer& observer)
    {
        m_subjectParamUpdate.registerObserver(observer);
    }
    
    constexpr ArpeggiatorParams()
    {
        m_params[static_cast<uint8_t>(ArpeggiatorParam::MODE)] = static_cast<uint8_t>(ArpeggiatorMode::OFF);
        m_params[static_cast<uint8_t>(ArpeggiatorParam::SCALE)] = static_cast<uint8_t>(Scale::_1_4);
        m_params[static_cast<uint8_t>(ArpeggiatorParam::VELOCITY)] = 127;
        
        updateBPMClock();
    }
    
    uint16_t m_clock {0};
    
    private:
    
    // Update BPM clock from BPM speed and arpeggiator scale
    constexpr void updateBPMClock()
    {
        // Convert 1/4 note to 1/32 note BPM
        uint16_t bpmSpeed = getParam(ArpeggiatorParam::SPEED);
        bpmSpeed += 45; // Convert 0..255 to 45..300 BPM
        uint16_t bpmScaled = bpmSpeed << (getParam(ArpeggiatorParam::SCALE)-static_cast<uint8_t>(Scale::_1_4)); // Multiply BPM speed with scale
        m_clock = 60000U / bpmScaled; // Convert BPM speed to clock interval in milliseconds
    }

    Subject<ArpeggiatorParam, uint8_t> m_subjectParamUpdate;
    
    void notify(const ArpeggiatorParam param)
    {
        m_subjectParamUpdate.notifyObserver(param, getParam(param));
    }

    // Actual arpeggiator parameters
    Param<uint8_t> m_params[static_cast<uint8_t>(ArpeggiatorParam::NOFENTRIES)];
};

/**
@brief Arpeggiator class
*/
class Arpeggiator
:
public ArpeggiatorParams
{
    public:
    
    // Standard Constructor
    constexpr Arpeggiator() = default;
    
    // Register observer for Note On events
    void registerNoteOnObserver(const typename Subject<uint8_t, uint8_t>::Observer& observer)
    {
        m_subjectNoteOn.registerObserver(observer);
    }
    
    // Register observer for Note Off events
    void registerNoteOffObserver(const typename Subject<uint8_t>::Observer& observer)
    {
        m_subjectNoteOff.registerObserver(observer);
    }
    
    /**
    @brief Get arpeggiator state
    @result Flag indicating if the arpeggiator is active
    */
    [[nodiscard]] constexpr bool isActive() const
    {
        return ArpeggiatorMode::HOLD == getMode();
    }

    // Reset the arpeggiator
    constexpr void resetCurrentStep()
    {
        m_currentStep = 0;
    }
    
    //// Increment 24 ppqn clock
    //constexpr void clock24PPQN()
    //{
    //if (m_clockDivider.clock())
    //{
    //clock();
    //}
    //}

    // Increment clock by one step
    constexpr void clock()
    {
        // Current note off
        noteOff();

        // Play next note
        playNextNote();
    }
    
    // Add a note to arpeggiator
    constexpr void addNote(const uint8_t note)
    {
        switch (getMode())
        {
            case ArpeggiatorMode::NORMAL:
            case ArpeggiatorMode::HOLD:
            m_keys.insert(note);
            updatePattern();
            break;
            
            default:
            break;
        }
    }
    
    // Remove a note from arpeggiator
    constexpr void removeNote(const uint8_t note)
    {
        // Remove note from pattern
        m_keys.remove(note);
        switch (getMode())
        {
            case ArpeggiatorMode::NORMAL:
            // Send Note Off
            noteOff(note);
            
            updatePattern();
            
            // Reset the arpeggiator to avoid glitch
            resetCurrentStep();
            break;

            default:
            break;
        }
    }
    
    // Remove all notes from arpeggiator
    constexpr void clear()
    {
        // Send Note Off for current note
        noteOff();
        
        // Clear the notes
        m_keys.clear();
        m_notes.clear();
        
        // Reset the arpeggiator to avoid glitch
        resetCurrentStep();
    }
    
    private:
 
    static constexpr uint8_t t_maxNofHeldKeys = 10;
    
    // Vector containing the held keys
    VectorSorted<uint8_t, uint8_t, t_maxNofHeldKeys, SortOrder::ASCENDING> m_keys;
    
    // Vector containing all notes of a pattern
    static constexpr uint8_t m_maxPatternLength = t_maxNofHeldKeys * 2;
    Vector<uint8_t, uint8_t, m_maxPatternLength> m_notes {0};
    
    uint8_t m_currentNote {255};

    // Step inside the pattern
    uint8_t m_currentStep {0};

    // Clock divider for 24 PPQN to 1/32 Note clock --> divide by 3
    ClockDivider<uint8_t, 3> m_clockDivider;
    
    Subject<uint8_t, uint8_t> m_subjectNoteOn;
    Subject<uint8_t> m_subjectNoteOff;
    
    [[nodiscard]] constexpr ArpeggiatorMode getMode() const
    {
        return static_cast<ArpeggiatorMode>(getParam(ArpeggiatorParam::MODE));
    }
    
    [[nodiscard]] constexpr auto getPattern() const
    {
        return static_cast<ArpeggiatorPattern>(getParam(ArpeggiatorParam::PATTERN));
    }
    
    constexpr void noteOn(const uint8_t note) const
    {
        m_subjectNoteOn.notifyObserver(note, getParam(ArpeggiatorParam::VELOCITY));
    }
    
    constexpr void noteOff(const uint8_t note) const
    {
        m_subjectNoteOff.notifyObserver(note);
    }
    constexpr void noteOff() const
    {
        noteOff(m_currentNote);
    }
    
    constexpr void updatePattern()
    {
        switch (getPattern())
        {
            case ArpeggiatorPattern::UP:
            updatePatternUP(m_keys, m_notes);
            break;

            case ArpeggiatorPattern::DOWN:
            updatePatternDOWN(m_keys, m_notes);
            break;
            
            case ArpeggiatorPattern::UPDOWN:
            updatePatternUPDOWN(m_keys, m_notes);
            break;
            
            case ArpeggiatorPattern::UPDOWN_HOLD:
            updatePatternUPDOWNNHOLD(m_keys, m_notes);
            break;
            
            case ArpeggiatorPattern::RANDOM:
            updatePatternUP(m_keys, m_notes);
            break;

            default:
            m_notes.clear();
        }
    }    
    
    // Set the note which is played next according to pattern
    constexpr void playNextNote()
    {
        const uint8_t nofSteps = m_notes.length();
        if (0 == nofSteps)
        {
            m_currentStep = 0;
            return;
        }
        
        switch (getPattern())
        {
            case ArpeggiatorPattern::UP:
            case ArpeggiatorPattern::DOWN:
            case ArpeggiatorPattern::UPDOWN:
            case ArpeggiatorPattern::UPDOWN_HOLD:
            noteOn(m_currentNote = m_notes[m_currentStep]);
            m_currentStep++;
            if (nofSteps == m_currentStep)
            {
                m_currentStep = 0;
            }
            break;
            
            case ArpeggiatorPattern::RANDOM:
            {
                const uint8_t note = static_cast<uint8_t>(rand()) % m_notes.length();
                noteOn(m_currentNote = m_notes[note]);
            }
            break;
            
            default:
            break;
        }
    }
};

#endif