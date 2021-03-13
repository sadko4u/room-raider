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

#include <lsp-plug.in/dsp-units/util/Oversampler.h>
#include <lsp-plug.in/dsp-units/units.h>
#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/debug.h>
#include <lsp-plug.in/dsp-units/util/Convolver.h>

#include <private/dsp.h>

#include <math.h>

namespace room_raider
{
    using namespace lsp;

    status_t synth_test_sweep(const config_t *cfg, dspu::Sample &out)
    {
        // A swept sine is a complex signal. Let's use oversampler to make sure we don't introduce too much aliasing.
        dspu::Oversampler sOversampler;
        if (!sOversampler.init())
            return STATUS_FAILED;

        sOversampler.set_sample_rate(cfg->nSampleRate);
        sOversampler.set_mode(dspu::OM_LANCZOS_8X3);
        sOversampler.update_settings();

        size_t nOversampling = sOversampler.get_oversampling();
        size_t nOverRate = nOversampling * cfg->nSampleRate;

        // Allocate the array of the oversampled chirp samples.
        uint8_t *pData;
        size_t nSamples = dspu::seconds_to_samples(nOverRate, cfg->fSweepLength);

        float *ptr = alloc_aligned<float>(pData, nSamples);
        if (ptr == NULL)
            return STATUS_NO_MEM;

        lsp_guard_assert(float *save = ptr);

        float *vSweep = ptr;
        ptr += nSamples;

        lsp_assert(ptr <= &save[nSamples]);

        // Some synth action. Linear Swept Sine (it makes deconvolution easier, lowers aliasing).
        float fSlope = (cfg->fEndFreq - cfg->fStartFreq) / cfg->fSweepLength;

        for (size_t n = 0; n < nSamples; ++n)
        {
            float fTime = float(n) / float(nOverRate);
            // For linear chirp we use quadratic instantaneous phase.
            vSweep[n] = dspu::db_to_gain(cfg->fGain) * sin(2.0f * M_PI * (0.5f * fSlope * fTime * fTime + cfg->fStartFreq * fTime));
        }

        // In-place downsample.
        size_t nDownSamples = nSamples / nOversampling;
        sOversampler.downsample(vSweep, vSweep, nDownSamples);

        // Copy to destination Sample object.

        // We expect this to be mono.
        if (out.channels() != 1)
            return STATUS_FAILED;

        float *vDst = out.getBuffer(0);
        size_t nOutLength = out.length();

        // We expect the Sample object to hold more samples than the sweep.
        if (nOutLength < nDownSamples)
            return STATUS_FAILED;

        // Fill with zero first, only the first samples are swept sine.
        dsp::fill_zero(vDst, nOutLength);

        // Copy The swept sine.
        dsp::copy(vDst, vSweep, nDownSamples);

        // Clean allocated resources.
        sOversampler.destroy();

        free_aligned(pData);
        pData = NULL;
        vSweep = NULL;

        // Done.
        return STATUS_OK;
    }

    status_t deconvolve(const config_t *cfg, const dspu::Sample &in, const dspu::Sample &ref, dspu::Sample &out)
    {
        // We first prepare the data in a new buffers as we need to have them all the same length.
        size_t nBufferSize = lsp_max(in.length(), ref.length());
        size_t nIRSize = 2 * nBufferSize - 1;
        size_t nOrigin = nIRSize / 2; // this is the origin of time in the deconvolution result.
        size_t nInChannels = in.channels();

        // We expect the reference to be mono.
        if (ref.channels() != 1)
            return STATUS_FAILED;

        // We will process the input channels one at a time.
        // Allocate 3 buffers:
        // 1X Input data buffer, of size nIRSize, so that the convolution operation produces the correct number of samples.
        // 1X Deconvolution kernel buffer, of size nBufferSize
        // 1X Deconvolution result buffer, of size nIRSize
        uint8_t *pData;
        size_t nTotal = nIRSize + nBufferSize + nIRSize;

        float *ptr = alloc_aligned<float>(pData, nTotal);
        if (ptr == NULL)
            return STATUS_NO_MEM;

        lsp_guard_assert(float *save = ptr);

        float *vInput = ptr;
        ptr += nIRSize;

        float *vKernel = ptr;
        ptr += nBufferSize;

        float *vResult = ptr;
        ptr += nIRSize;

        lsp_assert(ptr <= &save[nSamples]);

        // Let's fill the kernel, it is simply the reference, but backwards in time.
        dsp::fill_zero(vKernel, nBufferSize);
        const float *vRef = ref.getBuffer(0);
        dsp::reverse2(vKernel, vRef, ref.length());

        // Process.
        dspu::Convolver sConvolver;

        for (size_t ch = 0; ch < nInChannels; ++ch)
        {
            // Even though we always use the same impulse response,
            // we initialise at every iteration to make sure the internal state of the convolver is re-initialisated.
            if (!sConvolver.init(vKernel, nBufferSize, 16, 0))
                return STATUS_NO_MEM;

            dsp::fill_zero(vResult, nIRSize);
            dsp::copy(vInput, in.getBuffer(ch), in.length());
            dsp::fill_zero(vResult, nIRSize);

            sConvolver.process(vResult, vResult, nIRSize);

            // Copy to destination:
            dsp::fill_zero(out.getBuffer(ch), out.length());
            dsp::copy(out.getBuffer(ch), &vResult[nOrigin], lsp_min(out.length(), nIRSize - nOrigin));
        }

        // Clean allocated resources.
        free_aligned(pData);
        pData = NULL;
        vInput = NULL;
        vKernel = NULL;

        sConvolver.destroy();

        // Done.
        return STATUS_OK;
    }
}