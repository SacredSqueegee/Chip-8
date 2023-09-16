CC = clang
CFLAGS = -Wall -Wextra -O3

SDL_ROOT = /opt/homebrew/Cellar/sdl2
SDL_VERSION = 2.28.3
SDL_PATH = ${SDL_ROOT}/${SDL_VERSION}

INCLUDES = -I ${SDL_PATH}/include
LINKS = -L ${SDL_PATH}/lib 
LINK_FLAGS = -lSDL2

# INCLUDES = -I ${SDL_PATH}/include -I ./glad/include
# LINKS = -L ${SDL_PATH}/lib
# LINK_FLAGS = -lSDL2

APP = app

SRC_FILES = main.c
OBJ_FILES = main.o

${APP}: ${OBJ_FILES}
	$(CC) $(CFLAGS) -o $(APP) ${LINKS} $^ $(LINK_FLAGS)

main.o: main.c main.h
	$(CC) $(CFLAGS) ${INCLUDES} -c $^

clean:
	rm -rf $(OBJ_FILES) $(APP)
	rm -rf *.gch
