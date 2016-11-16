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

#include <gtest/gtest.h>

#include <chrono>

namespace rt = repowerd::test;

using namespace std::chrono_literals;

namespace
{

struct ANotification : rt::AcceptanceTest
{
};

}

TEST_F(ANotification, turns_on_display)
{
    expect_display_turns_on();
    emit_notification();
    verify_expectations();
}

TEST_F(ANotification, turns_off_display_after_post_notification_timeout_when_done)
{
    expect_display_turns_on();
    emit_notification();
    verify_expectations();

    expect_no_display_power_change();
    emit_notification_done();
    verify_expectations();

    expect_display_turns_off();
    advance_time_by(user_inactivity_post_notification_display_off_timeout);
}

TEST_F(ANotification, does_not_turn_on_display_if_display_is_already_on)
{
    turn_on_display();

    expect_no_display_power_change();
    emit_notification();
}

TEST_F(ANotification, keeps_display_on_if_display_is_already_on)
{
    turn_on_display();

    expect_no_display_power_change();
    emit_notification();
    advance_time_by(10h);
}

TEST_F(ANotification, does_not_dim_display_after_timeout)
{
    expect_display_turns_on();
    emit_notification();
    verify_expectations();

    expect_no_display_brightness_change();
    emit_notification_done();
    advance_time_by(user_inactivity_post_notification_display_off_timeout - 1ms);
}

TEST_F(ANotification, extends_existing_shorter_timeout)
{
    turn_on_display();
    advance_time_by(user_inactivity_normal_display_off_timeout - 1ms);

    emit_notification();
    emit_notification_done();

    expect_no_display_power_change();
    advance_time_by(1ms);
    verify_expectations();

    expect_display_turns_off();
    advance_time_by(user_inactivity_post_notification_display_off_timeout);
}

TEST_F(ANotification, does_not_reduce_existing_longer_timeout)
{
    turn_on_display();
    advance_time_by(
        user_inactivity_normal_display_off_timeout -
        user_inactivity_post_notification_display_off_timeout -
        1ms);

    emit_notification();
    emit_notification_done();

    expect_no_display_power_change();
    advance_time_by(user_inactivity_post_notification_display_off_timeout);
    verify_expectations();
}

TEST_F(ANotification, does_not_turn_on_display_when_proximity_is_near)
{
    set_proximity_state_near();

    expect_no_display_power_change();
    emit_notification();
}

TEST_F(ANotification, does_not_schedule_inactivity_timeout_when_proximity_is_near)
{
    set_proximity_state_near();

    expect_no_display_power_change();
    emit_notification();
    emit_notification_done();
    advance_time_by(user_inactivity_post_notification_display_off_timeout);
}

TEST_F(ANotification, timeout_is_extended_by_user_activity)
{
    expect_display_turns_on();
    emit_notification();
    emit_notification_done();
    verify_expectations();

    expect_no_display_power_change();
    perform_user_activity_extending_power_state();
    advance_time_by(user_inactivity_normal_display_off_timeout - 1ms);
    verify_expectations();

    expect_display_turns_off();
    advance_time_by(1ms);
}

TEST_F(ANotification, allows_power_button_to_turn_off_display)
{
    expect_display_turns_on();
    emit_notification();
    verify_expectations();

    expect_display_turns_off();
    press_power_button();
    release_power_button();
    verify_expectations();
}

TEST_F(ANotification, brightens_dim_display)
{
    turn_on_display();

    expect_display_dims();
    advance_time_by(user_inactivity_normal_display_off_timeout - 1ms);
    verify_expectations();

    expect_display_brightens();
    emit_notification();
}

TEST_F(ANotification,
       turns_off_display_when_done_if_a_client_disables_inactivity_timeout_and_the_display_was_off)
{
    expect_display_turns_on();
    emit_notification();
    verify_expectations();

    expect_no_display_power_change();
    client_request_disable_inactivity_timeout();
    emit_notification_done();
    verify_expectations();

    expect_display_turns_off();
    advance_time_by(user_inactivity_post_notification_display_off_timeout);
}

