ttts: ttts.o
		gcc -Wall $^ -o $@ -pthread
%.o: %.c
		gcc -c -Wall $<

