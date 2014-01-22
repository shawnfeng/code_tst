#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h> 
#include <fcntl.h>
#include <errno.h>
#include <err.h>
#include <stddef.h>
#include <assert.h>
#include <ev.h>

#define SERVER_PORT 8002
struct client {
        int fd;
        ev_io ev_write;
	ev_io ev_read;
};



static int setnonblock(int fd)
{
	int flags;
	// get current
	flags = fcntl(fd, F_GETFL);
	if (flags < 0)
		return flags;
	flags |= O_NONBLOCK;
	// set modify
	if (fcntl(fd, F_SETFL, flags) < 0) 
		return -1;

	return 0;
}

static int check_write(const int fd, const char *buff, int sz)
{
	int n;
	n = write(fd, buff, sz);
	if (n < 0) {
		if (errno == EINTR
		    || errno == EAGAIN) {
			printf("%s send queue full\n", strerror(errno));
		} else {
			printf("write error:%s\n", strerror(errno));
		}

	} else {
		printf("had beed write size %d\n", n);
	}

	return n;

}

static int check_read(const int fd, char *buff, int sz)
{
	int n;
	n = read(fd, buff, sz);
	if (n < 0) {
		if (errno == EINTR
		    || errno == EAGAIN) {
			printf("%s read over\n", strerror(errno));
		} else {
			printf("read error:%s\n", strerror(errno));
		}
	} else if (n == 0) {
		printf("read close by peer\n");
	} else {
		int i;
		printf("read SZ=%d STR=", sz);
		for (i = 0; i < n; ++i) {
			printf("%c", buff[i]);
		}
		printf("\n");
	}


	return n;
}


static void read_cb(struct ev_loop *loop, struct ev_io *w, int revents)
{ 
	
	struct client *cli= ((struct client*) (((char*)w) - offsetof(struct client,ev_read)));
	int n = 0;
	char rbuff[5];
	assert(revents == EV_READ);
	for (;;) {
		n = check_read(cli->fd, rbuff, sizeof(rbuff));
		if (n <= 0) {
			break;
		} else {
			// echo
			int s_n = check_write(cli->fd, rbuff, n);
			if (s_n < n) {
				printf("send buffer full\n");
				break;
			}
			if (s_n < 0) {
				// something err
				break;
			}
		}
		//sleep(1);
	}
	printf("ECHO OVER\n");

	ev_io_stop(EV_A_ w);
 	close(cli->fd);
	free(cli);

	
}
static void accept_cb(struct ev_loop *loop, struct ev_io *w, int revents)
{
	int client_fd;
	struct client *client;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	client_fd = accept(w->fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
                return;
        }
   	 
	client = calloc(1, sizeof(*client));
	client->fd = client_fd;
	if (setnonblock(client->fd) < 0)
                err(1, "failed to set client socket to non-blocking");

	ev_io_init(&client->ev_read, read_cb, client->fd, EV_READ);
	ev_io_start(loop, &client->ev_read);
}


int main()
{

	int listen_fd;
	struct sockaddr_in listen_addr; 
	int reuseaddr_on = 1;
	listen_fd = socket(AF_INET, SOCK_STREAM, 0); 
	if (listen_fd < 0)
		err(1, "create listen fd failed");
	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_on,
		       sizeof(reuseaddr_on)) == -1)
		err(1, "setsockopt failed");

	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = INADDR_ANY;
	listen_addr.sin_port = htons(SERVER_PORT);
	if (bind(listen_fd, (struct sockaddr *)&listen_addr,
		 sizeof(listen_addr)) < 0)
		err(1, "bind failed");

	if (listen(listen_fd, 5) < 0)
		err(1, "call listen failed");

	if (setnonblock(listen_fd) < 0)
		err(1, "failed to set server socket to non-blocking");
	 
	
	// start ev loop
	struct ev_loop *loop = ev_default_loop (0);
	ev_io ev_accept;
	ev_io_init(&ev_accept, accept_cb, listen_fd, EV_READ);
	ev_io_start(loop, &ev_accept);
	ev_loop(loop, 0);

	return 0;
}
