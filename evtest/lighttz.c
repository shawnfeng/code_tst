/*
Copyright (c) 2008, Arek Bochinski
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, 
    * this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, 
    * this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of Arek Bochinski nor the names of its contributors may be used to endorse or 
    * promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Compile command on my system, may be changed depending on your setup.

gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"lighttz.d" -MT"lighttz.d" -o"lighttz.o" "../lighttz.c"
gcc  -o"lighttz"  ./lighttz.o   -lev

You need to have Libev installed:
http://software.schmorp.de/pkg/libev.html
*/


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
#include <ev.h>

#define SERVER_PORT 8002
struct client {
        int fd;
        ev_io ev_write;
		ev_io ev_read;
};

ev_io ev_accept;

int
setnonblock(int fd)
{
    int flags;

    flags = fcntl(fd, F_GETFL);
    if (flags < 0)
            return flags;
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0) 
            return -1;

    return 0;
}

static void write_cb(struct ev_loop *loop, struct ev_io *w, int revents)
{ 
	struct client *cli= ((struct client*) (((char*)w) - offsetof(struct client,ev_write)));
	char superjared[]="HTTP/1.1 200 OK\r\nContent-Length: 336\r\nConnection: close\r\nContent-Type: text/html\r\nDate: Sat, 26 Apr 2008 01:13:35 GMT\r\nServer: lighttz/0.1\r\n\r\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\"><html xmlns=\"http://www.w3.org/1999/xhtml\" version=\"-//W3C//DTD XHTML 1.1//EN\" xml:lang=\"en\"><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/><title>Hello World</title></head><body><p>Hello World</p></body></html>";
 	
 	if (revents & EV_WRITE){
		write(cli->fd,superjared,strlen(superjared));
		ev_io_stop(EV_A_ w);
	}
 	close(cli->fd);
	free(cli);
	
}
static void read_cb(struct ev_loop *loop, struct ev_io *w, int revents)
{ 
	
	struct client *cli= ((struct client*) (((char*)w) - offsetof(struct client,ev_read)));
	int r=0;
	char rbuff[1024];
	if (revents & EV_READ){
		r=read(cli->fd,&rbuff,1024);
	}
	ev_io_stop(EV_A_ w);
	ev_io_init(&cli->ev_write,write_cb,cli->fd,EV_WRITE);
	ev_io_start(loop,&cli->ev_write);
	
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
   	 
	client = calloc(1,sizeof(*client));
	client->fd=client_fd;
	if (setnonblock(client->fd) < 0)
                err(1, "failed to set client socket to non-blocking");
	ev_io_init(&client->ev_read,read_cb,client->fd,EV_READ);
	ev_io_start(loop,&client->ev_read);
}
int main()
{
	struct ev_loop *loop = ev_default_loop (0);
	int listen_fd;
    struct sockaddr_in listen_addr; 
    int reuseaddr_on = 1;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0); 
    if (listen_fd < 0)
            err(1, "listen failed");
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
    if (listen(listen_fd,5) < 0)
            err(1, "listen failed");
    if (setnonblock(listen_fd) < 0)
            err(1, "failed to set server socket to non-blocking");
	 
	ev_io_init(&ev_accept,accept_cb,listen_fd,EV_READ);
	ev_io_start(loop,&ev_accept);
	ev_loop (loop, 0);
	return 0;
}
