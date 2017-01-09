#!/bin/sh
#
# /etc/init.d/dsemu
# Subsystem file for Distributed Systems Emulator Daemon
#
# chkconfig: 2345 99 01
# description: Distributed Systems Emulator Daemon
#

case "$1" in
	start)
        /usr/local/share/dsemu/startup.sh
		;;
	stop)
		/usr/local/share/dsemu/shutdown.sh
esac
exit
