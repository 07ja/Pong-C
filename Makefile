CC = gcc
CFLAGS = -Wall -std=c99 -I/opt/homebrew/opt/sdl3/include
LDFLAGS = -L/opt/homebrew/opt/sdl3/lib
LIBS = -lSDL3

build:
	$(CC) $(CFLAGS) ./src/*.c -o pong $(LDFLAGS) $(LIBS)

run: build
	DYLD_LIBRARY_PATH=/opt/homebrew/opt/sdl3/lib ./pong

clean:
	rm -f pong
