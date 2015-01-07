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

int get_buff_size(int fd)
{
  int err = -1;
  int snd_size = 0;   /* 发送缓冲区大小 */
  int rcv_size = 0;    /* 接收缓冲区大小 */
  socklen_t optlen;    /* 选项值长度 */


  optlen = sizeof(snd_size);
  err = getsockopt(fd, SOL_SOCKET, SO_SNDBUF,&snd_size, &optlen);
  if (err < 0) {
    printf("get send buff err\n");
  }


  optlen = sizeof(rcv_size);
  err = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcv_size, &optlen);
  if (err < 0) {
    printf("get recv buff err\n"); 
  }
     
  printf("send buff:%d\n", snd_size);
  printf("recv buff:%d\n", rcv_size);

}


int set_buff_size(int fd)
{
  int err = -1;
  int snd_size = 0;   /* 发送缓冲区大小 */
  int rcv_size = 0;    /* 接收缓冲区大小 */
  socklen_t optlen;    /* 选项值长度 */



  get_buff_size(fd);
  printf("====before set====\n");

  snd_size = 1024;
  optlen = sizeof(snd_size);
  err = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &snd_size, optlen);
  if (err < 0) {
    printf("set send buff err\n"); 
  } 


  rcv_size = 1024;
  optlen = sizeof(rcv_size); 
  err = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&rcv_size, optlen);
  if (err<0) {
    printf("set recv buff err\n");
  }


  printf("====after set====\n");
  get_buff_size(fd);


}



#define SERVER_PORT 8599
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


    printf("setsocket ok\n");

	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = INADDR_ANY;
	listen_addr.sin_port = htons(SERVER_PORT);
	if (bind(listen_fd, (struct sockaddr *)&listen_addr,
		 sizeof(listen_addr)) < 0)
		err(1, "bind failed");

	if (listen(listen_fd, 5) < 0)
		err(1, "call listen failed");
    /*
	if (setnonblock(listen_fd) < 0)
		err(1, "failed to set server socket to non-blocking");
    */


    printf("wait select:%s\n", inet_ntoa(listen_addr.sin_addr));
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd == -1) {
      err(1, "failed accept");
    }
    printf("accept fd:%d addr:%s\n", client_fd, inet_ntoa(client_addr.sin_addr));

    set_buff_size(client_fd);


    sleep(1000);

	return 0;
}
