all:
	gcc -std=c99 -D_POSIX_SOURCE -pthread src/server.c -I./include -o bin/server
	gcc -std=c99 -D_POSIX_SOURCE -pthread src/client.c -I./include -o bin/client

client:
	gcc -g -std=c99 -D_POSIX_SOURCE -pthread src/client.c -I./include -o bin/client

server:
	gcc -g -std=c99 -D_POSIX_SOURCE -pthread src/server.c -I./include -o bin/server


clean:
	rm bin/server
	rm bin/client
