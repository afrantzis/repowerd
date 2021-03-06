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

#include "src/core/display_power_control.h"
#include "src/core/log.h"

#include "dbus_connection_handle.h"

#include <memory>

namespace repowerd
{
class Log;

class UnityDisplayPowerControl : public DisplayPowerControl
{
public:
    UnityDisplayPowerControl(
        std::shared_ptr<Log> const& log,
        std::string const& dbus_bus_address);

    void turn_on() override;
    void turn_off() override;

private:
    std::shared_ptr<Log> const log;
    DBusConnectionHandle dbus_connection;
};

}

