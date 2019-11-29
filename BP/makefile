# Makefile for ECE1387 assignment3 Branch and Bound Partition - Zhiyuan Gu(1004920400)

CC := g++
RM := rm -f

CFLAGS := -Wall
LDFLAGS :=
LDLIBS := -lX11

OBJFILES := main.o Partitioning.o Block.o Net.o Node.o graphics.o draw.o
TARGET := BP

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) $(LDLIBS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	$(RM) $(OBJFILES) $(TARGET)
