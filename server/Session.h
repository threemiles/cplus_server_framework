#ifndef SESSION_H
#define SESSION_H
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
#include "ByteBuffer.h"
using namespace std;
class Session
{	
				public:
				int m_sock;
				ByteBuffer byteBuffer;
				public:
				Session(int sock):m_sock(sock)
				{
				}  
				int recv(){
								int retcode=TEMP_FAILURE_RETRY(::recv(m_sock,byteBuffer.wr_buf(),byteBuffer.wr_size(),0));
								if(-1==retcode){
												if ( errno == EAGAIN || errno == EWOULDBLOCK ){}
								}else if(0==retcode){
												return 1;
								}else{
												byteBuffer.write(retcode);
												byteBuffer.readAndPrint();
								}
								return 0;

				}
				int send(){
						
				}
};
#endif
