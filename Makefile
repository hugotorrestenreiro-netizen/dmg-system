CC = gcc
CXX = g++

SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LIBS = $(shell sdl2-config --libs)

CFLAGS = -Wall -Wextra -O2 $(SDL_CFLAGS)
CXXFLAGS = -Wall -Wextra -O2 -IGb_Snd_Emu-main -IGb_Snd_Emu-main/gb_apu $(SDL_CFLAGS)
LDFLAGS = $(SDL_LIBS) -lstdc++

C_SRCS = main.c dmg.c memorymap.c ppu.c instructions.c render.c
CXX_SRCS = c_apu.cpp \
           Gb_Snd_Emu-main/Basic_Gb_Apu.cpp \
           Gb_Snd_Emu-main/gb_apu/Blip_Buffer.cpp \
           Gb_Snd_Emu-main/gb_apu/Gb_Apu.cpp \
           Gb_Snd_Emu-main/gb_apu/Gb_Oscs.cpp \
           Gb_Snd_Emu-main/gb_apu/Multi_Buffer.cpp

OBJS = $(C_SRCS:.c=.o) $(CXX_SRCS:.cpp=.o)
TARGET = gameboy_emu

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean