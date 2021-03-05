/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of room-raider
 * Created on: 15 февр. 2021 г.
 *
 * room-raider is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * room-raider is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with room-raider. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/test-fw/mtest.h>
#include <lsp-plug.in/lltl/parray.h>
#include <private/tool.h>

MTEST_BEGIN("room_raider", main)

    MTEST_MAIN
    {
        lltl::parray<char> args;
        args.add(const_cast<char *>(full_name()));

        for (ssize_t i=0; i < argc; ++i)
            args.add(const_cast<char *>(argv[i]));

        room_raider::main(args.size(), const_cast<const char **>(args.array()));
    }

MTEST_END



