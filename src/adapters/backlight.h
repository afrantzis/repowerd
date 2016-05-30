/*
 * Copyright © 2016 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Alexandros Frantzis <alexandros.frantzis@canonical.com>
 */

#pragma once

namespace repowerd
{

class Backlight
{
public:
    virtual ~Backlight() = default;

    virtual void set_brightness(double) = 0;
    virtual double get_brightness() = 0;

    static double constexpr unknown_brightness = -1.0;

protected:
    Backlight() = default;
    Backlight(Backlight const&) = delete;
    Backlight& operator=(Backlight const&) = delete;
};

}
