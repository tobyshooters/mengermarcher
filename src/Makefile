CC=g++
CFLAGS=-O3 -Wall -I. -c
TARGET=render

$(TARGET): render.o sdf.o
	$(CC) -o $(TARGET) render.o sdf.o

render.o: sdf.h utils.h
	$(CC) $(CFLAGS) render.cpp

sdf.o: sdf.h utils.h
	$(CC) $(CFLAGS) sdf.cpp

clean:
	rm -rf *.o && rm $(TARGET) && rm image.ppm
