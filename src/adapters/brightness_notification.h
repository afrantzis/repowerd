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

#include "src/core/handler_registration.h"

#include <functional>

namespace repowerd
{

using BrightnessHandler = std::function<void(double)>;

class BrightnessNotification
{
public:
    virtual ~BrightnessNotification() = default;

    virtual HandlerRegistration register_brightness_handler(
        BrightnessHandler const& handler) = 0;

protected:
    BrightnessNotification() = default;
    BrightnessNotification(BrightnessNotification const&) = delete;
    BrightnessNotification& operator=(BrightnessNotification const&) = delete;
};

}
