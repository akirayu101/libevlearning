#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <string>

#include <stdlib.h>
#include <ev.h>
#include <iostream>
#define PORT 3003
#define BUFFER_SIZE 1024
using namespace std;
string user_name;
int sd;
static void read_cb(struct ev_loop *loop,struct ev_io *watcher,int revents)
{
  
  char buffer[BUFFER_SIZE];
  ssize_t read;
  if(EV_ERROR&revents)
  {
    perror("invalid events\n");
    return;
  }
  read = recv(watcher->fd,buffer,BUFFER_SIZE,0);
  if(read < 0)
  {
    perror("read error");
    return;
  }

  if(0 == read)
  {
    ev_io_stop(loop,watcher);
    free(watcher);
    perror("closing");
    return;
  }
  else
  {
    printf("message:%s\n",buffer);
  }
  bzero(buffer,read);
}

static void write_cb(struct ev_loop *loop,struct ev_io *watcher,int revents)
{
  
  char buffer[BUFFER_SIZE];
  ssize_t write_num;
  if(EV_ERROR&revents)
  {
    perror("invalid events\n");
    return;
  }
  gets(buffer);
  string temp(buffer);
  if(""==temp)
    return;
  temp = user_name+": "+temp;

  write_num = send(sd,temp.c_str(),BUFFER_SIZE,0);
  if(write_num < 0)
  {
    perror("write error");
    return;
  }
  printf("me:%s\n",buffer);
  bzero(buffer,BUFFER_SIZE);
}
int main()

{
  extern string user_name;
  while(""==user_name)
  {
    cout<<"please input your user name"<<endl;
    cin>>user_name;
  }
  cout<<"your nickname is: "<<user_name<<endl;

  struct sockaddr_in addr;
  int addr_len = sizeof(addr);

  if((sd = socket(PF_INET,SOCK_STREAM,0))<0)
  {
    perror("socket error");
    return -1;
  }
  bzero(&addr,sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_port=htons(PORT);
  addr.sin_addr.s_addr=htonl(INADDR_ANY);

  if(connect(sd,(struct sockaddr *)&addr,sizeof(addr))<0)
  {
    perror("connect error");
    return -1;
  }
  struct ev_loop *loop = ev_default_loop(0);
  struct ev_io w_read;
  struct ev_io w_write;

  ev_io_init(&w_read,read_cb,sd,EV_READ);
  ev_io_start(loop,&w_read);

  ev_io_init(&w_write,write_cb,STDIN_FILENO,EV_READ);
  ev_io_start(loop,&w_write);



  while(1)
  {
    ev_loop(loop,0);

  }

  return 0;
 
}
