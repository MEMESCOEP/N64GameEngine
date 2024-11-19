BUILD_DIR=build
T3D_INST=$(shell realpath ../tiny3d)
PARENT=$(shell realpath ../)
CURDIR=$(shell realpath ./)

include $(N64_INST)/include/n64.mk
include $(T3D_INST)/t3d.mk

N64_CFLAGS += -std=gnu2x
MKFONT_FLAGS ?= --size 14

src = main.c

assets_ttf = $(wildcard assets/*.ttf)
assets_png = $(wildcard assets/*.png)
assets_gltf = $(wildcard assets/*.glb)
assets_conv = $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite))) \
			  $(addprefix filesystem/,$(notdir $(assets_ttf:%.ttf=%.font64))) \
			  $(addprefix filesystem/,$(notdir $(assets_gltf:%.glb=%.t3dm)))

all: EngineTest.z64
all:
	rm $(CURDIR)/N64GameEngine.c $(CURDIR)/N64GameEngine.h $(CURDIR)/MathUtils.c $(CURDIR)/MathUtils.h

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"

filesystem/%.font64: assets/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	$(N64_MKFONT) $(MKFONT_FLAGS) -o filesystem "$<"

filesystem/%.t3dm: assets/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) "$<" $@
	$(N64_BINDIR)/mkasset -c 2 -o filesystem $@

$(BUILD_DIR)/EngineTest.dfs: $(assets_conv)
$(BUILD_DIR)/EngineTest.elf: build/main.o main.c build/N64GameEngine.o N64GameEngine.c build/MathUtils.o MathUtils.c

EngineTest.z64: N64_ROM_TITLE="N64 Game Engine Test"
EngineTest.z64: $(BUILD_DIR)/EngineTest.dfs

clean:
	cp $(PARENT)/N64GameEngine.c $(PARENT)/N64GameEngine.h $(PARENT)/MathUtils.c $(PARENT)/MathUtils.h $(CURDIR)
	rm -rf $(BUILD_DIR) *.z64
	rm -rf filesystem

build_lib:
	rm -rf $(BUILD_DIR) *.z64
	make -C $(T3D_INST)
	make all

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
