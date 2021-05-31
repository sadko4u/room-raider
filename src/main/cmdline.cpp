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

#include <lsp-plug.in/lltl/pphash.h>
#include <lsp-plug.in/stdlib/string.h>
#include <lsp-plug.in/stdlib/stdio.h>
#include <lsp-plug.in/io/InStringSequence.h>
#include <lsp-plug.in/expr/Tokenizer.h>

#include <private/config.h>
#include <private/cmdline.h>

namespace room_raider
{
    using namespace lsp;

    typedef struct option_t
    {
        const char *s_short;
        const char *s_long;
        bool        s_flag;
        const char *s_desc;
    } option_t;

    static const option_t options[] =
    {
        { "-d",   "--deconvolve",       true,      "Deconvolve the captured signal"             },
        { "-ef",  "--end-freq",         false,     "End frequency of the sine sweep"            },
        { "-g",   "--gain",             false,     "Gain (in dB) of the sine sweep"             },
        { "-h",   "--help",             true,      "Output this help message"                   },
        { "-i",   "--in-file",          false,     "Input audio file"                           },
        { "-o",   "--out-file",         false,     "Output audio file"                          },
        { "-r",   "--reference",        false,     "Reference audio file"                       },
        { "-s",   "--sweep",            true,      "Produce sine sweep signal"                  },
        { "-sf",  "--start-freq",       false,     "Start frequency of the sine sweep"          },
        { "-sl",  "--sweep-length",     false,     "The length of the sweep in ms"              },
        { "-sr",  "--srate",            false,     "Sample rate of output files"                },
        { NULL, NULL, false, NULL }
    };

    status_t print_usage(const char *name, bool fail)
    {
        LSPString buf, fmt;
        size_t maxlen = 0;

        // Estimate maximum parameter size
        for (const option_t *p = room_raider::options; p->s_short != NULL; ++p)
        {
            buf.fmt_ascii("%s, %s", p->s_short, p->s_long);
            maxlen  = lsp_max(buf.length(), maxlen);
        }
        fmt.fmt_ascii("  %%-%ds    %%s\n", int(maxlen));

        // Output usage
        printf("usage: %s [arguments]\n", name);
        printf("available arguments:\n");
        for (const option_t *p = room_raider::options; p->s_short != NULL; ++p)
        {
            buf.fmt_ascii("%s, %s", p->s_short, p->s_long);
            printf(fmt.get_native(), buf.get_native(), p->s_desc);
        }

        return (fail) ? STATUS_BAD_ARGUMENTS : STATUS_SKIP;
    }

    status_t parse_cmdline_int(ssize_t *dst, const char *val, const char *parameter)
    {
        LSPString in;
        if (!in.set_native(val))
            return STATUS_NO_MEM;

        io::InStringSequence is(&in);
        expr::Tokenizer t(&is);
        ssize_t ivalue;

        switch (t.get_token(expr::TF_GET))
        {
            case expr::TT_IVALUE: ivalue = t.int_value(); break;
            default:
                fprintf(stderr, "Bad '%s' value\n", parameter);
                return STATUS_INVALID_VALUE;
        }

        if (t.get_token(expr::TF_GET) != expr::TT_EOF)
        {
            fprintf(stderr, "Bad '%s' value\n", parameter);
            return STATUS_INVALID_VALUE;
        }

        *dst = ivalue;

        return STATUS_OK;
    }

    status_t parse_cmdline_float(float *dst, const char *val, const char *parameter)
    {
        LSPString in;
        if (!in.set_native(val))
        {
            fprintf(stderr, "Out of memory\n");
            return STATUS_NO_MEM;
        }

        io::InStringSequence is(&in);
        expr::Tokenizer t(&is);
        float fvalue;

        switch (t.get_token(expr::TF_GET))
        {
            case expr::TT_IVALUE: fvalue = t.int_value(); break;
            case expr::TT_FVALUE: fvalue = t.float_value(); break;
            default:
                fprintf(stderr, "Bad '%s' value\n", parameter);
                return STATUS_INVALID_VALUE;
        }

        if (t.get_token(expr::TF_GET) != expr::TT_EOF)
        {
            fprintf(stderr, "Bad '%s' value\n", parameter);
            return STATUS_INVALID_VALUE;
        }

        *dst = fvalue;

        return STATUS_OK;
    }

