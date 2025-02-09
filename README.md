# mmlog

"mmlog" is a simple log rotation tool for shell scripts, or apps which generate logging data to standard output or standard error. "mmlog" is an useful tool to prevent logging data to saturates user's disk.

There is built-in tool named "logrotate" in some systems. But "logrotate" costs too much CPU and cannot control the maximum size of the log file. "mmlog" is much more lightweight, esier configuration, and with more good features for shell scripts, or apps which generate logging data to standard output or standard error.  

## Build mmlog for your system
"mmlog" binaries for 7 platforms are provided in the "build" folder. You can also build "mmlog" for your own platforms with following command:
```
cmake .
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
MM_LOGFILE: Name of the log file including file path. Default is "mmlog"
MM_LOGSIZE: Max size of one log file. Default is 10M.
MM_LOG_PREFIX: Prefix of log record. Default is no prefix, "timestamp" means time stamp.
```

## Samples of usage
1) Rotate logging data from standard output and error, set logfile name and size to rotate, run in background...
```
export MM_LOGFILE=myapp
export MM_LOGSIZE=1000000
nohup ./myapp 2>&1 | ./mmlog &
```

2) Rotate your Tomcat log 

For example your Tomcat installed on Linux/x64 and is located in directory "TOMCAT_HOME=/opt/apache-tomcat-9.0.44".
Down `mmlog` for Linux from page: https://github.com/liurui-1/mmlog/blob/master/build/linux.amd64/mmlog and copy to directory "$TOMCAT_HOME/bin". Use following command to start Tomcat. Then you make your Tomcat log rotated. The major Tomcat log file originally named `catalina.out` which can grow up to unlimited size，is now named tomcat#.log as 10 rotated log files up to 1M size).
```
export TOMCAT_HOME=/opt/apache-tomcat-9.0.44
export CATALINA_OUT_CMD="nohup $TOMCAT_HOME/bin/mmlog"
export MM_LOGFILE=$TOMCAT_HOME/logs/tomcat
export MM_LOGSIZE=1000000
$TOMCAT_HOME/bin/startup.sh
```

