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
#include "Session.h"
#include "Log.h"
#define MAX_COMMAND_LENGTH 1024*1024
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
																if(events[i].data.fd!=sock){
																				Session* session=(Session*)events[i].data.ptr;
																				if(-1==epoll_ctl(epfd,EPOLL_CTL_DEL,session->m_sock,&ev)){
																								cout<<session->m_sock<<strerror(errno)<<endl;
																				}else{
																								close(session->m_sock);
																								cout<<"close connect"<<session->m_sock<<endl;
																								//SAFE_DELETE(session);

																				}
																}
												}else if(events[i].events & EPOLLIN){
																if(events[i].data.fd==sock){
																				struct sockaddr_in addr;
																				socklen_t len=sizeof(struct sockaddr_in);
																				int cfd=TEMP_FAILURE_RETRY(::accept(sock,(struct sockaddr *)&addr,&len));
																				if(cfd>0){
																								Session* session=new Session(cfd);
																								ev.data.ptr=session;
																								assert(0==epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&ev));
																								cout<<"accept conncet"<<cfd<<endl;
																				}else{
																						cout<<"accept failed"<<strerror(errno)<<endl;
																				}

																}else{
																				Session* session=(Session*)events[i].data.ptr;
																				int ret=session->recv();
																				if(ret==1){
																								if(-1==epoll_ctl(epfd,EPOLL_CTL_DEL,session->m_sock,&ev)){
																									 cout<<session->m_sock<<strerror(errno)<<endl;	
																								}else{
																									close(session->m_sock);
																									cout<<"close connect"<<session->m_sock<<endl;
																									//SAFE_DELETE(session);
																								}
																				}
																}
												}else if(events[i].events & EPOLLOUT){
																//cout<<"wait write"<<events[i].data.fd<<endl;
												}	
								}
				}
}
