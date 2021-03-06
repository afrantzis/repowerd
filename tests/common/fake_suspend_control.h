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

#include "src/core/suspend_control.h"

#include <gmock/gmock.h>

#include <unordered_set>
#include <mutex>

namespace repowerd
{
namespace test
{

class FakeSuspendControl : public SuspendControl
{
public:
    void allow_suspend(std::string const& id) override;
    void disallow_suspend(std::string const& id) override;

    bool is_suspend_allowed();

    struct MockMethods
    {
        MOCK_METHOD1(allow_suspend, void(std::string const&));
        MOCK_METHOD1(disallow_suspend, void(std::string const&));
    };
    testing::NiceMock<MockMethods> mock;

private:
    std::mutex mutex;
    std::unordered_set<std::string> suspend_disallowances;
};

}
}
