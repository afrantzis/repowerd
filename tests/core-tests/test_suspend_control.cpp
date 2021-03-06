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

#include "acceptance_test.h"
#include "fake_suspend_control.h"

#include <gtest/gtest.h>

namespace rt = repowerd::test;

namespace
{

struct ASuspendControl : rt::AcceptanceTest
{
    void expect_suspend_is_allowed()
    {
        EXPECT_TRUE(config.the_fake_suspend_control()->is_suspend_allowed());
    }

    void expect_suspend_is_disallowed()
    {
        EXPECT_FALSE(config.the_fake_suspend_control()->is_suspend_allowed());
    }

    void expect_suspend_disallow()
    {
        EXPECT_CALL(config.the_fake_suspend_control()->mock, disallow_suspend(testing::_));
    }
};

}

TEST_F(ASuspendControl, suspend_is_disallowed_when_display_turns_on)
{
    expect_suspend_is_allowed();

    turn_on_display();

    expect_suspend_is_disallowed();
}

TEST_F(ASuspendControl, suspend_is_allowed_when_display_turns_off)
{
    turn_on_display();
    turn_off_display();

    expect_suspend_is_allowed();
}

TEST_F(ASuspendControl, suspend_is_disallowed_when_display_turns_off_due_to_proximity)
{
    turn_on_display();
    emit_proximity_state_near();

    expect_suspend_is_disallowed();
}

TEST_F(ASuspendControl, suspend_is_disallowed_before_display_is_turned_on)
{
    testing::InSequence s;
    expect_suspend_disallow();
    expect_display_turns_on();

    press_power_button();
    release_power_button();
}
