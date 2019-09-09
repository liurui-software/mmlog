#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * Log rotation tool
 *
 * Author Rui Liu
 * rockskyliu@gmail.com
 *
 * Syntax:
 *  <cmd> | mmlog
 * Or:
 *  mmlog <log-message>
 *
 * Environment Varibles:
 *  MM_LOGFILE_NUM: Number of log files to rotate. Default is 10.
 *  MM_LOGFILE: Name of the the log file. Default is "mmlog"
 *  MM_LOGSIZE: Max size of the log file. Default is 10.
 */
int LogFileNum=10;
char *LogFile0="mmlog";
char **LogFile;
FILE *LogFp=0, *LogFpX=0;
long LogMaxSize=20000000L;
char LogTimeStr[40];


/*
 */
void toEnd(int code, char *msg, char *msg1){
  printf("[mmlog] Quit: %d - %s %s\n", code, msg, msg1);
  exit(code);
}


/*
 */
void init(){
  char *p;
  int n1, i;
  
  p= getenv("MM_LOGFILE_NUM");
  if(p!=0)
    LogFileNum=atoi(p); 
  
  LogFile= (char **)calloc(LogFileNum, sizeof(char *));
  
  p= getenv("MM_LOGFILE");
  if(p!=0)
    LogFile0= p;
  
  p= getenv("MM_LOGSIZE");
  if(p!=0)
    LogMaxSize= atol(p); 
  
  n1= strlen(LogFile0)+10;
  for(i=0; i<LogFileNum; i++){
    LogFile[i] = malloc(n1);
    if(!i){
      sprintf(LogFile[0], "%s.log", LogFile0);
    } else{
      sprintf(LogFile[i], "%s%d.log", LogFile0, i);
    }
  }
}


/*
 */
void toClose(){
  int i;
  
  if(LogFp){
    fclose(LogFp);
    LogFp=0;
  }
  
  for(i=0; i<LogFileNum; i++){
    free(LogFile[i]);
  }
}


/*
 */
long chkfilestat(){
  int i;
  
  if(!LogFp){
    LogFp= fopen(LogFile[0], "a");
    if(!LogFp){
      toEnd(-2, "Error open file:", LogFile[0]);
    }
  }
  
  return ftell(LogFp);
}


/*
 */
void rotate(){
  int i;
  
  fclose(LogFp);
  
  remove(LogFile[LogFileNum-1]);
  for(i=LogFileNum-1; i>0; i--){
    rename(LogFile[i-1], LogFile[i]);
  }
  
  LogFp= fopen(LogFile[0], "a");
  if(!LogFp){
    toEnd(-3, "Error open file:", LogFile[0]);
  }
}


/*
 */
void appendMsg(char *logmsg){
  fwrite(logmsg, 1, strlen(logmsg), LogFp);
  fwrite("\n", 1, 1, LogFp);
  fflush(LogFp);
}


/*
 */
char *getTimeString(){
  time_t times = time(NULL);
  struct tm* utcTime = gmtime(&times);
  sprintf(LogTimeStr, "%04d-%02d-%02dT%02d:%02d:%02dZ", utcTime->tm_year+1900, utcTime->tm_mon+1, utcTime->tm_mday, utcTime->tm_hour, utcTime->tm_min, utcTime->tm_sec);
  return LogTimeStr;
}


/*
 */
int main( int argc, char *argv[] ){
  char *logmsg;
  char *buf;
  int n;
  
  init();
  
  if(argc>2){
    printf("[mmlog] Syntax:\n %s [<log-message>]\n", argv[0]);
    toEnd(-1, "arguments number must not greater than 2 @", getTimeString());
  }
  
  if(argc==2){
    logmsg= argv[1];
    printf("[mmlog] Got message: %s\n", logmsg);
    
    if( chkfilestat() > LogMaxSize)
      rotate();
    
    if(logmsg)
      appendMsg(logmsg);
  
  } else{
    printf("[mmlog] Read message from stdio\n");
    buf= malloc(65536);
   
    do{
      logmsg= fgets(buf, 65536, stdin);
      if(!logmsg)
        break;
      printf("[mmlog] Got message: %s\n", logmsg);
      
      n= strlen(logmsg);
      if(n>0 && logmsg[n-1]=='\n')
        logmsg[n-1]=0;
     
      if( chkfilestat() > LogMaxSize)
        rotate();
      
      appendMsg(logmsg);
    }while(1);
   
    free(buf);
  }
  
  toClose();
  toEnd(0, "Exit normally @", getTimeString());
}
