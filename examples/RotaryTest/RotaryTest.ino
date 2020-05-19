/*
 * RotaryTest.ino - Copyright (c) 2014-2020 - Olivier Poncet
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
#include <Rotary.h>

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------

struct Setup
{
    static constexpr unsigned long SERIAL_SPEED   = 9600UL;
    static constexpr unsigned long LOOP_DELAY     = 1000UL;
    static constexpr uint8_t       ROTARY_CLK_PIN = 2;
    static constexpr uint8_t       ROTARY_DIR_PIN = 3;
    static constexpr uint8_t       ROTARY_BTN_PIN = 4;
};

// ---------------------------------------------------------------------------
// globals
// ---------------------------------------------------------------------------

Rotary rotary ( Setup::ROTARY_CLK_PIN
              , Setup::ROTARY_DIR_PIN
              , Setup::ROTARY_BTN_PIN );

// ---------------------------------------------------------------------------
// setup
// ---------------------------------------------------------------------------

void setup()
{
    Serial.begin(Setup::SERIAL_SPEED);
    rotary.begin();
}

// ---------------------------------------------------------------------------
// loop
// ---------------------------------------------------------------------------

void loop()
{
    Serial.print(F("rotary value : "));
    Serial.println(rotary.value());
    Serial.print(F("rotary clk   : "));
    Serial.println(rotary.clk());
    Serial.print(F("rotary dir   : "));
    Serial.println(rotary.dir());
    Serial.print(F("rotary btn   : "));
    Serial.println(rotary.btn());
    Serial.println(' ');
    ::delay(Setup::LOOP_DELAY);
}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
