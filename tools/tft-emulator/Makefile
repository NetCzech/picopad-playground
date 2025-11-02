CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-sign-compare -Wno-narrowing \
		   -Wno-unused-parameter -Wno-implicit-fallthrough \
		   -DSFML_STATIC -static -static-libgcc -static-libstdc++ \
           -I"C:/Development/sfml/include" \
		   -I"." \
		   -Isnd \
		   -Iimg

LDFLAGS  = -L"C:/Development/sfml/lib" \
           -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lsfml-audio-s \
		   -lopengl32 -lfreetype -lwinmm -lgdi32 -lws2_32 \
		   -lflac -lvorbis -lvorbisenc -lvorbisfile -logg -mwindows


TARGET   = tft-emulator.exe
SRCS     = main.cpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	-del /Q $(TARGET)
