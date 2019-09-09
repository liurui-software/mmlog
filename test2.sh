#!/bin/sh

export MM_LOGFILE_NUM=5
export MM_LOGFILE=applog
export MM_LOGSIZE=2K
export MM_LOG_PREFIX=1
python testapp.py 2>&1 | ./mmlog
