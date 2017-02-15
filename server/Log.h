#ifndef LOG
#define LOG
#include <iostream>
#include <fstream>
#include <time.h>
#include <stdarg.h>
using namespace std;
class Log
{
				public:
								void log(const char * pattern, ...){
												char buffer[2048];
												va_list ap; 
												bzero(buffer, 2048);
												va_start(ap, pattern);
												vsnprintf(buffer, 2048 - 1, pattern, ap);
												va_end(ap);
												ofstream file;
												time_t rawtime;
												struct tm * timeinfo;

												time (&rawtime);
												timeinfo = localtime (&rawtime);
												char fileName[9];
												strftime (fileName,9,"%Y%m%d",timeinfo);
												file.open(fileName,std::ios::out|std::ios::app);
												if(!file){
													cout<<"open file failed"<<strerror(errno)<<endl;
													return;
													}
												char logTime[30];
												strftime (logTime,30,"[%Y%m%d %H:%M:%S]",timeinfo);
												 file<<logTime<<buffer<<std::endl;
								}
};
#endif
