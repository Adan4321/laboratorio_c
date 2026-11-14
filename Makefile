CC = gcc
CFLAGS = -Wall -Wextra -g
OBJS = main.o proceso_calculo.o validaciones.o
TARGET = artemis.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lm

%.o: %.c cabeceras.h validaciones.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /F /Q $(OBJS) $(TARGET) 2>nul || exit 0

