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

#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/lltl/parray.h>
#include <lsp-plug.in/io/Path.h>

#include <private/config.h>
#include <private/cmdline.h>

UTEST_BEGIN("room_raider", cmdline)

    void validate_config(room_raider::config_t *cfg)
    {
        LSPString key;

        // Validate root parameters
        UTEST_ASSERT(cfg->enMode == room_raider::M_SWEEP);
        UTEST_ASSERT(float_equals_absolute(cfg->fStartFreq, 12.3f));
        UTEST_ASSERT(float_equals_absolute(cfg->fEndFreq, 20001.2f));
        UTEST_ASSERT(float_equals_absolute(cfg->fSweepLength, 12.34f));
        UTEST_ASSERT(float_equals_absolute(cfg->fChirpDelay, 4.56f));
        UTEST_ASSERT(float_equals_absolute(cfg->fGain, 0.57f));
        UTEST_ASSERT(cfg->sInFile.equals_ascii("input-file.wav"));
        UTEST_ASSERT(cfg->sOutFile.equals_ascii("output-file.wav"));
        UTEST_ASSERT(cfg->sReference.equals_ascii("reference-file.wav"));
        UTEST_ASSERT(cfg->nSampleRate == 88200);
    }

    void parse_cmdline(room_raider::config_t *cfg)
    {
        static const char *ext_argv[] =
        {
            "-s",
            "-sf",  "12.3",
            "-ef",  "20001.2",
            "-sl",  "12.34",
            "-cd",  "4.56",
            "-g",   "0.57",
            "-i",   "input-file.wav",
            "-o",   "output-file.wav",
            "-r",   "reference-file.wav",
            "-sr",  "88200",
            NULL
        };

        lltl::parray<char> argv;
        UTEST_ASSERT(argv.add(const_cast<char *>(full_name())));
        for (const char **pv = ext_argv; *pv != NULL; ++pv)
        {
            UTEST_ASSERT(argv.add(const_cast<char *>(*pv)));
        }

        status_t res = room_raider::parse_cmdline(cfg, argv.size(), const_cast<const char **>(argv.array()));
        UTEST_ASSERT(res == STATUS_OK);
    }

    UTEST_MAIN
    {
        // Parse configuration from file and cmdline
        room_raider::config_t cfg;
        parse_cmdline(&cfg);

        // Validate the final configuration
        validate_config(&cfg);
    }

UTEST_END


