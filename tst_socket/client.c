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

  
int main(int argc, char *argv[])  
{  
  int client_sockfd;
  int len;
  struct sockaddr_in remote_addr;
  int optlen;


  memset(&remote_addr, 0, sizeof(remote_addr));
  remote_addr.sin_family = AF_INET;
  remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  remote_addr.sin_port = htons(8599);


  if ((client_sockfd=socket(PF_INET,SOCK_STREAM,0))<0) {
      perror("socket");
      return 1;
  }
  /*将套接字绑定到服务器的网络地址上*/  
  if (connect(client_sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) < 0) {
    perror("connect");
    return 1;
  }
  printf("connected to server fd:%d\n", client_sockfd);


  //int flags = fcntl(client_sockfd, F_GETFL, 0); 
  //fcntl(client_sockfd, F_SETFL, flags & ~O_NONBLOCK);

  get_buff_size(client_sockfd);



  char sbuf[] = "HelloWorld";

  printf("one sz:%lu\n", sizeof(sbuf)-1);

  int cn_all = 0;
  int i;
  for (i = 0; i < 1000; i++) {
    int sz = check_write(client_sockfd, sbuf, sizeof(sbuf)-1);
    cn_all += sz;
    printf("send over fd:%d sz:%d all:%d\n", client_sockfd, sz, cn_all);
  }




  sleep(1000);

  return 0;
}
