CC = gcc
CFLAGS = -Wall -Wextra -pthread
TARGET = echo
SRC = echo.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)