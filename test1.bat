del /q *.log

set MM_LOG_PREFIX=
set MM_LOGFILE=

mmlog 123

mmlog abc

set MM_LOG_PREFIX=1
mmlog AA11BB22CC33

set MM_LOG_PREFIX=
set MM_LOGFILE=mylog
mmlog "log with new file name"

set MM_LOG_PREFIX=1
set MM_LOGFILE=mylog
mmlog "log with new file name and prefix"
