CC = gcc

debug: CC += -g
debug: all

all:
	$(CC) tcp_send.c -o tcp_send
	$(CC) tcp_recv.c -o tcp_recv
	$(CC) udp_server.c -o udp_server
	$(CC) udp_client.c -o udp_client

tcp_send: tcp_send.c
	$(CC) tcp_send.c -o tcp_send

tcp_recv: tcp_recv.c
	$(CC) tcp_recv.c -o tcp_recv

udp_server: udp_server.c
	$(CC) udp_server.c -o udp_server

udp_client: udp_client.c
	$(CC) udp_client.c -o udp_client

clean:
	rm tcp_send tcp_recv udp_server udp_client
