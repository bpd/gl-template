CC = gcc

DEADCODESTRIP =  \
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
								
CFLAGS = -O3 -m64 -Wall -std=c99 -pedantic -DGLEW_STATIC -DGLEW_NO_GLU -DGLFW_NO_GLU



INCLUDES = -I./include

default: 
	echo "make win64 | x11"

win64: LFLAGS = -L./lib/win64 
win64: LIBS = -lglfw -lglu32 -lopengl32
win64: CFLAGS += -Wl,-static 
win64: EXEC_NAME = a.exe
win64: executable

win64-clean:
	rm a.exe

x11: LFLAGS = -L./lib/x11  -L/usr/lib
x11: LIBS = ./lib/x11/libglfw.a -lGLU -lGL
x11: EXEC_NAME = a.out
x11: executable

x11-clean:
	rm a.out

executable: glew.o glm.o shader.o src/main.c
	$(CC) $(DEADCODESTRIP) $(CFLAGS) $(INCLUDES) \
  glew.o glm.o shader.o src/main.c \
  $(LFLAGS) $(LIBS) \
  -o $(EXEC_NAME) 

room: glew.o glm.o shader.o src/room.c
	$(CC) $(DEADCODESTRIP) $(CFLAGS) $(INCLUDES) \
  glew.o glm.o shader.o src/room.c \
  -o room.exe $(LFLAGS) $(WIN_LIBS)
  
glew.o: src/glew.c
	$(CC) -c $(CFLAGS) $(INCLUDES) src/glew.c -o glew.o \
  $(LFLAGS) $(LIBS)

glm.o: src/glm.c
	$(CC) -c $(CFLAGS) $(INCLUDES)  src/glm.c -o glm.o

shader.o: src/shader.c
	$(CC) -c $(CFLAGS) $(INCLUDES)  src/shader.c -o shader.o
 
clean-objs:
	rm glm.o shader.o glew.o
