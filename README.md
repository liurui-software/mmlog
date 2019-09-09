# mmlog

Log rotation tool for shell scripts or apps which generate logs to standard output or error. "mmlog" is an useful tool if you do not want the logging data saturates your disk.

## Build mmlog for your system
By default, a "mmlog" for Linux X64 is provided in the "build" folder. You can build "mmlog" for your own platform with following command:
```
make
```

## Syntax:
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
1) Rotate logging data from standard output and error, set logfile name and size, run in background...
```
export MM_LOGFILE=myapp
export MM_LOGSIZE=1000000
nohup ./myapp 2>&1 | ./mmlog >mmlog.out 2>&1 &
```

