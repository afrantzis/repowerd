# Copyright © 2016 Canonical Ltd.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Authored by: Alexandros Frantzis <alexandros.frantzis@canonical.com>

add_executable(
    repowerd-adapter-tests

    dbus_bus.cpp
    dbus_client.cpp
    fake_android_properties.cpp
    fake_brightness_notification.cpp
    fake_chrono.cpp
    fake_device_config.cpp
    fake_device_quirks.cpp
    fake_filesystem.cpp
    fake_libhardware.cpp
    fake_ofono.cpp
    fake_upower.cpp
    fake_wakeup_service.cpp
    run_command.cpp
    temporary_environment_value.cpp
    temporary_file.cpp
    unity_screen_dbus_client.cpp

    test_android_backlight.cpp
    test_android_autobrightness_algorithm.cpp
    test_android_device_config.cpp
    test_backlight_brightness_control.cpp
    test_brightness_params.cpp
    test_dev_alarm_wakeup_service.cpp
    test_event_loop_timer.cpp
    test_monotone_spline.cpp
    test_ofono_voice_call_service.cpp
    test_path.cpp
    test_powerd_service.cpp
    test_real_chrono.cpp
    test_real_filesystem.cpp
    test_real_temporary_suspend_inhibition.cpp
    test_sysfs_backlight.cpp
    test_ubuntu_light_sensor.cpp
    test_ubuntu_proximity_sensor.cpp
    test_unity_display_power_control.cpp
    test_unity_power_button.cpp
    test_unity_screen_service.cpp
    test_unity_user_activity.cpp
    test_upower_power_source.cpp
)

target_link_libraries(
    repowerd-adapter-tests

    repowerd-core
    repowerd-adapters
    repowerd-test-common

    ${GTEST_BOTH_LIBRARIES}
    ${GMOCK_LIBRARY}
    ${GMOCK_MAIN_LIBRARY}
)

if (REPOWERD_DISABLE_TIME_SENSITIVE_TESTS)
    set(ADAPTER_TESTS_FILTER "${ADAPTER_TESTS_FILTER}:ARealChrono.*:AnEventLoopTimer.*:ARealTemporarySuspendInhibition.*")
endif()

add_test(
    repowerd-adapter-tests
    ${EXECUTABLE_OUTPUT_PATH}/repowerd-adapter-tests --gtest_filter=-${ADAPTER_TESTS_FILTER}
)

add_dependencies(repowerd-adapter-tests GMock)
