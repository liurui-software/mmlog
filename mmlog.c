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
 *  MM_LOG_PREFIX: Prefix of log record. Default is no prefix, "1" means using timestamp as prefix.
 */
#define MM_LOGFILE_NUM "MM_LOGFILE_NUM"
#define MM_LOGFILE "MM_LOGFILE"
#define MM_LOGSIZE "MM_LOGSIZE"
#define MM_LOG_PREFIX "MM_LOG_PREFIX"
#define MM_ALWAYS_FLUSH "MM_ALWAYS_FLUSH"

#define DEFAULT_FILE_NUM 10
#define DEFAULT_FILE_NAME "mmlog"
#define DEFAULT_FILE_SIZE 10000000L
#define PREFIX_TYPE_TIMESTAMP 1
#define MAX_LOG_SIZE_ATLEAST 1024
#define MAX_LOG_LINE_SIZE 65536
#define MAX_LOG_FILE_SUFFIX_LEN 12
#define MAX_TIMESTR_LEN 40
#define MAX_FILE_NUM 999999999

// Global Log Context
typedef struct
{
  int fileNum;
  char **fileNames;
  FILE *fp;
  long maxSize;
  int prefixType;
  int prefixLen;
  int alwaysFlush;
  time_t times;
  char timeStr[MAX_TIMESTR_LEN];
} LogContext;

/*
 * Quit the application process abnormally
 */
void toQuit(int code, const char *msg, const char *msg1)
{
  fprintf(stderr, "MMlog Quit (%d) - %s %s\n", code, msg, msg1);
  exit(code);
}

/*
 * Initialize Global Log Context
 */
LogContext *init()
{
  LogContext *ctx = calloc(1, sizeof(LogContext));
  if (!ctx)
    toQuit(-12, "Cannot allocate memory:", "for LogContext");

  char *p;
  int n1, i;

  ctx->fileNum = DEFAULT_FILE_NUM;
  ctx->maxSize = DEFAULT_FILE_SIZE;
  ctx->prefixType = 0;
  ctx->prefixLen = 0;
  ctx->alwaysFlush = 1;
  ctx->fp = NULL;
  ctx->times = 0;
  ctx->timeStr[0] = 0;

  p = getenv(MM_LOGFILE_NUM);
  if (p != 0)
  {
    ctx->fileNum = atoi(p);
    if (ctx->fileNum < 1)
      ctx->fileNum = 1;
    else if (ctx->fileNum > MAX_FILE_NUM)
      ctx->fileNum = MAX_FILE_NUM;
  }

  ctx->fileNames = (char **)calloc(ctx->fileNum, sizeof(char *));
  if (!ctx->fileNames)
  {
    free(ctx);
    toQuit(-11, "Cannot allocate memory:", "for file name array");
  }

  p = getenv(MM_LOGFILE);
  const char *baseName = DEFAULT_FILE_NAME;
  if (p != 0)
    baseName = p;

  p = getenv(MM_LOGSIZE);
  if (p != 0)
  {
    ctx->maxSize = atol(p);
    size_t len = strlen(p);
    if (len > 1)
    {
      int last = len - 1;
      if (p[last] == 'M' || p[last] == 'm')
        ctx->maxSize *= 1024L * 1024;
      else if (p[last] == 'K' || p[last] == 'k')
        ctx->maxSize *= 1024;
    }
    if (ctx->maxSize < MAX_LOG_SIZE_ATLEAST)
      ctx->maxSize = MAX_LOG_SIZE_ATLEAST;
  }

  n1 = strlen(baseName) + MAX_LOG_FILE_SUFFIX_LEN;
  for (i = 0; i < ctx->fileNum; i++)
  {
    ctx->fileNames[i] = malloc(n1);
    if (!ctx->fileNames[i])
    {
      for (int j = 0; j < i; j++)
      {
        free(ctx->fileNames[j]);
      }
      free(ctx->fileNames);
      free(ctx);
      toQuit(-11, "Cannot allocate memory:", "for file names");
    }
    if (!i)
      sprintf(ctx->fileNames[0], "%s.log", baseName);
    else
      sprintf(ctx->fileNames[i], "%s%d.log", baseName, i);
  }

  p = getenv(MM_LOG_PREFIX);
  if (p != 0)
    ctx->prefixType = atoi(p);

  p = getenv(MM_ALWAYS_FLUSH);
  if (p != 0)
    ctx->alwaysFlush = atoi(p);

  return ctx;
}