    status_t parse_cmdline(config_t *cfg, int argc, const char **argv)
    {
        const char *cmd = argv[0], *val;
        lltl::pphash<char, char> options;

        // Read options to hash
        for (int i=1; i < argc; )
        {
            const char *opt = argv[i++];

            // Aliases
            for (const option_t *p = room_raider::options; p->s_short != NULL; ++p)
                if (!strcmp(opt, p->s_short))
                {
                    opt = p->s_long;
                    break;
                }

            // Check arguments
            const char *xopt = opt;
            if (!strcmp(opt, "--help"))
                return print_usage(cmd, false);
            else if ((opt[0] != '-') || (opt[1] != '-'))
            {
                fprintf(stderr, "Invalid argument: %s\n", opt);
                return STATUS_BAD_ARGUMENTS;
            }
            else
                xopt = opt;

            // Parse options
            bool found = false;
            for (const option_t *p = room_raider::options; p->s_short != NULL; ++p)
                if (!strcmp(xopt, p->s_long))
                {
                    if ((!p->s_flag) && (i >= argc))
                    {
                        fprintf(stderr, "Not defined value for option: %s\n", opt);
                        return STATUS_BAD_ARGUMENTS;
                    }

                    // Add option to settings map
                    val = (p->s_flag) ? NULL : argv[i++];
                    if (options.exists(xopt))
                    {
                        fprintf(stderr, "Duplicate option: %s\n", opt);
                        return STATUS_BAD_ARGUMENTS;
                    }

                    // Try to create option
                    if (!options.create(xopt, const_cast<char *>(val)))
                    {
                        fprintf(stderr, "Not enough memory\n");
                        return STATUS_NO_MEM;
                    }

                    found       = true;
                    break;
                }

            if (!found)
            {
                fprintf(stderr, "Invalid option: %s\n", opt);
                return STATUS_BAD_ARGUMENTS;
            }
        }

        // Override configuration file parameters
        status_t res;
        if (options.contains("--sweep"))
        {
            if (cfg->enMode != M_NONE)
            {
                fprintf(stderr, "Can not select sweep mode\n");
                return STATUS_NO_MEM;
            }
            cfg->enMode     = M_SWEEP;
        }
        if (options.contains("--deconvolve"))
        {
            if (cfg->enMode != M_NONE)
            {
                fprintf(stderr, "Can not select deconvolution mode\n");
                return STATUS_NO_MEM;
            }
            cfg->enMode     = M_DECONVOLVE;
        }

        if ((val = options.get("--in-file")) != NULL)
            cfg->sInFile.set_native(val);
        if ((val = options.get("--out-file")) != NULL)
            cfg->sOutFile.set_native(val);
        if ((val = options.get("--reference")) != NULL)
            cfg->sReference.set_native(val);
        if ((val = options.get("--srate")) != NULL)
        {
            if ((res = parse_cmdline_int(&cfg->nSampleRate, val, "sample rate")) != STATUS_OK)
                return res;
        }
        if ((val = options.get("--start-freq")) != NULL)
        {
            if ((res = parse_cmdline_float(&cfg->fStartFreq, val, "start frequency")) != STATUS_OK)
                return res;
        }
        if ((val = options.get("--end-freq")) != NULL)
        {
            if ((res = parse_cmdline_float(&cfg->fEndFreq, val, "end frequency")) != STATUS_OK)
                return res;
        }
        if ((val = options.get("--gain")) != NULL)
        {
            if ((res = parse_cmdline_float(&cfg->fGain, val, "gain")) != STATUS_OK)
                return res;
        }
        if ((val = options.get("--sweep-length")) != NULL)
        {
            if ((res = parse_cmdline_float(&cfg->fSweepLength, val, "sweep length")) != STATUS_OK)
                return res;
        }

        return STATUS_OK;
    }
}






