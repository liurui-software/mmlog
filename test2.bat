set MM_LOGFILE_NUM=5
set MM_LOGFILE=applog
set MM_LOGSIZE=2K
set MM_LOG_PREFIX=1
python testapp.py 2>&1 | mmlog