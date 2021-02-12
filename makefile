clean:
	rm -f *.txt *.out *.o

all: shell.cpp
	g++ shell.cpp -g -fsanitize=address

comp: clean all
	./a.out


run: all
	./a.out