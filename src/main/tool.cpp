/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of room-raider
 * Created on: 5 мар. 2021 г.
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

#include <lsp-plug.in/common/status.h>
#include <lsp-plug.in/stdlib/stdio.h>
#include <lsp-plug.in/dsp-units/sampling/Sample.h>
#include <lsp-plug.in/dsp-units/units.h>

#include <private/tool.h>
#include <private/config.h>
#include <private/cmdline.h>

#define MIN_SAMPLE_RATE         8000
#define MAX_SAMPLE_RATE         192000

namespace room_raider
{
    using namespace lsp;

    status_t generate_sweep(config_t *cfg)
    {
        status_t res;
        dspu::Sample out;       // Sample for output

        // Initial frequency, Hz, smaller than sample_rate/2
        if ((cfg->fStartFreq*2.0f) >= cfg->nSampleRate)
        {
            fprintf(stderr, "Invalid start frequency, should be not greater than half of the sample rate\n");
            return STATUS_INVALID_VALUE;
        }

        // Final frequency, Hz: smaller than sample_rate/2 but bigger than initial frequency
        if ((cfg->fEndFreq*2.0f) >= cfg->nSampleRate)
        {
            fprintf(stderr, "Invalid end frequency, should be not greater than half of the sample rate\n");
            return STATUS_INVALID_VALUE;
        }
        else if (cfg->fEndFreq < cfg->fStartFreq)
        {
            fprintf(stderr, "Invalid end frequency, should be not less than start frequency\n");
            return STATUS_INVALID_VALUE;
        }

        // Chirp duration: as you wish
        if (cfg->fSweepLength <= 0.0f)
        {
            fprintf(stderr, "Invalid sine sweep length\n");
            return STATUS_INVALID_VALUE;
        }

        // Amplitude: as you wish but we can pin it to 1
        float cgain     = dspu::db_to_gain(cfg->fGain);

        // Delay between chirp and sine sweep
        if (cfg->fChirpDelay < 0.0f)
        {
            fprintf(stderr, "Invalid chirp delay value\n");
            return STATUS_INVALID_VALUE;
        }

        // Initialize output sample
        size_t length   = 0x1000; // length in samples, should be computed
        res             = out.init(1, length, length);
        if (res != STATUS_OK)
        {
            fprintf(stderr, "Could not initialize outut sample: error code=%d\n", int(res));
            return res;
        }
        out.set_sample_rate(cfg->nSampleRate); // This sample rate will be written to output file

        // TODO: sync chirp + sine sweep generation - main processing here
        // out   - output sample with generated chirp + sine sweep signal
        // cgain - the gain
        // length- length of the output sample
        // cfg   - other parameters

        // Save the sample to output
        if ((res = out.save(&cfg->sOutFile)) != STATUS_OK)
        {
            fprintf(stderr, "Could not write output audio file: error code=%d\n", int(res));
            return res;
        }

        return STATUS_OK;
    }

    status_t deconvolve(config_t *cfg)
    {
        status_t res;
        dspu::Sample in;        // Sample for input
        dspu::Sample out;       // Sample for output
        dspu::Sample ref;       // Sample for reference

        // Check that input file name is present
        if (cfg->sInFile.is_empty())
        {
            fprintf(stderr, "Not specified required input file name\n");
            return STATUS_INVALID_VALUE;
        }

        // Check that reference file name is present
        if (cfg->sReference.is_empty())
        {
            fprintf(stderr, "Not specified required reference file name\n");
            return STATUS_INVALID_VALUE;
        }

        // Read the input file
        if ((res = in.load(&cfg->sInFile)) != STATUS_OK)
        {
            fprintf(stderr, "Could not read input audio file: error code=%d\n", int(res));
            return res;
        }

        // Resample input file to desired sample rate
        if ((res = in.resample(cfg->nSampleRate)) != STATUS_OK)
        {
            fprintf(stderr, "Could not resample input audio file content: error code=%d\n", int(res));
            return res;
        }

        // Read the reference file
        if ((res = ref.load(&cfg->sReference)) != STATUS_OK)
        {
            fprintf(stderr, "Could not read reference audio file: error code=%d\n", int(res));
            return res;
        }

        // Resample reference file to desired sample rate
        if ((res = ref.resample(cfg->nSampleRate)) != STATUS_OK)
        {
            fprintf(stderr, "Could not resample reference audio file content: error code=%d\n", int(res));
            return res;
        }

        // Initialize output sample
        size_t length   = 0x1000; // length in samples, should be computed
        res             = out.init(in.channels(), length, length);
        if (res != STATUS_OK)
        {
            fprintf(stderr, "Could not initialize outut sample: error code=%d\n", int(res));
            return res;
        }
        out.set_sample_rate(cfg->nSampleRate); // This sample rate will be written to output file

        // TODO: deconvolution - main processing here
        // out   - output sample with generated chirp + sine sweep signal
        // length- length of the output sample
        // cfg   - other parameters

        // Save the sample to output
        if ((res = out.save(&cfg->sOutFile)) != STATUS_OK)
        {
            fprintf(stderr, "Could not write output audio file: error code=%d\n", int(res));
            return res;
        }

        return STATUS_OK;
    }

    int main(int argc, const char **argv)
    {
        config_t cfg;

        // Parse command line
        status_t res = parse_cmdline(&cfg, argc, argv);
        if (res != STATUS_OK)
            return res;

        // Common checks
        if (cfg.enMode == M_NONE)
        {
            fprintf(stderr, "Sweep or deconvolution operating mode should be selected\n");
            return STATUS_INVALID_VALUE;
        }

        // Check that output file name is present
        if (cfg.sOutFile.is_empty())
        {
            fprintf(stderr, "Not specified required output file name\n");
            return STATUS_INVALID_VALUE;
        }

        // Check sample rate
        if ((cfg.nSampleRate < MIN_SAMPLE_RATE) || (cfg.nSampleRate > MAX_SAMPLE_RATE))
        {
            fprintf(stderr, "Unsupported sample rate\n");
            return STATUS_INVALID_VALUE;
        }

        // Check mode
        if (cfg.enMode == M_SWEEP)
            return generate_sweep(&cfg);
        if (cfg.enMode == M_DECONVOLVE)
            return deconvolve(&cfg);

        return STATUS_OK;
    }
}

