OBJS = parse-bacs.o chans_opcode.o util.o
CFLAGS = -g -std=c99 -DDEBUG


all: parse-bacs

parse-bacs: $(OBJS)
	$(CC) $(OBJS) -o $@


%.o: %.c
	$(CC) $^ -c $(CFLAGS)

clean:
	rm *.o parse-bacs


valgrind-run:
	valgrind --gen-suppressions=all --leak-check=full --show-reachable=yes --leak-resolution=high --log-file=parse-bacs.valgrind ./parse-bacs icon.cs
