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
// some useful stuff
// ---------------------------------------------------------------------------

namespace {

Rotary* volatile rotary0 = nullptr;
Rotary* volatile rotary1 = nullptr;
Rotary* volatile rotary2 = nullptr;
Rotary* volatile rotary3 = nullptr;

typedef void (*RotaryHandlerProc)(void);

void rotaryHandler0(void)
{
    if(rotary0 != nullptr) {
        rotary0->read();
    }
}

void rotaryHandler1(void)
{
    if(rotary1 != nullptr) {
        rotary1->read();
    }
}

void rotaryHandler2(void)
{
    if(rotary2 != nullptr) {
        rotary2->read();
    }
}

void rotaryHandler3(void)
{
    if(rotary3 != nullptr) {
        rotary3->read();
    }
}

uint8_t findRotary(Rotary* rotary)
{
    if(rotary0 == rotary) return 0;
    if(rotary1 == rotary) return 1;
    if(rotary2 == rotary) return 2;
    if(rotary3 == rotary) return 3;

    return static_cast<uint8_t>(-1);
}

void attachRotaryToAnInterrupt(Rotary& rotary, const Rotary_Wiring& wiring)
{
    constexpr uint8_t notAnInterrupt  = static_cast<uint8_t>(NOT_AN_INTERRUPT);
    const     uint8_t clkPinInterrupt = digitalPinToInterrupt(wiring.clkPin);
    const     uint8_t dirPinInterrupt = digitalPinToInterrupt(wiring.dirPin);
    const     uint8_t btnPinInterrupt = digitalPinToInterrupt(wiring.btnPin);
    const     uint8_t rotaryIndex     = findRotary(nullptr);
    RotaryHandlerProc rotaryHandler   = nullptr;

    if((clkPinInterrupt == notAnInterrupt)
    && (dirPinInterrupt == notAnInterrupt)
    && (btnPinInterrupt == notAnInterrupt)) {
        return;
    }
    switch(rotaryIndex) {
        case 0:
            rotary0       = &rotary;
            rotaryHandler = &rotaryHandler0;
            break;
        case 1:
            rotary1       = &rotary;
            rotaryHandler = &rotaryHandler1;
            break;
        case 2:
            rotary2       = &rotary;
            rotaryHandler = &rotaryHandler2;
            break;
        case 3:
            rotary3       = &rotary;
            rotaryHandler = &rotaryHandler3;
            break;
        default:
            break;
    }
    if((clkPinInterrupt != notAnInterrupt) && (rotaryHandler != nullptr)) {
        ::attachInterrupt(clkPinInterrupt, rotaryHandler, CHANGE);
    }
    if((dirPinInterrupt != notAnInterrupt) && (rotaryHandler != nullptr)) {
        ::attachInterrupt(dirPinInterrupt, rotaryHandler, CHANGE);
    }
    if((btnPinInterrupt != notAnInterrupt) && (rotaryHandler != nullptr)) {
        ::attachInterrupt(btnPinInterrupt, rotaryHandler, CHANGE);
    }
}

void detachRotaryFromInterrupt(Rotary& rotary, const Rotary_Wiring& wiring)
{
    constexpr uint8_t notAnInterrupt  = static_cast<uint8_t>(NOT_AN_INTERRUPT);
    const     uint8_t clkPinInterrupt = digitalPinToInterrupt(wiring.clkPin);
    const     uint8_t dirPinInterrupt = digitalPinToInterrupt(wiring.dirPin);
    const     uint8_t btnPinInterrupt = digitalPinToInterrupt(wiring.btnPin);
    const     uint8_t rotaryIndex     = findRotary(&rotary);
    RotaryHandlerProc rotaryHandler   = nullptr;

    switch(rotaryIndex) {
        case 0:
            rotary0       = nullptr;
            rotaryHandler = &rotaryHandler0;
            break;
        case 1:
            rotary1       = nullptr;
            rotaryHandler = &rotaryHandler1;
            break;
        case 2:
            rotary2       = nullptr;
            rotaryHandler = &rotaryHandler2;
            break;
        case 3:
            rotary3       = nullptr;
            rotaryHandler = &rotaryHandler3;
            break;
        default:
            break;
    }
    if((clkPinInterrupt != notAnInterrupt) && (rotaryHandler != nullptr)) {
        ::detachInterrupt(clkPinInterrupt);
    }
    if((dirPinInterrupt != notAnInterrupt) && (rotaryHandler != nullptr)) {
        ::detachInterrupt(dirPinInterrupt);
    }
    if((btnPinInterrupt != notAnInterrupt) && (rotaryHandler != nullptr)) {
        ::detachInterrupt(btnPinInterrupt);
    }
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
        attachRotaryToAnInterrupt(*this, _wiring);
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
        detachRotaryFromInterrupt(*this, _wiring);
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
