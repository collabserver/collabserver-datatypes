CC=g++
FLAGS=-g -Wall
TARGET=crdt_examples.out


all: $(TARGET)

$(TARGET) : main.cpp
	$(CC) $(FLAGS) -I../include/ -o $@ $^


.PHONY: clean
clean:
	-rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
