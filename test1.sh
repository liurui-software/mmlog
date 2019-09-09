#!/bin/sh

rm -f *.log
./mmlog 123
./mmlog abc
MM_LOG_PREFIX=1 ./mmlog AA11BB22CC33
MM_LOGFILE=mylog ./mmlog "log with new file name"
MM_LOG_PREFIX=1 MM_LOGFILE=mylog ./mmlog "log with new file name and prefix"
