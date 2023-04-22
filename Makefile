CC = gcc
CFLAGS = -I./header -lm -march=native -funroll-loops -ffast-math -mavx2 -O3

SRCS = main.c src/tools.c src/sauvola.c src/pgm.c src/flow.c src/test.c
TARGET = run

$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) -o $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)