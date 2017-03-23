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

#include "fake_client_requests.h"

namespace rt = repowerd::test;

namespace
{
auto null_arg_handler = [](auto){};
auto null_arg2_handler = [](auto,auto){};
}

rt::FakeClientRequests::FakeClientRequests()
    : disable_inactivity_timeout_handler{null_arg2_handler},
      enable_inactivity_timeout_handler{null_arg2_handler},
      set_inactivity_timeout_handler{null_arg2_handler},
      disable_autobrightness_handler{null_arg_handler},
      enable_autobrightness_handler{null_arg_handler},
      set_normal_brightness_value_handler{null_arg2_handler}
{
}

void rt::FakeClientRequests::start_processing()
{
    mock.start_processing();
}

repowerd::HandlerRegistration rt::FakeClientRequests::register_disable_inactivity_timeout_handler(
    DisableInactivityTimeoutHandler const& handler)
{
    mock.register_disable_inactivity_timeout_handler(handler);
    disable_inactivity_timeout_handler = handler;
    return HandlerRegistration{
        [this]
        {
            mock.unregister_disable_inactivity_timeout_handler();
            disable_inactivity_timeout_handler = null_arg2_handler;
        }};
}

repowerd::HandlerRegistration rt::FakeClientRequests::register_enable_inactivity_timeout_handler(
    EnableInactivityTimeoutHandler const& handler)
{
    mock.register_enable_inactivity_timeout_handler(handler);
    enable_inactivity_timeout_handler = handler;
    return HandlerRegistration{
        [this]
        {
            mock.unregister_enable_inactivity_timeout_handler();
            enable_inactivity_timeout_handler = null_arg2_handler;
        }};
}

repowerd::HandlerRegistration rt::FakeClientRequests::register_set_inactivity_timeout_handler(
    SetInactivityTimeoutHandler const& handler)
{
    mock.register_set_inactivity_timeout_handler(handler);
    set_inactivity_timeout_handler = handler;
    return HandlerRegistration{
        [this]
        {
            mock.unregister_set_inactivity_timeout_handler();
            set_inactivity_timeout_handler = null_arg2_handler;
        }};
}

repowerd::HandlerRegistration rt::FakeClientRequests::register_disable_autobrightness_handler(
    DisableAutobrightnessHandler const& handler)
{
    mock.register_disable_autobrightness_handler(handler);
    disable_autobrightness_handler = handler;
    return HandlerRegistration{
        [this]
        {
            mock.unregister_disable_autobrightness_handler();
            disable_autobrightness_handler = null_arg_handler;
        }};
}

repowerd::HandlerRegistration rt::FakeClientRequests::register_enable_autobrightness_handler(
    EnableAutobrightnessHandler const& handler)
{
    mock.register_enable_autobrightness_handler(handler);
    enable_autobrightness_handler = handler;
    return HandlerRegistration{
        [this]
        {
            mock.unregister_enable_autobrightness_handler();
            enable_autobrightness_handler = null_arg_handler;
        }};
}

repowerd::HandlerRegistration rt::FakeClientRequests::register_set_normal_brightness_value_handler(
    SetNormalBrightnessValueHandler const& handler)
{
    mock.register_set_normal_brightness_value_handler(handler);
    set_normal_brightness_value_handler = handler;
    return HandlerRegistration{
        [this]
        {
            mock.unregister_set_normal_brightness_value_handler();
            set_normal_brightness_value_handler = null_arg2_handler;
        }};
}

repowerd::HandlerRegistration rt::FakeClientRequests::register_allow_suspend_handler(
    AllowSuspendHandler const& handler)
{
    mock.register_allow_suspend_handler(handler);
    allow_suspend_handler = handler;
    return HandlerRegistration{
        [this]
        {
            mock.unregister_allow_suspend_handler();
            allow_suspend_handler = null_arg2_handler;
        }};
}

repowerd::HandlerRegistration rt::FakeClientRequests::register_disallow_suspend_handler(
    DisallowSuspendHandler const& handler)
{
    mock.register_disallow_suspend_handler(handler);
    disallow_suspend_handler = handler;
    return HandlerRegistration{
        [this]
        {
            mock.unregister_disallow_suspend_handler();
            disallow_suspend_handler = null_arg2_handler;
        }};
}

void rt::FakeClientRequests::emit_disable_inactivity_timeout(
    std::string const& id, pid_t pid)
{
    disable_inactivity_timeout_handler(id, pid);
}

void rt::FakeClientRequests::emit_enable_inactivity_timeout(
    std::string const& id, pid_t pid)
{
    enable_inactivity_timeout_handler(id, pid);
}

void rt::FakeClientRequests::emit_set_inactivity_timeout(
    std::chrono::milliseconds timeout, pid_t pid)
{
    set_inactivity_timeout_handler(timeout, pid);
}

void rt::FakeClientRequests::emit_disable_autobrightness(pid_t pid)
{
    disable_autobrightness_handler(pid);
}

void rt::FakeClientRequests::emit_enable_autobrightness(pid_t pid)
{
    enable_autobrightness_handler(pid);
}

void rt::FakeClientRequests::emit_set_normal_brightness_value(double f, pid_t pid)
{
    set_normal_brightness_value_handler(f, pid);
}

void rt::FakeClientRequests::emit_allow_suspend(
    std::string const& id, pid_t pid)
{
    allow_suspend_handler(id, pid);
}

void rt::FakeClientRequests::emit_disallow_suspend(
    std::string const& id, pid_t pid)
{
    disallow_suspend_handler(id, pid);
}
