CC=g++
LD=g++
CFLAGS=-c -g -std=gnu++17 -Wall -Wextra
LFLAGS=-lGL -lglfw -lGLEW -lpthread

OBJS=main.o \
	 App.o \
	 ReadMesh.o \
	 graphics/Shader.o \
	 graphics/Texture.o \
	 graphics/Mesh.o \
	 graphics/Entity.o \
	 entities/Background.o \

TARGET=main


all: $(TARGET)

clean:
	rm -f $(OBJS) main

$(TARGET): $(OBJS)
	$(LD) $^ -o $@ $(LFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@
