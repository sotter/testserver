#!/usr/bin/env bash
PIDFILE=~/.mydaemon.pid
if [ x"$1" = x-daemon ]; then
  if test -f "$PIDFILE"; then exit; fi
  echo $$ > "$PIDFILE"
  trap "rm '$PIDFILE'" EXIT SIGTERM
  while true; do
    #launch your app here
    ./tcpserver &
    wait # needed for trap to work
  done
elif [ x"$1" = x-stop ]; then
  kill `cat "$PIDFILE"`
else
  nohup "$0" -daemon
fi