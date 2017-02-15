//环形缓冲区,避免每次读取数据之后对剩余数据内存的复制，读取的时候如果缓冲区只有一个单位的数据，读取之后
//将写入位置调整到环形的开头
#ifndef BYTEBUFF_H
#define BYTEBUFF_H
#include <iostream>
#include "Log.h"
using namespace std;
class ByteBuffer
{
	 private:
	 int readPos;
	 int writePos;
	 int bufferSize;
	 int dataSize;
	 std::vector<unsigned char> buffer;
   Log log;
	 public:
	 ByteBuffer():readPos(0),writePos(0),dataSize(0),bufferSize(1024)
	 {
			buffer.resize(bufferSize); 
   }
	 inline unsigned char* wr_buf()
	 {
			//如果buff不够扩大buff,每次增加512
			log.log("wr_buf1 bufferSize %d,writePos %d readPos %d",bufferSize,writePos,readPos);
			return &buffer[writePos];
	 }
	 //剩余可连续写入的空间
	 inline int wr_size(){
			log.log("wr_size bufferSize %d,writePos %d readPos %d",bufferSize,writePos,readPos);
			if(remain_size()<512){
				 int oldSize=bufferSize;
				 bufferSize+=512;
				 buffer.resize(bufferSize);
				 if(writePos>readPos){
				 			//这种情况不需要处理，可写的空间在后面，添加的空间也在后面
				 }else if(writePos<readPos){
							//这种情况添加在空间在数据空间后面，为了保证数据能够继续写下去，需要把数据移到后面	
							memmove(&buffer[readPos], &buffer[readPos+512],oldSize-readPos);
							readPos+=512;
				 }else{
				 		if(writePos==0){
							  //这种情况不需要处理,可写空间在后面，添加的空间也在后面
						}else{
								//这种情况添加在空间在数据空间后面，为了保证数据能够继续写下去，需要把数据移到后面
								memmove(&buffer[readPos], &buffer[readPos+512],oldSize-readPos);
								readPos+=512;
						}
				 }
			}
			int wrSize=0;
			if(writePos>readPos){
							wrSize=bufferSize-writePos;
			}else if(writePos<readPos){
							wrSize=readPos<writePos;
			}else{
						  if(bufferSize!=dataSize){
								wrSize= bufferSize-writePos;
							}else{
								wrSize=0;
							}
			}
			log.log("wr_size bufferSize %d,writePos %d readPos %d wr_size %d",bufferSize,writePos,readPos,wrSize);
			return wrSize;
	 }
	 //剩余空闲可写入的空间
	 inline int remain_size(){
					int remainSize=0;
					log.log(" remain_size bufferSize %d,writePos %d readPos %d",bufferSize,writePos,readPos);
					 if(writePos>readPos){
									 remainSize=bufferSize-(writePos-readPos);
					 }else if(writePos<readPos){
									 remainSize=readPos-writePos;
					 }else{
							 remainSize=bufferSize-dataSize;
					 }
					log.log(" remain_size bufferSize %d,writePos %d readPos %d reamin_size %d",bufferSize,writePos,readPos,remainSize);
					return remainSize;
	 }	
	 //记录写入的大小,调整写入的位置标志
	 inline void write(uint32_t writeSize){
			dataSize+=writeSize;
			log.log(" write bufferSize %d,writePos %d readPos %d dataSize %d",bufferSize,writePos,readPos,dataSize);
	 		if(writePos+writeSize==bufferSize){   //表明已经写到了缓冲的尾部需要调整最前面
				  writePos=0;
			}else{
					writePos+=writeSize;
			}
	 }
	//读取记录打印出来
	inline void readAndPrint(){
			log.log(" readAndPrint bufferSize %d,writePos %d readPos %d dataSize %d",bufferSize,writePos,readPos,dataSize);
				char data[102410];
				int i=0;
		    if(readPos<writePos){
						for(;readPos<writePos;readPos++){
							//	cout<<buffer[readPos];
								dataSize--;
								data[i]=buffer[readPos];
								i++;
						}
						
				}else if(readPos>writePos){
						//第一次读到缓冲区结尾
						for(;readPos<bufferSize;readPos++){
             //   cout<<buffer[readPos];
                dataSize--;
								data[i]=buffer[readPos];
                i++;
            }
						readPos=0;
						//第二次读取到writePos前面
						for(;readPos<writePos;readPos++){
							//	cout<<buffer[readPos];
								dataSize--;
								data[i]=buffer[readPos];
                i++;
						}
				}else{
								if(dataSize==bufferSize){
												if(readPos>0){
																for(;readPos<bufferSize;readPos++){
																			//	cout<<buffer[readPos];
																				dataSize--;
																				data[i]=buffer[readPos];
                												i++;
																}
																readPos=0;
																for(;readPos<writePos;readPos++){
																			//	cout<<buffer[readPos];
																				dataSize--;
																				data[i]=buffer[readPos];
                												i++;
																}
												}else{
													for(;readPos<bufferSize;readPos++){
																			dataSize--;
                                      data[i]=buffer[readPos];
                                      i++;

													}
													readPos=0;
												}
								}
				}
				data[i]='\0';
				log.log(data);
			log.log(" readAndPrint bufferSize %d,writePos %d readPos %d dataSize %d",bufferSize,writePos,readPos,dataSize);

	}
};
#endif
