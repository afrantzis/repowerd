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

#include "fake_logind.h"
#include <algorithm>

#include <gio/gunixfdlist.h>
#include <fcntl.h>

namespace rt = repowerd::test;
using namespace std::literals::string_literals;

namespace
{

char const* const logind_introspection = R"(<!DOCTYPE node PUBLIC '-//freedesktop//DTD D-BUS Object Introspection 1.0//EN' 'http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd'>
<node>
    <interface name='org.freedesktop.login1.Manager'>
        <method name='GetSessionByPID'>
            <arg name='pid' type='u'/>
            <arg name='session' type='o' direction='out'/>
        </method>
        <method name='Inhibit'>
            <arg name='what' type='s'/>
            <arg name='who' type='s'/>
            <arg name='why' type='s'/>
            <arg name='mode' type='s'/>
            <arg name='fd' type='h' direction='out'/>
        </method>
        <method name='PowerOff'>
            <arg name='interactive' type='b'/>
        </method>
        <method name='Suspend'>
            <arg name='interactive' type='b'/>
        </method>
        <signal name='SessionAdded'>
            <arg name='name' type='s'/>
            <arg name='path' type='o'/>
        </signal>
        <signal name='SessionRemoved'>
            <arg name='name' type='s'/>
            <arg name='path' type='o'/>
        </signal>
        <signal name='PrepareForSleep'>
            <arg name='start' type='b'/>
        </signal>
        <property type='s' name='BlockInhibited' access='read'/>
    </interface>
</node>)";

char const* const logind_seat_introspection = R"(<!DOCTYPE node PUBLIC '-//freedesktop//DTD D-BUS Object Introspection 1.0//EN' 'http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd'>
<node>
    <interface name='org.freedesktop.login1.Seat'>
        <property type='(so)' name='ActiveSession' access='read'/>
    </interface>
</node>)";

char const* const logind_session_introspection = R"(<!DOCTYPE node PUBLIC '-//freedesktop//DTD D-BUS Object Introspection 1.0//EN' 'http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd'>
<node>
    <interface name='org.freedesktop.login1.Session'>
        <property type='s' name='Type' access='read'/>
        <property type='(uo)' name='User' access='read'/>
    </interface>
</node>)";

char const* const seat_path = "/org/freedesktop/login1/seat/seat0";

std::string session_path_for_id(std::string const& session_id)
{
    return "/org/freedesktop/login1/session/" + session_id;
}

std::string session_id_for_path(std::string const& session_path)
{
    return session_path.substr(session_path.rfind("/") + 1);
}

}

rt::FakeLogind::FakeLogind(
    std::string const& dbus_address)
    : rt::DBusClient{dbus_address, "org.freedesktop.login1", "/org/freedesktop/login1"}
{
    connection.request_name("org.freedesktop.login1");

    logind_handler_registration = event_loop.register_object_handler(
        connection,
        "/org/freedesktop/login1",
        logind_introspection,
        [this] (
            GDBusConnection* connection,
            gchar const* sender,
            gchar const* object_path,
            gchar const* interface_name,
            gchar const* method_name,
            GVariant* parameters,
            GDBusMethodInvocation* invocation)
        {
            dbus_method_call(
                connection, sender, object_path, interface_name,
                method_name, parameters, invocation);
        });

    logind_seat_handler_registration = event_loop.register_object_handler(
        connection,
        seat_path,
        logind_seat_introspection,
        [this] (
            GDBusConnection* connection,
            gchar const* sender,
            gchar const* object_path,
            gchar const* interface_name,
            gchar const* method_name,
            GVariant* parameters,
            GDBusMethodInvocation* invocation)
        {
            dbus_method_call(
                connection, sender, object_path, interface_name,
                method_name, parameters, invocation);
        });
}

void rt::FakeLogind::add_session(
    std::string const& session_id, std::string const& session_type, pid_t pid, uid_t uid)
{
    {
        std::lock_guard<std::mutex> lock{sessions_mutex};
        sessions[session_id] = {session_type, pid, uid};
    }

    auto const session_path = session_path_for_id(session_id);

    session_handler_registrations[session_id] = event_loop.register_object_handler(
        connection,
        session_path.c_str(),
        logind_session_introspection,
        [this] (
            GDBusConnection* connection,
            gchar const* sender,
            gchar const* object_path,
            gchar const* interface_name,
            gchar const* method_name,
            GVariant* parameters,
            GDBusMethodInvocation* invocation)
        {
            dbus_method_call(
                connection, sender, object_path, interface_name,
                method_name, parameters, invocation);
        });

    auto const params =
        g_variant_new_parsed("(@s %s, @o %o)",
            session_id.c_str(),session_path.c_str());

    emit_signal_full(
        "/org/freedesktop/login1",
        "org.freedesktop.login1.Manager",
        "SessionAdded", params);
}

