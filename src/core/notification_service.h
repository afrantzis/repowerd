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

#include "handler_registration.h"

#include <functional>
#include <string>

#include <sys/types.h>

namespace repowerd
{

using NotificationHandler = std::function<void(std::string const&, pid_t pid)>;
using NotificationDoneHandler = std::function<void(std::string const&, pid_t pid)>;

class NotificationService
{
public:
    virtual ~NotificationService() = default;

    virtual void start_processing() = 0;

    virtual HandlerRegistration register_notification_handler(
        NotificationHandler const& handler) = 0;

    virtual HandlerRegistration register_notification_done_handler(
        NotificationDoneHandler const& handler) = 0;

protected:
    NotificationService() = default;
    NotificationService (NotificationService const&) = default;
    NotificationService& operator=(NotificationService const&) = default;
};

}
