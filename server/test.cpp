#include <iostream>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <cassert>
#include <string.h>
using namespace std;
int main()
{
	int sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	int reuse=1;
	if(-1==setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse)))
	{
		cout<<"set reuse failed"<<endl;	
		close(sock);
		return false;
	}
	struct sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(80);
	if(-1==bind(sock,(struct sockaddr*)&addr,sizeof(struct sockaddr)))
	{
		cout<<"bind failed"<<endl;
                close(sock);
                return false;
	}
	if(-1==listen(sock,2000)){
		cout<<"listen failed"<<endl;
                close(sock);
                return false;
	}
	int epfd=epoll_create(1);
	int epfd1=epoll_create(1);
	struct epoll_event ev;
	ev.events=EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLPRI;
	ev.data.ptr=NULL;
	ev.data.fd=sock;
	assert(0==epoll_ctl(epfd,EPOLL_CTL_ADD,sock,&ev));
	std::vector<struct epoll_event> events;
	events.resize(256);
	while(1){
		int ret=epoll_wait(epfd, &events[0], 10, 1);
		for(int i=0;i<ret;i++)
		{
			if(events[i].events & (EPOLLERR | EPOLLPRI)){
                cout<<"error"<<events[i].data.fd<<endl;
            }else if(events[i].events & EPOLLIN){
				if(events[i].data.fd==sock){
					struct sockaddr_in addr;
					socklen_t len=sizeof(struct sockaddr_in);
					int cfd=TEMP_FAILURE_RETRY(::accept(sock,(struct sockaddr *)&addr,&len));
					if(cfd>0){
						ev.data.fd=cfd;
						assert(0==epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&ev));
						assert(0==epoll_ctl(epfd1,EPOLL_CTL_ADD,cfd,&ev));
					}
					cout<<"accept conncet"<<cfd<<endl;
					
				}else{
					cout<<"event"<<i<<endl;
					char a[10];
					bzero(a,10);
					int retcode=TEMP_FAILURE_RETRY(::recv(events[i].data.fd,a,10,0));
					if(-1==retcode){
						if ( errno == EAGAIN || errno == EWOULDBLOCK ){}
					}else if(0==retcode){
						ev.data.fd=events[i].data.fd;
						assert(0==epoll_ctl(epfd,EPOLL_CTL_DEL,events[i].data.fd,&ev));
						close(events[i].data.fd);
						cout<<"close connect"<<endl;
					}else{
						cout<<"handle message 0 "<<a <<endl;
					}
				}
			}else if(events[i].events & EPOLLOUT){
				//cout<<"wait write"<<events[i].data.fd<<endl;
			}	
		}
	}
	
}