void rt::FakeLogind::remove_session(std::string const& session_id)
{
    {
        std::lock_guard<std::mutex> lock{sessions_mutex};
        sessions.erase(session_id);
    }

    session_handler_registrations.erase(session_id);

    auto const session_path = session_path_for_id(session_id);

    auto const params =
        g_variant_new_parsed("(@s %s, @o %o)",
            session_id.c_str(), session_path.c_str());

    emit_signal_full(
        "/org/freedesktop/login1",
        "org.freedesktop.login1.Manager",
        "SessionRemoved", params);
}

void rt::FakeLogind::activate_session(std::string const& session_id)
{
    {
        std::lock_guard<std::mutex> lock{sessions_mutex};

        if (sessions.find(session_id) == sessions.end())
            throw std::runtime_error("Cannot activate non-existent session " + session_id);

        active_session_id = session_id;
    }

    auto const session_path = session_path_for_id(session_id);

    auto const changed_properties_str =
        "'ActiveSession': <(@s '"s + session_id + "', @o '" + session_path + "')>";
    auto const params_str =
        "(@s 'org.freedesktop.login1.Seat',"s +
        " @a{sv} {" + changed_properties_str + "}," +
        " @as [])";

    auto const params = g_variant_new_parsed(params_str.c_str());

    emit_signal_full(
        seat_path,
        "org.freedesktop.DBus.Properties",
        "PropertiesChanged", params);
}

void rt::FakeLogind::deactivate_session()
{
    auto const changed_properties_str =
        "'ActiveSession': <(@s '', @o '/')>";
    auto const params_str =
        "(@s 'org.freedesktop.login1.Seat',"s +
        " @a{sv} {" + changed_properties_str + "}," +
        " @as [])";

    auto const params = g_variant_new_parsed(params_str.c_str());

    emit_signal_full(
        seat_path,
        "org.freedesktop.DBus.Properties",
        "PropertiesChanged", params);
}

std::unordered_set<std::string> rt::FakeLogind::active_inhibitions()
{
    std::lock_guard<std::mutex> lock{sessions_mutex};

    std::unordered_set<std::string> ret;

    for (auto iter = inhibitions.begin(); iter != inhibitions.end(); )
    {
        char c = 0;
        if (read(iter->second, &c, 1) == 0)
        {
            iter = inhibitions.erase(iter);
        }
        else
        {
            ret.insert(iter->first);
            ++iter;
        }
    }

    return ret;
}

std::string rt::FakeLogind::power_requests()
{
    std::lock_guard<std::mutex> lock{sessions_mutex};

    return power_requests_;
}

void rt::FakeLogind::emit_prepare_for_sleep(bool start)
{
    gboolean const value{start ? TRUE : FALSE};
    auto const params = g_variant_new_parsed("(%b,)", value);

    emit_signal_full(
        "/org/freedesktop/login1",
        "org.freedesktop.login1.Manager",
        "PrepareForSleep", params);
}

void rt::FakeLogind::set_block_inhibited(std::string const& blocks)
{
    block_inhibited = blocks;

    auto const changed_properties_str =
        "'BlockInhibited': <@s '"s + blocks + "'>";
    auto const params_str =
        "(@s 'org.freedesktop.login1.Manager',"s +
        " @a{sv} {" + changed_properties_str + "}," +
        " @as [])";

    auto const params = g_variant_new_parsed(params_str.c_str());

    emit_signal_full(
        "/org/freedesktop/login1",
        "org.freedesktop.DBus.Properties",
        "PropertiesChanged", params);
}

