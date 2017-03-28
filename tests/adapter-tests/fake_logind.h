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

#include "dbus_client.h"
#include "src/adapters/fd.h"

#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <functional>

#include <sys/types.h>

namespace repowerd
{
namespace test
{

class FakeLogind : private DBusClient
{
public:
    FakeLogind(std::string const& dbus_address);

    void add_session(
        std::string const& session_path,
        std::string const& session_type,
        pid_t pid,
        uid_t uid);
    void remove_session(std::string const& session_path);
    void activate_session(std::string const& session_path);
    void deactivate_session();

    std::unordered_set<std::string> active_inhibitions();
    std::string power_requests();

    void emit_prepare_for_sleep(bool start);
    void set_block_inhibited(std::string const& blocks);

private:
    void dbus_method_call(
        GDBusConnection* connection,
        gchar const* sender_cstr,
        gchar const* object_path_cstr,
        gchar const* interface_name_cstr,
        gchar const* method_name_cstr,
        GVariant* parameters,
        GDBusMethodInvocation* invocation);

    repowerd::HandlerRegistration logind_handler_registration;
    repowerd::HandlerRegistration logind_seat_handler_registration;

    struct SessionInfo
    {
        std::string type;
        pid_t pid;
        uid_t uid;
    };

    std::mutex sessions_mutex;
    std::unordered_map<std::string,SessionInfo> sessions;
    std::unordered_map<std::string,HandlerRegistration> session_handler_registrations;
    std::string active_session_id;

    std::unordered_map<std::string,Fd> inhibitions;
    std::string power_requests_;
    std::string block_inhibited;
};

}
}
