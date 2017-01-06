#!/usr/bin/env bash

# Check if 'make' completed
if [ ! -f ./dsemu ]; then
    echo "Please use 'make install' instead of this script.\n"
    exit 1
fi

# Check whether the application is already installed
if [ -f /usr/local/bin/dsemu ]; then
    echo "Another instance of the application is already installed."
    read -p "Remove (y/n)? " -n 1 -r; echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then exit 1; fi
    ./uninstall.sh
fi

if [ -f /usr/bin/dsemu ]; then
    echo "Another instance of the application is already installed via a package manager."
    exit 1
fi

# Install startup scripts
if [ -d /usr/lib/systemd/system ]; then
    cp ./startup-systemd.service /usr/lib/systemd/system/dsemu.service
    chown root:root /usr/lib/systemd/system/dsemu.service
    chmod u=rw,g=r,o=r /usr/lib/systemd/system/dsemu.service
    systemctl enable dsemu.service
    systemctl start dsemu.service
elif [ -d /lib/systemd/system ]; then
    cp ./startup-systemd.service /lib/systemd/system/dsemu.service
    chown root:root /lib/systemd/system/dsemu.service
    chmod u=rw,g=r,o=r /lib/systemd/system/dsemu.service
    systemctl enable dsemu.service
    systemctl start dsemu.service
elif [ -d /etc/init.d ]; then
    cp ./startup-initd.sh /etc/init.d/dsemu
    chown root:root /etc/init.d/dsemu
    chmod u=rwx,g=rx,o=rx /etc/init.d/dsemu
    chkconfig --add dsemu
    service dsemu start
else
    echo "Installation failed: Cannot install startup script: Unknown system."
    exit 1
fi

# Add daemon user to the system
groupadd -r dsemu
useradd -rg dsemu dsemu

# Install application binary executable
cp ./dsemu /usr/local/bin/dsemu
chown root:dsemu /usr/local/bin/dsemu
chmod u=rwx,g=rs,o=rx /usr/local/bin/dsemu

# Install program files for the application
mkdir /usr/local/share/dsemu
chown root:root /usr/local/share/dsemu
chmod u=rwx,g=rx,o=rx /usr/local/share/dsemu
cp *.py /usr/local/share/dsemu/
chown root:dsemu /usr/local/share/dsemu/*.py
cp ./startup.sh /usr/local/share/dsemu/
cp ./shutdown.sh /usr/local/share/dsemu/
chown root:root /usr/local/share/dsemu/*.sh
chmod u=rwx,g=rx,o=rx /usr/local/share/dsemu/*

echo "Installation complete."