TEST_F(ANotification,
       turns_off_display_when_done_if_a_client_sets_infinite_inactivity_timeout_and_the_display_was_off)
{
    expect_display_turns_on();
    emit_notification();
    client_request_set_inactivity_timeout(infinite_timeout);
    emit_notification_done();
    verify_expectations();

    expect_display_turns_off();
    advance_time_by(user_inactivity_post_notification_display_off_timeout);
}

TEST_F(ANotification,
       turns_off_display_when_done_if_a_client_has_previously_disabled_inactivity_timeout_and_the_display_was_off)
{
    expect_display_turns_on();
    client_request_disable_inactivity_timeout();
    verify_expectations();

    turn_off_display();

    expect_display_turns_on();
    emit_notification();
    emit_notification_done();
    verify_expectations();

    expect_display_turns_off();
    advance_time_by(user_inactivity_post_notification_display_off_timeout);
}

TEST_F(ANotification,
       turns_off_display_when_done_if_a_client_has_previously_set_infinite_inactivity_timeout_and_the_display_was_off)
{
    client_request_set_inactivity_timeout(infinite_timeout);

    expect_display_turns_on();
    emit_notification();
    emit_notification_done();
    verify_expectations();

    expect_display_turns_off();
    advance_time_by(user_inactivity_post_notification_display_off_timeout);
}

TEST_F(ANotification,
       does_not_turn_off_display_when_done_if_a_client_has_disabled_inactivity_timeout_and_the_display_was_on)
{
    expect_display_turns_on();
    client_request_disable_inactivity_timeout();
    verify_expectations();

    expect_no_display_power_change();
    emit_notification();
    emit_notification_done();
    advance_time_by(user_inactivity_post_notification_display_off_timeout);
}

TEST_F(ANotification,
       does_not_turn_off_display_when_done_if_a_client_has_set_infinite_timeout_and_the_display_was_on)
{
    client_request_set_inactivity_timeout(infinite_timeout);
    turn_on_display();

    expect_no_display_power_change();
    emit_notification();
    emit_notification_done();
    advance_time_by(user_inactivity_post_notification_display_off_timeout);
}

TEST_F(ANotification, event_notifies_of_display_power_change)
{
    expect_display_power_on_notification(
        repowerd::DisplayPowerChangeReason::notification);
    emit_notification();
    verify_expectations();

    emit_notification_done();

    expect_display_power_off_notification(
        repowerd::DisplayPowerChangeReason::activity);
    advance_time_by(user_inactivity_post_notification_display_off_timeout);
}

TEST_F(ANotification, turns_display_on_for_reduced_timeout_if_proximity_uncovered)
{
    set_proximity_state_near();

    emit_notification();

    expect_display_turns_on();
    emit_proximity_state_far_if_enabled();
    emit_notification_done();
    verify_expectations();

    expect_no_display_power_change();
    advance_time_by(user_inactivity_reduced_display_off_timeout - 1ms);
    verify_expectations();

    expect_display_turns_off();
    advance_time_by(1ms);
}

TEST_F(ANotification, disables_proximity_handling_after_proximity_uncovered)
{
    expect_display_turns_on();
    set_proximity_state_near();
    emit_notification();
    emit_proximity_state_far_if_enabled();
    verify_expectations();

    expect_no_display_power_change();
    emit_proximity_state_near_if_enabled();
    emit_proximity_state_far_if_enabled();
    emit_proximity_state_near_if_enabled();
}

TEST_F(ANotification,
       does_not_disable_proximity_handling_after_proximity_uncovered_if_call_is_active)
{
    set_proximity_state_near();
    emit_active_call();
    emit_notification();
    emit_proximity_state_far_if_enabled();

    expect_display_turns_off();
    emit_proximity_state_near_if_enabled();
    verify_expectations();

    expect_display_turns_on();
    emit_proximity_state_far_if_enabled();
    verify_expectations();
}

TEST_F(ANotification,
       just_before_user_activity_timeout_does_not_turn_off_display_if_inactivity_is_disabled_and_display_was_turned_on_by_notification)
{
    expect_display_turns_on();
    client_request_disable_inactivity_timeout();
    verify_expectations();

    turn_off_display();

    expect_display_turns_on();
    emit_notification();
    expect_display_brightens();
    perform_user_activity_extending_power_state();
    emit_notification_done();
    verify_expectations();

    expect_no_display_power_change();
    advance_time_by(user_inactivity_normal_display_off_timeout - 1ms);
    emit_notification();
    emit_notification_done();
    advance_time_by(user_inactivity_post_notification_display_off_timeout);
    verify_expectations();
}

