/*
 * Rotary.h - Copyright (c) 2014-2025 - Olivier Poncet
 *
 * This file is part of the Rotary library
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __INO_ROTARY_Rotary_h__
#define __INO_ROTARY_Rotary_h__

// ---------------------------------------------------------------------------
// Rotary_Traits
// ---------------------------------------------------------------------------

struct Rotary_Traits
{
    struct Setup
    {
        static constexpr uint8_t CLK_PINMODE = INPUT_PULLUP;
        static constexpr uint8_t DIR_PINMODE = INPUT_PULLUP;
        static constexpr uint8_t BTN_PINMODE = INPUT_PULLUP;
    };

    struct State
    {
        static constexpr uint8_t CLK_BITMASK = (1 << 0);
        static constexpr uint8_t DIR_BITMASK = (1 << 1);
        static constexpr uint8_t BTN_BITMASK = (1 << 2);
    };

    struct Default
    {
        static constexpr long    COUNT = 0L;
        static constexpr uint8_t STATE = 0;
    };
};

// ---------------------------------------------------------------------------
// Rotary_Wiring
// ---------------------------------------------------------------------------

struct Rotary_Wiring
{
    uint8_t clkPin;
    uint8_t dirPin;
    uint8_t btnPin;
};

// ---------------------------------------------------------------------------
// Rotary_Values
// ---------------------------------------------------------------------------

struct Rotary_Values
{
    long    count;
    uint8_t state;
};

// ---------------------------------------------------------------------------
// Rotary
// ---------------------------------------------------------------------------

class Rotary
{
public: // public interface
    Rotary ( const uint8_t clkPin
           , const uint8_t dirPin
           , const uint8_t btnPin );

    void begin();
    void reset();
    void end();

    bool read();

    long value() const
    {
        return _values.count;
    }

    uint8_t state() const
    {
        return _values.state;
    }

    bool clk() const
    {
        return _values.state & Traits::State::CLK_BITMASK;
    }

    bool dir() const
    {
        return _values.state & Traits::State::DIR_BITMASK;
    }

    bool btn() const
    {
        return _values.state & Traits::State::BTN_BITMASK;
    }

    uint8_t readState() const;

protected: // protected interface
    using Traits = Rotary_Traits;
    using Wiring = Rotary_Wiring;
    using Values = Rotary_Values;

protected: // protected data
    Wiring const _wiring;
    Values       _values;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __INO_ROTARY_Rotary_h__ */
