CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SURSA = sfl.c
EXECUTABIL = sfl

build: $(SOURCE)
	$(CC) -g $(CFLAGS) $(SURSA) -lm -o $(EXECUTABIL)

run_sfl: build
	./$(EXECUTABIL)

clean:
	rm -f $(EXECUTABIL) *.o
