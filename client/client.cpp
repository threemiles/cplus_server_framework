#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;
string m_strIP = "127.0.0.1";  
int m_nPort = 80;  
void create_clients(int num){
				cout<<"thread"<<num<<endl;
				int clients[100];
				for(int i=0;i<100;i++){
								struct sockaddr_in addr;  
								struct timeval timeo = {10, 0};  
								unsigned long flags;  
								memset(&addr,0x00,sizeof(struct sockaddr_in)); 
								int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
								if(fd<0){
										cout<<"create socket failed"<<endl;
										return;
								}
								addr.sin_family = AF_INET;  
								addr.sin_addr.s_addr = inet_addr(m_strIP.c_str());  
								addr.sin_port = htons(m_nPort); 
								int retcode = connect(fd, (struct sockaddr*)&addr, sizeof(addr));  
								if(retcode<0){
										cout<<"connect failed"<<endl;
                    return;
								}
								clients[i]=fd;
				}
				while(1){
						for(int i=0;i<100;i++){
								char buff[4];
								buff[0]='1';
								buff[1]='-';
								buff[2]='3';
								int retcode=send(clients[i],&buff[0],3,0);
								//cout<<retcode<<" "<<clients[i]<<endl;
						}
				}
}
int main(){
				cout<<"client"<<endl;
				for(int i=0;i<10;i++){
						cout<<i<<endl;
						std::thread t(create_clients,i);
						t.detach();  
				}
				while(1){
					sleep(10);
				}
				return 0;
}
