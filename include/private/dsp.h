/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Stefano Tronci <stefano.tronci@protonmail.com>
 *
 * This file is part of room-raider
 * Created on: 13 Mar 2021
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
#ifndef PRIVATE_DSP_H_
#define PRIVATE_DSP_H_

#include <lsp-plug.in/common/status.h>
#include <private/config.h>
#include <lsp-plug.in/dsp-units/sampling/Sample.h>

namespace room_raider
{
    status_t synth_test_sweep(const config_t *cfg, dspu::Sample &out);

    status_t deconvolve(const config_t *cfg, const dspu::Sample &in, const dspu::Sample &ref, dspu::Sample &out);

    /**
     * Normalize sample to the specified gain
     * @param dst sample to normalize
     * @param gain the maximum peak gain
     * @param mode the normalization mode
     * @return status of operation
     */
    status_t normalize(dspu::Sample *dst, float gain, size_t mode);
}

#endif