TEST_F(ANotification,
       after_user_activity_timeout_does_not_turn_off_display_if_inactivity_is_disabled_and_display_was_turned_on_by_notification)
{
    expect_display_turns_on();
    client_request_disable_inactivity_timeout();
    verify_expectations();

    turn_off_display();

    expect_display_turns_on();
    emit_notification();
    expect_display_brightens();
    perform_user_activity_extending_power_state();
    emit_notification_done();
    verify_expectations();

    expect_no_display_power_change();
    advance_time_by(user_inactivity_normal_display_off_timeout);
    emit_notification();
    emit_notification_done();
    advance_time_by(user_inactivity_post_notification_display_off_timeout);
    verify_expectations();
}

TEST_F(ANotification,
       does_not_keep_the_display_on_longer_than_the_notification_expiration_timeout)
{
    expect_display_turns_on();
    emit_notification();
    verify_expectations();

    expect_no_display_power_change();
    advance_time_by(notification_expiration_timeout - 1ms);
    verify_expectations();

    expect_display_turns_off();
    advance_time_by(1ms);
    // timer needs extra kick to apply alarm just set in previous advancement
    advance_time_by(0ms);
    verify_expectations();

    expect_no_display_power_change();
    emit_notification_done();
    verify_expectations();
}

TEST_F(ANotification,
       does_not_keep_the_display_on_longer_than_the_notification_expiration_timeout_if_inactivity_is_disabled)
{
    expect_display_turns_on();
    client_request_disable_inactivity_timeout();
    verify_expectations();

    turn_off_display();

    expect_display_turns_on();
    emit_notification();
    verify_expectations();

    expect_no_display_power_change();
    advance_time_by(notification_expiration_timeout - 1ms);
    verify_expectations();

    expect_display_turns_off();
    advance_time_by(1ms);
    // timer needs extra kick to apply alarm just set in previous advancement
    advance_time_by(0ms);
    verify_expectations();

    expect_no_display_power_change();
    emit_notification_done();
    verify_expectations();
}

TEST_F(ANotification,
       does_not_keep_the_display_on_longer_than_the_notification_expiration_timeout_if_inactivity_timeout_is_infinite)
{
    client_request_set_inactivity_timeout(infinite_timeout);

    expect_display_turns_on();
    emit_notification();
    verify_expectations();

    expect_no_display_power_change();
    advance_time_by(notification_expiration_timeout - 1ms);
    verify_expectations();

    expect_display_turns_off();
    advance_time_by(1ms);
    // timer needs extra kick to apply alarm just set in previous advancement
    advance_time_by(0ms);
    verify_expectations();

    expect_no_display_power_change();
    emit_notification_done();
    verify_expectations();
}

TEST_F(ANotification, keeps_display_on_until_all_done)
{
    std::string const id1{"id1"};
    std::string const id2{"id2"};
    std::string const id3{"id3"};

    expect_display_turns_on();
    emit_notification(id1);
    verify_expectations();

    expect_display_brightens();
    emit_notification(id2);
    verify_expectations();

    expect_display_brightens();
    emit_notification(id3);
    verify_expectations();

    expect_no_display_power_change();
    emit_notification_done(id3);
    emit_notification_done(id1);
    advance_time_by(user_inactivity_post_notification_display_off_timeout);
    verify_expectations();

    expect_display_turns_off();
    emit_notification_done(id2);
    advance_time_by(user_inactivity_post_notification_display_off_timeout);
}

TEST_F(ANotification, is_logged)
{
    emit_notification();

    EXPECT_TRUE(log_contains_line({"notification"}));
}

TEST_F(ANotification, all_done_is_logged)
{
    emit_notification();
    emit_notification_done();

    EXPECT_TRUE(log_contains_line({"no_notification"}));
}

TEST_F(ANotification, expiration_timeout_alarm_is_logged)
{
    emit_notification();
    advance_time_by(notification_expiration_timeout);

    EXPECT_TRUE(log_contains_line({"alarm", "notification", "expiration"}));
}
