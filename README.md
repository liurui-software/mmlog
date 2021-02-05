# mmlog

"mmlog" is a simple log rotation tool for shell scripts, or apps which generate logging data to standard output or standard error. "mmlog" is an useful tool to prevent logging data to saturates user's disk.

There is built-in tool named "logrotate" in some systems. But "logrotate" costs too much CPU and cannot control the maximum size of the log file. "mmlog" is much more lightweight, esier configuration, and with more good features for shell scripts, or apps which generate logging data to standard output or standard error.  

## Build mmlog for your system
"mmlog" binaries for 7 platforms are provided in the "build" folder. You can also build "mmlog" for your own platforms with following command:
```
make
```

## Syntax to use mmlog:
```
<cmd> | mmlog
Or:
 mmlog <log-message>
```

## Environment Varibles:
```
MM_LOGFILE_NUM: Number of log files to rotate. Default is 10.
MM_LOGFILE: Name of the log file. Default is "mmlog"
MM_LOGSIZE: Max size of one log file. Default is 10M.
```

## Samples of usage
1) Rotate logging data from standard output, set logfile name...
```
export MM_LOGFILE=myapp
./myapp | ./mmlog
```
2) Rotate logging data from standard output and error, set logfile name and size, run in background...
```
export MM_LOGFILE=myapp
export MM_LOGSIZE=1000000
nohup ./myapp 2>&1 | ./mmlog >mmlog.out 2>&1 &
```


