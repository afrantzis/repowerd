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

#include <functional>
#include <limits>

namespace repowerd
{

class AlarmId
{
public:
    AlarmId() : AlarmId{invalid} {}
    AlarmId(int id) : id{id} {}

    AlarmId operator++(int)
    {
        if (id == std::numeric_limits<decltype(id)>::max())
        {
            auto const old_id = id;
            id = 0;
            return AlarmId{old_id};
        }
        else
        {
            return AlarmId{id++};
        }
    }
    operator int() const { return id; }

    static int constexpr invalid{-1};

private:
    int id;
};

}

namespace std
{

template <> struct hash<repowerd::AlarmId>
{
    size_t operator()(repowerd::AlarmId const& x) const
    {
        return std::hash<int>{}(static_cast<int>(x));
    }
};

}
