CC = gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
LDLIBS ?= -lm
TARGET = simulador_artemis
OBJETOS = main.o artemis.o validaciones.o

all: $(TARGET)

$(TARGET): $(OBJETOS)
	$(CC) $(CFLAGS) -o $@ $(OBJETOS) $(LDLIBS)

main.o: main.c artemis.h validaciones.h
	$(CC) $(CFLAGS) -c main.c

artemis.o: artemis.c artemis.h validaciones.h
	$(CC) $(CFLAGS) -c artemis.c

validaciones.o: validaciones.c validaciones.h
	$(CC) $(CFLAGS) -c validaciones.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJETOS) $(TARGET)

.PHONY: all run clean
