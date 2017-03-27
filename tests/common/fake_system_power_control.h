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

#include "src/core/system_power_control.h"

#include <gmock/gmock.h>

#include <unordered_set>
#include <mutex>

namespace repowerd
{
namespace test
{

class FakeSystemPowerControl : public SystemPowerControl
{
public:
    FakeSystemPowerControl();

    void start_processing() override;

    HandlerRegistration register_system_resume_handler(
        SystemResumeHandler const& systemd_resume_handler) override;

    HandlerRegistration register_system_allow_suspend_handler(
        SystemAllowSuspendHandler const& system_allow_suspend_handler) override;
    HandlerRegistration register_system_disallow_suspend_handler(
        SystemDisallowSuspendHandler const& system_disallow_suspend_handler) override;

    void allow_automatic_suspend(std::string const& id) override;
    void disallow_automatic_suspend(std::string const& id) override;

    void power_off() override;
    void suspend() override;

    void allow_default_system_handlers() override;
    void disallow_default_system_handlers() override;

    bool is_automatic_suspend_allowed();
    bool are_default_system_handlers_allowed();

    void emit_system_resume();
    void emit_system_allow_suspend();
    void emit_system_disallow_suspend();

    struct MockMethods
    {
        MOCK_METHOD0(start_processing, void());
        MOCK_METHOD1(register_system_resume_handler, void(SystemResumeHandler const&));
        MOCK_METHOD0(unregister_system_resume_handler, void());
        MOCK_METHOD1(register_system_allow_suspend_handler,
                     void(SystemAllowSuspendHandler const&));
        MOCK_METHOD0(unregister_system_allow_suspend_handler, void());
        MOCK_METHOD1(register_system_disallow_suspend_handler,
                     void(SystemDisallowSuspendHandler const&));
        MOCK_METHOD0(unregister_system_disallow_suspend_handler, void());
        MOCK_METHOD1(allow_automatic_suspend, void(std::string const&));
        MOCK_METHOD1(disallow_automatic_suspend, void(std::string const&));
        MOCK_METHOD0(power_off, void());
        MOCK_METHOD0(suspend, void());
        MOCK_METHOD0(allow_default_system_handlers, void());
        MOCK_METHOD0(disallow_default_system_handlers, void());
    };
    testing::NiceMock<MockMethods> mock;

private:
    std::mutex mutex;
    std::unordered_set<std::string> automatic_suspend_disallowances;
    bool are_default_system_handlers_allowed_;
    SystemResumeHandler system_resume_handler;
    SystemAllowSuspendHandler system_allow_suspend_handler;
    SystemDisallowSuspendHandler system_disallow_suspend_handler;
};

}
}
