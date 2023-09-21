CC = clang
CFLAGS = -Wall -Wextra -Werror -O3 -std=c17

SDL_ROOT = /opt/homebrew/Cellar/sdl2
SDL_VERSION = 2.28.3
SDL_PATH = ${SDL_ROOT}/${SDL_VERSION}

INCLUDES = -I ${SDL_PATH}/include
LINKS = -L ${SDL_PATH}/lib 
LINK_FLAGS = -lSDL2

# INCLUDES = -I ${SDL_PATH}/include -I ./glad/include
# LINKS = -L ${SDL_PATH}/lib
# LINK_FLAGS = -lSDL2

APP = app.out

SRC_FILES = main.c chip8.c ./helpers/logging.c
OBJ_FILES = main.o chip8.o logging.o

${APP}: ${OBJ_FILES}
	$(CC) $(CFLAGS) -o $(APP) ${LINKS} $^ $(LINK_FLAGS)
	@echo

main.o: main.c main.h chip8.h ./helpers/logging.h
	$(CC) $(CFLAGS) ${INCLUDES} -c $^

chip8.o: chip8.c chip8.h ./helpers/logging.h
	$(CC) $(CFLAGS) ${INCLUDES} -c $^

logging.o: ./helpers/logging.c ./helpers/logging.h
	$(CC) $(CFLAGS) ${INCLUDES} -c $^


.PHONY: run
run: ${APP}
	@echo Running ${APP} ...
	@./${APP} && echo "${APP}" exited with: $$?


.PHONY: clean
clean:
	rm -rf $(OBJ_FILES) $(APP)
	rm -rf *.gch ./helpers/*.gch
