all: create client

create:
	mkdir -p bin

client: src/client_zad2.c src/myprot.c
	cc src/client_zad2.c src/myprot.c -o bin/client

clean:
	rm -f bin/client bin/server