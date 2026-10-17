C_SRCS = main.c dmg.c memorymap.c ppu.c instructions.c render.c
CXX_SRCS = c_apu.cpp \
           Gb_Snd_Emu-main/Basic_Gb_Apu.cpp \
           Gb_Snd_Emu-main/gb_apu/Blip_Buffer.cpp \
           Gb_Snd_Emu-main/gb_apu/Gb_Apu.cpp \
           Gb_Snd_Emu-main/gb_apu/Gb_Oscs.cpp \
           Gb_Snd_Emu-main/gb_apu/Multi_Buffer.cpp


OBJS_LINUX = $(C_SRCS:.c=.o) $(CXX_SRCS:.cpp=.o)
OBJS_WIN   = $(C_SRCS:.c=.win.o) $(CXX_SRCS:.cpp=.win.o)

TARGET_LINUX = gameboy_emu
TARGET_WIN   = gameboy_emu.exe

CC_LINUX  = gcc
CXX_LINUX = g++

SDL_CFLAGS_LINUX = $(shell sdl2-config --cflags)
SDL_LIBS_LINUX   = $(shell sdl2-config --libs)

CFLAGS_LINUX   = -Wall -Wextra -O2 $(SDL_CFLAGS_LINUX)
CXXFLAGS_LINUX = -Wall -Wextra -O2 -IGb_Snd_Emu-main -IGb_Snd_Emu-main/gb_apu $(SDL_CFLAGS_LINUX)
LDFLAGS_LINUX  = $(SDL_LIBS_LINUX) -lstdc++

CC_WIN  = x86_64-w64-mingw32-gcc
CXX_WIN = x86_64-w64-mingw32-g++

SDL2_DIR = SDL2-2.30.1/x86_64-w64-mingw32

SDL_CFLAGS_WIN = -I$(SDL2_DIR)/include -I$(SDL2_DIR)/include/SDL2
SDL_LIBS_WIN   = -L$(SDL2_DIR)/lib -lmingw32 -lSDL2main -lSDL2 -mconsole

CFLAGS_WIN   = -Wall -Wextra -O2 $(SDL_CFLAGS_WIN)
CXXFLAGS_WIN = -Wall -Wextra -O2 -IGb_Snd_Emu-main -IGb_Snd_Emu-main/gb_apu $(SDL_CFLAGS_WIN)
LDFLAGS_WIN  = $(SDL_LIBS_WIN) -lstdc++ -static-libgcc -static-libstdc++


all: $(TARGET_LINUX)

# Cible Linux
$(TARGET_LINUX): $(OBJS_LINUX)
	$(CXX_LINUX) $(OBJS_LINUX) -o $@ $(LDFLAGS_LINUX)

%.o: %.c
	$(CC_LINUX) $(CFLAGS_LINUX) -c $< -o $@

%.o: %.cpp
	$(CXX_LINUX) $(CXXFLAGS_LINUX) -c $< -o $@

# Cible Windows
win: $(TARGET_WIN)

$(TARGET_WIN): $(OBJS_WIN)
	$(CXX_WIN) $(OBJS_WIN) -o $@ $(LDFLAGS_WIN)

%.win.o: %.c
	$(CC_WIN) $(CFLAGS_WIN) -c $< -o $@

%.win.o: %.cpp
	$(CXX_WIN) $(CXXFLAGS_WIN) -c $< -o $@

clean:
	rm -f $(OBJS_LINUX) $(OBJS_WIN) $(TARGET_LINUX) $(TARGET_WIN)

.PHONY: all win clean