# mmlog

## A Lightweight Log Rotation Tool for Safe Logging
Disk overflow is one of the most common operational issues in system maintenance. Modern systems run numerous applications and scripts that continuously generate large volumes of log data. In many cases, log files grow indefinitely without effective size control, eventually consuming all available disk space.

`mmlog` is a simple and lightweight log rotation tool designed for shell pipelines and applications that write logs to standard output (stdout) or standard error (stderr). It helps prevent disk saturation caused by uncontrolled log growth.

Unlike traditional solutions, `mmlog` is optimized for simplicity, low overhead, and ease of integration with scripts and CLI-based applications.

While tools such as logrotate are widely used in Linux environments, they are primarily designed for file-based rotation managed by the system and are not always suitable for real-time streaming logs from pipelines. In contrast, `mmlog` is designed specifically for streaming scenarios, offering lightweight processing and straightforward configuration, including explicit control over maximum file size.


## Build mmlog for your system
We only provide `mmlog` binaries for Linux and Windows amd64 platforms in the releases. You can build `mmlog` binaries for your own platforms with following command:
```
cmake .
make
```


## Usage
`mmlog` can be used in two ways. The primary mode is as a pipeline filter that processes output from other commands.

Pipeline mode (recommended)
```Bash
<command> | mmlog
```
Direct input mode
```Bash
mmlog <log-message>
```


## Environment Varibles:
`mmlog` can be configured using the following environment variables:

```
MM_LOGFILE
```

Specifies the log file name (including path).

Default: mmlog

```
MM_LOGFILE_NUM
```

Number of rotated log files to keep.

Default: 10

```
MM_LOGSIZE
```

Maximum size of a single log file before rotation occurs.
Supports suffixes such as K, M, or raw bytes.

Examples:
- 100000
- 50K
- 2M

Default: 10M

```
MM_LOG_PREFIX
```

Controls log line prefix formatting.

- 0 or empty: no prefix
- 1: prepend timestamp to each log entry

Default: 0 (no prefix)


## Examples

1) Rotate logs from stdout/stderr with size control
```
export MM_LOGFILE=myapp
export MM_LOGSIZE=100K
python testapp.py 2>&1 | ./mmlog
```


2) Run application in background with log rotation
```
export MM_LOGFILE=myapp
export MM_LOGSIZE=1M
nohup ./myapp 2>&1 | ./mmlog &
```

3) Use `mmlog` with Apache Tomcat
```
export TOMCAT_HOME=/opt/apache-tomcat-9.0.44
export CATALINA_OUT_CMD="nohup $TOMCAT_HOME/bin/mmlog"
export MM_LOGFILE=$TOMCAT_HOME/logs/tomcat
export MM_LOGSIZE=1M
$TOMCAT_HOME/bin/startup.sh
```


# Notes

- `mmlog` is designed for streaming log pipelines, not for managing existing log files on disk.
- It is most effective when used with applications that continuously write to stdout/stderr.
- For traditional file-based rotation workflows, system tools such as logrotate may still be appropriate.

