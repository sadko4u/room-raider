/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of room-raider
 * Created on: 6 мар. 2021 г.
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

#include <private/config.h>

namespace room_raider
{
    using namespace lsp;

    config_t::config_t()
    {
        enMode          = M_NONE;
        nSampleRate     = 48000;

        fChirpDelay     = 5.0f;
        fStartFreq      = 10.0f;
        fEndFreq        = 20000.0f;
        fGain           = 0.0f;
        fSweepLength    = 20.0f;
    }

    config_t::~config_t()
    {
        clear();
    }

    void config_t::clear()
    {
        enMode          = M_NONE;
        nSampleRate     = 48000;

        fChirpDelay     = 5.0f;
        fStartFreq      = 10.0f;
        fEndFreq        = 20000.0f;
        fGain           = 0.0f;
        fSweepLength    = 20.0f;

        sInFile.clear();
        sOutFile.clear();
    }

}