void toClose(LogContext *ctx)
{
  if (!ctx)
    return;

  if (ctx->fp)
  {
    fflush(ctx->fp);
    fclose(ctx->fp);
    ctx->fp = NULL;
  }

  if (ctx->fileNames)
  {
    for (int i = 0; i < ctx->fileNum; i++)
    {
      free(ctx->fileNames[i]);
    }
    free(ctx->fileNames);
  }

  free(ctx);
}

/*
 */
long chkFilestat(LogContext *ctx)
{
  if (!ctx->fp)
  {
    ctx->fp = fopen(ctx->fileNames[0], "a");
    if (!ctx->fp)
      toQuit(-2, "Error open file:", ctx->fileNames[0]);
  }

  fseek(ctx->fp, 0, SEEK_END);
  return ftell(ctx->fp);
}

/*
 * Rotate log files
 */
void rotate(LogContext *ctx)
{
  if (!ctx->fp)
    return;

  fflush(ctx->fp);
  fclose(ctx->fp);
  ctx->fp = NULL;

  for (int i = ctx->fileNum - 1; i > 0; i--)
  {
#ifdef _WIN32
    remove(ctx->fileNames[i]);
#endif
    rename(ctx->fileNames[i - 1], ctx->fileNames[i]);
  }

  ctx->fp = fopen(ctx->fileNames[0], "a");
  if (!ctx->fp)
    toQuit(-3, "Error open file:", ctx->fileNames[0]);
}

/*
 */
char *getTimeString(LogContext *ctx)
{
  time_t times = time(NULL);
  if (times != ctx->times)
  {
    ctx->times = times;
    struct tm *utcTime = gmtime(&times);
    sprintf(ctx->timeStr, "%04d-%02d-%02dT%02d:%02d:%02dZ",
            utcTime->tm_year + 1900,
            utcTime->tm_mon + 1,
            utcTime->tm_mday,
            utcTime->tm_hour,
            utcTime->tm_min,
            utcTime->tm_sec);
  }
  return ctx->timeStr;
}

/*
 */
const char *getPrefix(LogContext *ctx)
{
  if (ctx->prefixType == PREFIX_TYPE_TIMESTAMP)
  {
    char *timeStr = getTimeString(ctx);
    if (!ctx->prefixLen)
      ctx->prefixLen = strlen(timeStr);
    return timeStr;
  }

  return NULL;
}

/*
 */
void appendMsg(LogContext *ctx, const char *logmsg, size_t len)
{
  const char *prefix = getPrefix(ctx);
  if (prefix)
  {
    fwrite(prefix, 1, ctx->prefixLen, ctx->fp);
    fwrite(" ", 1, 1, ctx->fp);
  }
  fwrite(logmsg, 1, len, ctx->fp);
  fwrite("\n", 1, 1, ctx->fp);
  if (ctx->alwaysFlush)
    fflush(ctx->fp);
}

/*
 * Main function
 */
int main(int argc, char *argv[])
{
  LogContext *ctx = init();

  char *logmsg;
  char buf[MAX_LOG_LINE_SIZE];
  size_t n;

  if (argc > 2)
  {
    fprintf(stderr, "Syntax:\n %s [<log-message>]\n", argv[0]);
    toQuit(-1, "arguments number must not greater than 2 @", getTimeString(ctx));
  }

  if (argc == 2)
  {
    logmsg = argv[1];
    /*printf("Got message: %s\n", logmsg);*/

    if (chkFilestat(ctx) > ctx->maxSize)
      rotate(ctx);

    if (logmsg)
      appendMsg(ctx, logmsg, strlen(logmsg));
  }
  else
  {
    do
    {
      logmsg = fgets(buf, MAX_LOG_LINE_SIZE, stdin);
      if (!logmsg)
      {
        if (feof(stdin))
          break;

        if (ferror(stdin))
          toQuit(-13, "Cannot read data from stdin", "Reading message from stdin");
      }

      n = strlen(logmsg);
      if (n > 0 && logmsg[n - 1] == '\n')
      {
        n--;
        logmsg[n] = 0;
      }

      if (chkFilestat(ctx) > ctx->maxSize)
        rotate(ctx);

      appendMsg(ctx, logmsg, n);
    } while (1);
  }

  fprintf(stdout, "MMlog Ended @ %s \n", getTimeString(ctx));
  toClose(ctx);
  return 0;
}