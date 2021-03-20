/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of timbre-mill
 * Created on: 11 февр. 2021 г.
 *
 * timbre-mill is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * timbre-mill is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with timbre-mill. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef PRIVATE_CONFIG_H_
#define PRIVATE_CONFIG_H_

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/runtime/LSPString.h>

namespace room_raider
{
    using namespace lsp;

    enum mode_t
    {
        M_NONE,
        M_SWEEP,
        M_DECONVOLVE
    };

    /**
     * Overall configuration
     */
    struct config_t
    {
        private:
            config_t & operator = (const config_t &);

        public:
            mode_t                                  enMode;         // Operating mode
            ssize_t                                 nSampleRate;    // Sample rate for output files
            float                                   fStartFreq;     // Start frequency of sine sweep
            float                                   fEndFreq;       // End frequency of sine sweep
            float                                   fGain;          // Gain of the sine sweep signal
            float                                   fSweepLength;   // The length of the sweep
            LSPString                               sInFile;        // Source file
            LSPString                               sOutFile;       // Destination file
            LSPString                               sReference;     // Reference file

        public:
            explicit config_t();
            ~config_t();

        public:
            void clear();
    };

}

#endif /* PRIVATE_CONFIG_H_ */
