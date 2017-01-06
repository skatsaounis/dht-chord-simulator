#!/usr/bin/env bash


# Check whether the application is already uninstalled
if [ ! -f /usr/local/bin/dsemu ]; then
    echo "Application has been already uninstalled."
    exit
fi

# Terminate daemon if it is running
dsemu terminate

# Remove program files of the application
rm -r /usr/local/share/dsemu

# Remove application binary executable
rm /usr/local/bin/dsemu

# Remove daemon user from the system
userdel dsemu
if [ $(getent group dsemu) ]; then
    groupdel dsemu
fi

# Uninstall startup scripts
if [ -f /usr/lib/systemd/system/dsemu.service ]; then
    systemctl stop dsemu.service
    systemctl disable dsemu.service
    rm /usr/lib/systemd/system/dsemu.service
elif [ -f /lib/systemd/system/dsemu.service ]; then
    systemctl stop dsemu.service
    systemctl disable dsemu.service
    rm /lib/systemd/system/dsemu.service
elif [ -d /etc/init.d ]; then
    service dsemu stop
    chkconfig --del dsemu
    rm /etc/init.d/dsemu
fi

echo "Uninstall complete."
