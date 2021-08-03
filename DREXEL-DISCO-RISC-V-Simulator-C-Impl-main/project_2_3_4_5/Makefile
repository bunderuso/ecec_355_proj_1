SOURCE	:= Main.c Parser.c Registers.c Core.c
CC	:= gcc
TARGET	:= RVSim

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) -g -o $(TARGET) $(SOURCE) -lm

clean:
	rm -f $(TARGET)
