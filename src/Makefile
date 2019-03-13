CC=clang++
CFLAGS=-O3 -Wall -I. -std=c++11 -stdlib=libc++ -c
TARGET=render

$(TARGET): render.o sdf.o
	$(CC) -o $(TARGET) render.o sdf.o

render.o: render.cpp sdf.h Vec3.h Mat3.h utils.h threading.h animate.h
	$(CC) $(CFLAGS) render.cpp

sdf.o: sdf.cpp sdf.h Vec3.h utils.h
	$(CC) $(CFLAGS) sdf.cpp

clean:
	rm -rf *.o && rm $(TARGET)
