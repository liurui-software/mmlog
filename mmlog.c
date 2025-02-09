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
 *  MM_LOGFILE: Name of the log file including file path. Default is "mmlog"
 *  MM_LOGSIZE: Max size of one log file. Default is 10M.
 *  MM_LOG_PREFIX: Prefix of log record. Default is no prefix, "timestamp" means time stamp.
 */
#define MM_LOGFILE_NUM "MM_LOGFILE_NUM"
#define MM_LOGFILE "MM_LOGFILE"
#define MM_LOGSIZE "MM_LOGSIZE"
#define MM_LOG_PREFIX "MM_LOG_PREFIX"

#define DEFAULT_FILE_NUM 10
#define DEFAULT_FILE_NAME "mmlog"
#define DEFAULT_FILE_SIZE 10000000L
#define PREFIX_TIMESTAMP "timestamp"
#define PREFIX_TYPE_TIMESTAMP 1

int LogFileNum = DEFAULT_FILE_NUM;
char *LogFile0 = DEFAULT_FILE_NAME;
char **LogFile;
FILE *LogFp = 0;
long LogMaxSize = DEFAULT_FILE_SIZE;
char LogTimeStr[40];
int PrefixType = 0;

/*
 */
void toEnd(int code, char *msg, char *msg1)
{
  fprintf(stderr, "Quit: %d - %s %s\n", code, msg, msg1);
  exit(code);
}

/*
 */
void init()
{
  char *p;
  int n1, i;

  p = getenv(MM_LOGFILE_NUM);
  if (p != 0)
  {
    LogFileNum = atoi(p);
    if (LogFileNum < 1)
      LogFileNum = 1;
  }

  LogFile = (char **)calloc(LogFileNum, sizeof(char *));

  p = getenv(MM_LOGFILE);
  if (p != 0)
  {
    LogFile0 = p;
  }

  p = getenv(MM_LOGSIZE);
  if (p != 0)
  {
    LogMaxSize = atol(p);
    if (LogMaxSize < 1000)
      LogMaxSize = 1000;
  }

  n1 = strlen(LogFile0) + 10;
  for (i = 0; i < LogFileNum; i++)
  {
    LogFile[i] = malloc(n1);
    if (!LogFile[i])
    {
      toEnd(-11, "Cannot allocate memory:", "for file names");
    }
    if (!i)
    {
      sprintf(LogFile[0], "%s.log", LogFile0);
    }
    else
    {
      sprintf(LogFile[i], "%s%d.log", LogFile0, i);
    }
  }

  p = getenv(MM_LOG_PREFIX);
  if (p != 0)
  {
    if (!strcmp(p, PREFIX_TIMESTAMP))
    {
      PrefixType = PREFIX_TYPE_TIMESTAMP;
    }
  }
}

/*
 */
void toClose()
{
  int i;

  if (LogFp)
  {
    fclose(LogFp);
    LogFp = 0;
  }

  for (i = 0; i < LogFileNum; i++)
  {
    free(LogFile[i]);
  }
}

/*
 */
long chkfilestat()
{
  int i;

  if (!LogFp)
  {
    LogFp = fopen(LogFile[0], "a");
    if (!LogFp)
    {
      toEnd(-2, "Error open file:", LogFile[0]);
    }
  }

  return ftell(LogFp);
}

/*
 */
void rotate()
{
  int i;

  fclose(LogFp);

  remove(LogFile[LogFileNum - 1]);
  for (i = LogFileNum - 1; i > 0; i--)
  {
    rename(LogFile[i - 1], LogFile[i]);
  }

  LogFp = fopen(LogFile[0], "a");
  if (!LogFp)
  {
    toEnd(-3, "Error open file:", LogFile[0]);
  }
}

/*
 */
char *getTimeString()
{
  time_t times = time(NULL);
  struct tm *utcTime = gmtime(&times);
  sprintf(LogTimeStr, "%04d-%02d-%02dT%02d:%02d:%02dZ", utcTime->tm_year + 1900, utcTime->tm_mon + 1, utcTime->tm_mday, utcTime->tm_hour, utcTime->tm_min, utcTime->tm_sec);
  return LogTimeStr;
}

/*
 */
char *getPrefix()
{
  if (PrefixType == PREFIX_TYPE_TIMESTAMP)
  {
    return getTimeString();
  }
  else
  {
    return 0;
  }
}

/*
 */
void appendMsg(char *logmsg)
{
  char *prefix = getPrefix();
  if (prefix)
  {
    fwrite(prefix, 1, strlen(prefix), LogFp);
    fwrite(" ", 1, 1, LogFp);
  }
  fwrite(logmsg, 1, strlen(logmsg), LogFp);
  fwrite("\n", 1, 1, LogFp);
  fflush(LogFp);
}


/*
 */
int main(int argc, char *argv[])
{
  char *logmsg;
  char *buf;
  int n;

  init();

  if (argc > 2)
  {
    fprintf(stderr, "Syntax:\n %s [<log-message>]\n", argv[0]);
    toEnd(-1, "arguments number must not greater than 2 @", getTimeString());
  }

  if (argc == 2)
  {
    logmsg = argv[1];
    /*printf("Got message: %s\n", logmsg);*/

    if (chkfilestat() > LogMaxSize)
      rotate();

    if (logmsg)
      appendMsg(logmsg);
  }
  else
  {
    buf = malloc(65536);
    if (!buf)
    {
      toEnd(-10, "Cannot allocate memory:", "Reading message from stdio");
    }

    do
    {
      logmsg = fgets(buf, 65536, stdin);
      if (!logmsg)
        break;

      n = strlen(logmsg);
      if (n > 0 && logmsg[n - 1] == '\n')
        logmsg[n - 1] = 0;

      if (chkfilestat() > LogMaxSize)
        rotate();

      appendMsg(logmsg);
    } while (1);

    free(buf);
  }

  toClose();
  toEnd(0, "Exit normally @", getTimeString());
}
