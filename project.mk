#
# Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
#           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
#
# This file is part of room-raider
#
# room-raider is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# room-raider is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with room-raider.  If not, see <https://www.gnu.org/licenses/>.
#

# Package version
ARTIFACT_ID                 = ROOM_RAIDER
ARTIFACT_NAME               = room-raider
ARTIFACT_DESC               = Room-Raider - a tool for performing off-line impulse response capture of the room
ARTIFACT_VERSION            = 0.5.3-devel

#------------------------------------------------------------------------------
# Plugin dependencies
# List of dependencies
DEPENDENCIES = \
  LIBPTHREAD \
  LIBDL \
  LSP_COMMON_LIB \
  LSP_LLTL_LIB \
  LSP_RUNTIME_LIB \
  LSP_DSP_LIB \
  LSP_DSP_UNITS

TEST_DEPENDENCIES = \
  LSP_TEST_FW

# Platform-dependent
ifeq ($(PLATFORM),Linux)
  DEPENDENCIES += \
    LIBSNDFILE
endif

ifeq ($(PLATFORM),BSD)
  DEPENDENCIES += \
    LIBSNDFILE \
    LIBICONV
endif

ifeq ($(PLATFORM),Windows)
  DEPENDENCIES += \
    LIBSHLWAPI \
    LIBWINMM \
    LIBMSACM
endif

# Overall system dependencies
ALL_DEPENDENCIES = \
  $(DEPENDENCIES) \
  $(TEST_DEPENDENCIES) \
  LIBSNDFILE \
  LIBICONV \
  LIBSHLWAPI \
  LIBWINMM \
  LIBMSACM




