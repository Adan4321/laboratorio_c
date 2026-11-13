CC = gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
LDLIBS ?= -lm
OBJETOS = main.o artemis.o validaciones.o

ifeq ($(OS),Windows_NT)
TARGET = simulador_artemis.exe
RUN = ejecutar.bat
CLEAN = limpiar.bat
else
TARGET = simulador_artemis
RUN = ./$(TARGET)
CLEAN = rm -f $(OBJETOS) $(TARGET)
endif

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
	$(RUN)

clean:
	$(CLEAN)

.PHONY: all run clean