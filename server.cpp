#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <ev.h>
#include <stdlib.h>
#include <set>
#define PORT 3003
#define BUFFER_SIZE 1024
using namespace std;
set<int> client_fds;

int total_clients = 0;
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
    client_fds.erase(watcher->fd);
    total_clients --;
    return;
  }
  set<int> temp(client_fds);
  temp.erase(watcher->fd);
  for(set<int>::const_iterator it = temp.begin();it != temp.end();it++)
  {

    send((*it),buffer,read,0);


  }
  bzero(buffer,read);
}


static void accept_cb(struct ev_loop *loop,struct ev_io *watcher,int revents)
{
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_sd;
  struct ev_io *w_client = (struct ev_io*)malloc(sizeof(struct ev_io));
  if(EV_ERROR&revents)
  {
    perror("invalid events\n");
    return;
  }

  client_sd= accept(watcher->fd,(struct sockaddr*)&client_addr,&client_len);
  if(client_sd<0)
  {
    perror("accept error");
    return;
  }

  client_fds.insert(client_sd);
  total_clients ++;
  printf("client num is %d\n",total_clients);
  ev_io_init(w_client,read_cb,client_sd,EV_READ);
  ev_io_start(loop,w_client);
}

int main()
{
  struct ev_loop *loop = ev_default_loop(0);
  int sd;
  struct sockaddr_in addr;
  int addr_len = sizeof(addr);
  struct ev_io w_accept;



  if((sd = socket(PF_INET,SOCK_STREAM,0))<0)
  {
    perror("sock error\n");
    return -1;
  }


  bzero(&addr,sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr= INADDR_ANY;


  if((bind(sd,(struct sockaddr*)&addr,sizeof(addr))!=0))
  {      
    perror("bind error\n");
    return -1;
  }
  if((listen(sd,0)<0))
  {      
    perror("listen error\n");
    return -1;
  }
  ev_io_init(&w_accept,accept_cb,sd,EV_READ);
  ev_io_start(loop,&w_accept);

  while(1)
  {
    ev_loop(loop,0);
  }

  return 0;


}