void rt::FakeLogind::dbus_method_call(
    GDBusConnection* /*connection*/,
    gchar const* /*sender_cstr*/,
    gchar const* object_path_cstr,
    gchar const* interface_name_cstr,
    gchar const* method_name_cstr,
    GVariant* parameters,
    GDBusMethodInvocation* invocation)
{
    std::string const object_path{object_path_cstr ? object_path_cstr : ""};
    std::string const method_name{method_name_cstr ? method_name_cstr : ""};
    std::string const interface_name{interface_name_cstr ? interface_name_cstr : ""};

    if (interface_name == "org.freedesktop.DBus.Properties" &&
        method_name == "Get" &&
        object_path == seat_path)
    {
        std::string local_active_session_id;
        {
            std::lock_guard<std::mutex> lock{sessions_mutex};
            local_active_session_id = active_session_id;
        }

        auto const local_active_session_path = session_path_for_id(local_active_session_id);

        auto const properties =
            g_variant_new_parsed("(<(@s %s, @o %o)>,)",
                local_active_session_id.c_str(), local_active_session_path.c_str());

        g_dbus_method_invocation_return_value(invocation, properties);
    }
    else if (interface_name == "org.freedesktop.DBus.Properties" &&
             method_name == "Get" &&
             sessions.find(session_id_for_path(object_path)) != sessions.end())
    {
        char const* property_name_cstr{""};
        g_variant_get(parameters, "(&s&s)", nullptr, &property_name_cstr);
        std::string const property_name{property_name_cstr ? property_name_cstr : ""};

        GVariant* property = nullptr;
        {
            std::lock_guard<std::mutex> lock{sessions_mutex};
            auto const iter = sessions.find(session_id_for_path(object_path));
            if (iter != sessions.end())
            {
                if (property_name == "Type")
                {
                    property = g_variant_new_parsed(
                        "(<%s>,)",
                        iter->second.type.c_str());
                }
                else if (property_name == "User")
                {
                    auto const user_path =
                        "/org/freedesktop/login1/user/_" + std::to_string(iter->second.uid);
                    property = g_variant_new_parsed(
                        "(<(@u %u, @o %o)>,)",
                        iter->second.uid,
                        user_path.c_str());
                }
            }
        }

        g_dbus_method_invocation_return_value(invocation, property);
    }
    else if (interface_name == "org.freedesktop.DBus.Properties" &&
             method_name == "Get" &&
             object_path == "/org/freedesktop/login1")
    {
        char const* property_name_cstr{""};
        g_variant_get(parameters, "(&s&s)", nullptr, &property_name_cstr);
        std::string const property_name{property_name_cstr ? property_name_cstr : ""};

        GVariant* property = nullptr;
        if (property_name == "BlockInhibited")
        {
            property = g_variant_new_parsed(
                "(<%s>,)", block_inhibited.c_str());
        }

        g_dbus_method_invocation_return_value(invocation, property);
    }
    else if (interface_name == "org.freedesktop.login1.Manager" &&
             method_name == "GetSessionByPID")
    {
        guint gpid = -1;
        g_variant_get(parameters, "(u)", &gpid);
        pid_t const pid = gpid;

        std::string session_path;
        {
            std::lock_guard<std::mutex> lock{sessions_mutex};
            auto const iter = std::find_if(
                sessions.begin(), sessions.end(),
                [pid] (auto const& kv) { return kv.second.pid == pid; });
            if (iter != sessions.end())
                session_path = session_path_for_id(iter->first);
        }

        if (session_path.empty())
        {
            g_dbus_method_invocation_return_error_literal(
                invocation, G_DBUS_ERROR, G_DBUS_ERROR_UNIX_PROCESS_ID_UNKNOWN, "");
        }
        else
        {
            auto const session_variant =
                g_variant_new_parsed("(%o,)", session_path.c_str());

            g_dbus_method_invocation_return_value(invocation, session_variant);
        }
    }
    else if (interface_name == "org.freedesktop.login1.Manager" &&
             method_name == "Inhibit")
    {
        char const* what_cstr = nullptr;
        char const* who_cstr = nullptr;
        char const* why_cstr = nullptr;
        char const* mode_cstr = nullptr;

        g_variant_get(parameters, "(&s&s&s&s)",
                      &what_cstr, &who_cstr, &why_cstr, &mode_cstr);

        int pipefd[2];
        if (pipe2(pipefd, O_CLOEXEC | O_NONBLOCK) == -1)
            pipefd[0] = pipefd[1] = -1;

        auto inhibition_id =
            std::string{what_cstr} + "," + who_cstr + "," + why_cstr + "," + mode_cstr;

        {
            std::lock_guard<std::mutex> lock{sessions_mutex};
            inhibitions.emplace(inhibition_id, Fd{pipefd[0]});
        }

        auto const reply = g_variant_new_parsed("(@h 0,)");
        auto const fd_list = g_unix_fd_list_new_from_array(pipefd + 1, 1);

        g_dbus_method_invocation_return_value_with_unix_fd_list(
            invocation, reply, fd_list);

        g_object_unref(fd_list);
    }
    else if (interface_name == "org.freedesktop.login1.Manager" &&
             method_name == "PowerOff")
    {
        gboolean interactive = TRUE;
        g_variant_get(parameters, "(b)", &interactive);

        {
            std::lock_guard<std::mutex> lock{sessions_mutex};
            power_requests_.append(interactive ? "[power-off:t]" : "[power-off:f]");
        }

        g_dbus_method_invocation_return_value(invocation, nullptr);
    }
    else if (interface_name == "org.freedesktop.login1.Manager" &&
             method_name == "Suspend")
    {
        gboolean interactive = TRUE;
        g_variant_get(parameters, "(b)", &interactive);

        {
            std::lock_guard<std::mutex> lock{sessions_mutex};
            power_requests_.append(interactive ? "[suspend:t]" : "[suspend:f]");
        }

        g_dbus_method_invocation_return_value(invocation, nullptr);
    }
    else
    {
        g_dbus_method_invocation_return_error_literal(
            invocation, G_DBUS_ERROR, G_DBUS_ERROR_NOT_SUPPORTED, "");
    }
}
