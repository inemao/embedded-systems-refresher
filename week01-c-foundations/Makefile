CC = clang
CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -Wconversion -Iinclude

TARGET = signal_analyzer

SRC = src/main.c src/signal.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
