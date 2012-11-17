CC = gcc

DEADCODESTRIP = -Wl,-static \
								-fdata-sections \
								-ffunction-sections \
								-Wl,--gc-sections \
								-Wl,--strip-all

# -Wl,-static
# Link against static libraries.  Required for dead-code elimination

# -fdata-sections
# -ffunction-sections
# Keep data/functions in separate sections, so they can be discarded if unused

# Wl,--gc-sections
# tell the linker to garbage collect sections

# -s
# strip debug information
								
CFLAGS = -O3 -m64 -Wall -std=c99 -pedantic -DGLEW_STATIC -DGLEW_NO_GLU



# windows
WIN_LIBS = -lglfw  -lglu32 -lopengl32

LFLAGS = -L./lib 
INCLUDES = -I./include

default: glew.o glm.o shader.o src/main.c
	$(CC) $(DEADCODESTRIP) $(CFLAGS) $(INCLUDES) \
  glew.o glm.o shader.o src/main.c \
  -o game.exe $(LFLAGS) $(WIN_LIBS)

glew.o: src/glew.c
	$(CC) -c $(CFLAGS) $(INCLUDES) src/glew.c -o glew.o \
  $(LFLAGS) $(WIN_LIBS)

glm.o: src/glm.c
	$(CC) -c $(CFLAGS) $(INCLUDES)  src/glm.c -o glm.o

shader.o: src/shader.c
	$(CC) -c $(CFLAGS) $(INCLUDES)  src/shader.c -o shader.o

game.o: src/game.c
	$(CC) $(CFLAGS) $(INCLUDES) -c src/game.c -o game.o