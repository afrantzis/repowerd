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

#include "src/default_daemon_config.h"
#include "src/adapters/wakeup_service.h"

#include <chrono>
#include <cstdlib>
#include <vector>
#include <future>
#include <iostream>

std::vector<int> parse_wakeups(int argc, char** argv)
{
    std::vector<int> wakeups;

    for (auto i = 1; i < argc; ++i)
    {
        wakeups.push_back(std::stoi(argv[i]));
    }

    return wakeups;
}

void print_wakeups(std::vector<int> const& wakeups)
{
    std::cout << "Wakeups @ ";
    for (auto const& w : wakeups)
        std::cout << w << " ";
    std::cout << std::endl;
}

void print_wakeup_event(std::string const& cookie)
{
    std::cout << "Wakeup with cookie '" << cookie << "'" << std::endl;
}

void print_usage(std::string const& progname)
{
    std::cout << "Usage: " << progname << " <wakeups-in-seconds>" << std::endl;
    std::cout << "Schedules and waits for hardware wakeups" << std::endl;
    std::cout << "Example: " << progname << " 1 3 5" << std::endl;
}

int main(int argc, char** argv)
{
    setenv("REPOWERD_LOG", "console", 1);

    repowerd::DefaultDaemonConfig config;
    auto const wakeup_service = config.the_wakeup_service();

    auto const wakeups = parse_wakeups(argc, argv);

    if (wakeups.empty())
    {
        print_usage(argv[0]);
        return 1;
    }

    print_wakeups(wakeups);

    std::promise<void> done;
    auto done_future = done.get_future();

    auto wakeup_count = 0u;
    auto registration = wakeup_service->register_wakeup_handler(
        [&] (std::string const& cookie)
        { 
            print_wakeup_event(cookie);
            if (++wakeup_count == wakeups.size())
                done.set_value();
        });

    for (auto const& w : wakeups)
    {
        wakeup_service->schedule_wakeup_at(
            std::chrono::system_clock::now() + std::chrono::seconds{w});
    }

    done_future.get();
}
