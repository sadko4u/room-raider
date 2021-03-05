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

BASEDIR            := $(CURDIR)
DEPLIST            := $(BASEDIR)/dependencies.mk
PROJECT            := $(BASEDIR)/project.mk
CONFIG             := $(CURDIR)/.config.mk

include $(BASEDIR)/make/functions.mk
ifeq ($(TREE),1)
  include $(DEPLIST)
else
  -include $(CONFIG)
endif

include $(PROJECT)

UNIQ_DEPENDENCIES       = $(call uniq,$(DEPENDENCIES) $(TEST_DEPENDENCIES))
UNIQ_ALL_DEPENDENCIES  := $(call uniq,$(ALL_DEPENDENCIES))

# Find the proper branch of the GIT repository
ifeq ($(TREE),1)
  MODULES                := $(BASEDIR)/modules
  GIT                    := git
  
  ifeq ($(findstring -devel,$(ARTIFACT_VERSION)),-devel)
    $(foreach dep, $(UNIQ_ALL_DEPENDENCIES), \
      $(eval $(dep)_BRANCH=devel) \
      $(eval $(dep)_PATH=$(MODULES)/$($(dep)_NAME)) \
    )
  else
    $(foreach dep, $(UNIQ_ALL_DEPENDENCIES), \
      $(eval $(dep)_BRANCH="$($(dep)_VERSION)") \
      $(eval $(dep)_PATH=$(MODULES)/$($(dep)_NAME)) \
    )
  endif
endif

# Form list of modules, exclude all modules that have 'system' version
SRC_MODULES         = $(foreach dep, $(UNIQ_DEPENDENCIES), $(if $(findstring src,$($(dep)_TYPE)),$(dep)))
HDR_MODULES         = $(foreach dep, $(UNIQ_DEPENDENCIES), $(if $(findstring hdr,$($(dep)_TYPE)),$(dep)))
BIN_MODULES         = $(foreach dep, $(UNIQ_DEPENDENCIES), $(if $(findstring bin,$($(dep)_TYPE)),$(dep)))
PLUG_MODULES        = $(foreach dep, $(UNIQ_DEPENDENCIES), $(if $(findstring plug,$($(dep)_TYPE)),$(dep)))
ALL_SRC_MODULES     = $(foreach dep, $(UNIQ_ALL_DEPENDENCIES), $(if $(findstring src,$($(dep)_TYPE)),$(dep)))
ALL_HDR_MODULES     = $(foreach dep, $(UNIQ_ALL_DEPENDENCIES), $(if $(findstring hdr,$($(dep)_TYPE)),$(dep)))
ALL_BIN_MODULES     = $(foreach dep, $(UNIQ_ALL_DEPENDENCIES), $(if $(findstring bin,$($(dep)_TYPE)),$(dep)))
ALL_PLUG_MODULES    = $(foreach dep, $(UNIQ_ALL_DEPENDENCIES), $(if $(findstring plug,$($(dep)_TYPE)),$(dep)))
ALL_PATHS           = $(foreach dep, $(ALL_SRC_MODULES) $(ALL_HDR_MODULES) $(ALL_BIN_MODULES) $(ALL_PLUG_MODULES), $($(dep)_PATH))

# Branches
.PHONY: $(ALL_SRC_MODULES) $(ALL_HDR_MODULES) $(ALL_BIN_MODULES) $(ALL_PATHS)
.PHONY: fetch prune clean

$(ALL_SRC_MODULES) $(ALL_HDR_MODULES) $(ALL_BIN_MODULES) $(ALL_PLUG_MODULES):
	@echo "Cloning $($(@)_URL) -> $($(@)_PATH) [$($(@)_BRANCH)]"
	@test -f "$($(@)_PATH)/.git/config" || $(GIT) clone "$($(@)_URL)" "$($(@)_PATH)"
	@$(GIT) -C "$($(@)_PATH)" reset --hard
	@$(GIT) -C "$($(@)_PATH)" fetch origin --force
	@$(GIT) -C "$($(@)_PATH)" fetch origin '+refs/heads/*:refs/tags/*' --force
	@$(GIT) -c advice.detachedHead=false -C "$($(@)_PATH)" checkout origin/$($(@)_BRANCH) || \
	 $(GIT) -c advice.detachedHead=false -C "$($(@)_PATH)" checkout refs/tags/$($(@)_BRANCH) || \
	 $(GIT) -c advice.detachedHead=false -C "$($(@)_PATH)" checkout origin/$($(@)_NAME)-$($(@)_BRANCH) || \
	 $(GIT) -c advice.detachedHead=false -C "$($(@)_PATH)" checkout refs/tags/$($(@)_NAME)-$($(@)_BRANCH)

$(ALL_PATHS):
	@echo "Removing $(notdir $(@))"
	@-rm -rf $(@)

fetch: $(SRC_MODULES) $(HDR_MODULES) $(BIN_MODULES) $(PLUG_MODULES)

tree: $(ALL_SRC_MODULES) $(ALL_HDR_MODULES) $(ALL_BIN_MODULES) $(ALL_PLUG_MODULES)

clean:
	@echo rm -rf "$($(ARTIFACT_VARS)_BIN)/$(ARTIFACT_NAME)"
	@-rm -rf "$($(ARTIFACT_VARS)_BIN)/$(ARTIFACT_NAME)"

prune: $(ALL_PATHS)

