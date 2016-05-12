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

#include "src/core/brightness_control.h"
#include "brightness_notification.h"
#include "event_loop.h"

#include <memory>

namespace repowerd
{

class AutobrightnessAlgorithm;
class Backlight;
class DeviceConfig;
class LightSensor;

class BacklightBrightnessControl : public BrightnessControl, public BrightnessNotification
{
public:
    BacklightBrightnessControl(
        std::shared_ptr<Backlight> const& backlight,
        std::shared_ptr<LightSensor> const& light_sensor,
        std::shared_ptr<AutobrightnessAlgorithm> const& autobrightness_algorithm,
        DeviceConfig const& device_config);

    void disable_autobrightness() override;
    void enable_autobrightness() override;
    void set_dim_brightness() override;
    void set_normal_brightness() override;
    void set_normal_brightness_value(float) override;
    void set_off_brightness() override;

    HandlerRegistration register_brightness_handler(
        BrightnessHandler const& handler) override;

    void sync();

private:
    enum TransitionSpeed {normal, slow};
    void transition_to_brightness_value(float brightness, TransitionSpeed transition_speed);
    void set_brightness_value(float brightness);
    float get_brightness_value();

    std::shared_ptr<Backlight> const backlight;
    std::shared_ptr<LightSensor> const light_sensor;
    std::shared_ptr<AutobrightnessAlgorithm> autobrightness_algorithm;
    bool const ab_supported;

    EventLoop event_loop;
    HandlerRegistration light_handler_registration;
    HandlerRegistration ab_handler_registration;
    BrightnessHandler brightness_handler;

    float dim_brightness;
    float normal_brightness;
    float user_normal_brightness;
    bool normal_brightness_active;
    bool ab_active;
};

}