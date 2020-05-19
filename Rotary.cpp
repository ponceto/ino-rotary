/*
 * Rotary.cpp - Copyright (c) 2014-2020 - Olivier Poncet
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
#include <Arduino.h>
#include "Rotary.h"

// ---------------------------------------------------------------------------
// some useful macros
// ---------------------------------------------------------------------------

#ifndef countof
#define countof(array) (sizeof(array) / sizeof(array[0]))
#endif

// ---------------------------------------------------------------------------
// some useful stuff
// ---------------------------------------------------------------------------

namespace {

typedef void(*RotaryHandler)();

void rotaryHandler0();
void rotaryHandler1();
void rotaryHandler2();
void rotaryHandler3();

Rotary* volatile rotaries[4] = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
};

RotaryHandler const handlers[4] = {
    &rotaryHandler0,
    &rotaryHandler1,
    &rotaryHandler2,
    &rotaryHandler3,
};

void rotaryHandler0()
{
    Rotary* rotary(rotaries[0]);

    if(rotary != nullptr) {
        rotary->read();
    }
}

void rotaryHandler1()
{
    Rotary* rotary(rotaries[1]);

    if(rotary != nullptr) {
        rotary->read();
    }
}

void rotaryHandler2()
{
    Rotary* rotary(rotaries[2]);

    if(rotary != nullptr) {
        rotary->read();
    }
}

void rotaryHandler3()
{
    Rotary* rotary(rotaries[3]);

    if(rotary != nullptr) {
        rotary->read();
    }
}

uint8_t findRotary(Rotary* rotary)
{
    constexpr uint8_t rotaryNotFound = static_cast<uint8_t>(-1);

    constexpr uint8_t count = countof(rotaries);
    for(uint8_t index = 0; index < count; ++index) {
        Rotary* current(rotaries[index]);
        if(current == rotary) {
            return index;
        }
    }
    return rotaryNotFound;
}

bool attachRotaryToAnInterrupt(Rotary& rotary, const Rotary_Wiring& wiring)
{
    constexpr uint8_t notAnInterrupt  = static_cast<uint8_t>(NOT_AN_INTERRUPT);
    const     uint8_t clkPinInterrupt = digitalPinToInterrupt(wiring.clkPin);
    const     uint8_t dirPinInterrupt = digitalPinToInterrupt(wiring.dirPin);
    const     uint8_t btnPinInterrupt = digitalPinToInterrupt(wiring.btnPin);
    constexpr uint8_t rotaryNotFound  = static_cast<uint8_t>(-1);
    const     uint8_t rotaryIndex     = findRotary(nullptr);

    if((clkPinInterrupt == notAnInterrupt)
    && (dirPinInterrupt == notAnInterrupt)
    && (btnPinInterrupt == notAnInterrupt)) {
        return false;
    }
    if(rotaryIndex == rotaryNotFound) {
        return false;
    }
    else {
        rotaries[rotaryIndex] = &rotary;
    }
    if(clkPinInterrupt != notAnInterrupt) {
        ::attachInterrupt(clkPinInterrupt, handlers[rotaryIndex], CHANGE);
    }
    if(dirPinInterrupt != notAnInterrupt) {
        ::attachInterrupt(dirPinInterrupt, handlers[rotaryIndex], CHANGE);
    }
    if(btnPinInterrupt != notAnInterrupt) {
        ::attachInterrupt(btnPinInterrupt, handlers[rotaryIndex], CHANGE);
    }
    return true;
}

bool detachRotaryFromInterrupt(Rotary& rotary, const Rotary_Wiring& wiring)
{
    constexpr uint8_t notAnInterrupt  = static_cast<uint8_t>(NOT_AN_INTERRUPT);
    const     uint8_t clkPinInterrupt = digitalPinToInterrupt(wiring.clkPin);
    const     uint8_t dirPinInterrupt = digitalPinToInterrupt(wiring.dirPin);
    const     uint8_t btnPinInterrupt = digitalPinToInterrupt(wiring.btnPin);
    constexpr uint8_t rotaryNotFound  = static_cast<uint8_t>(-1);
    const     uint8_t rotaryIndex     = findRotary(&rotary);

    if(rotaryIndex == rotaryNotFound) {
        return false;
    }
    else {
        rotaries[rotaryIndex] = nullptr;
    }
    if(clkPinInterrupt != notAnInterrupt) {
        ::detachInterrupt(clkPinInterrupt);
    }
    if(dirPinInterrupt != notAnInterrupt) {
        ::detachInterrupt(dirPinInterrupt);
    }
    if(btnPinInterrupt != notAnInterrupt) {
        ::detachInterrupt(btnPinInterrupt);
    }
    return true;
}

}

// ---------------------------------------------------------------------------
// Rotary
// ---------------------------------------------------------------------------

Rotary::Rotary ( const uint8_t clkPin
               , const uint8_t dirPin
               , const uint8_t btnPin )
    : _wiring { clkPin
              , dirPin
              , btnPin }
    , _values { Traits::Default::COUNT
              , Traits::Default::STATE }
{
}

void Rotary::begin()
{
    /* setup */ {
        ::pinMode(_wiring.clkPin, Traits::Setup::CLK_PINMODE);
        ::pinMode(_wiring.dirPin, Traits::Setup::DIR_PINMODE);
        ::pinMode(_wiring.btnPin, Traits::Setup::BTN_PINMODE);
    }
    /* reset */ {
        reset();
    }
    /* attach to interrupt */ {
        (void) attachRotaryToAnInterrupt(*this, _wiring);
    }
}

void Rotary::reset()
{
    /* reset values */ {
        _values.count = Traits::Default::COUNT;
        _values.state = Traits::Default::STATE;
    }
}

void Rotary::end()
{
    /* detach from interrupt */ {
        (void) detachRotaryFromInterrupt(*this, _wiring);
    }
    /* reset */ {
        reset();
    }
}

bool Rotary::read()
{
    const uint8_t old_state(_values.state);
    const uint8_t new_state(_values.state = readState());

    if(new_state != old_state) {
        const bool old_clk = ((old_state & Traits::State::CLK_BITMASK) != 0);
        const bool new_clk = ((new_state & Traits::State::CLK_BITMASK) != 0);
        const bool old_dir = ((old_state & Traits::State::DIR_BITMASK) != 0);
        const bool new_dir = ((new_state & Traits::State::DIR_BITMASK) != 0);
        /* gray code, only one pin can change at a time */ {
            if((old_clk != new_clk) && (old_dir != new_dir)) {
                return false;
            }
        }
        /* increment or decrement count */ {
            if((old_clk == false) && (new_clk != false)) {
                if(new_dir == new_clk) {
                    ++_values.count;
                }
                else {
                    --_values.count;
                }
            }
        }
        return true;
    }
    return false;
}

uint8_t Rotary::readState() const
{
    const uint8_t state = (::digitalRead(_wiring.clkPin) == 0 ? Traits::State::CLK_BITMASK : 0)
                        | (::digitalRead(_wiring.dirPin) == 0 ? Traits::State::DIR_BITMASK : 0)
                        | (::digitalRead(_wiring.btnPin) == 0 ? Traits::State::BTN_BITMASK : 0)
                        ;
    return state;
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
