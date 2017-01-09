#!/usr/bin/env bash

cd ..

# Check if 'make' completed
if [ ! -f ./dsemu ]; then
    echo "Please use 'make install' instead of this script.\n"
    exit 1
fi

# Check whether the application is already installed
if [ -f /usr/local/bin/dsemu ]; then
    echo "Another instance of the application is already installed. Use 'make uninstall'."
    exit 1
fi

if [ -f /usr/bin/dsemu ]; then
    echo "Another instance of the application is already installed via a package manager."
    exit 1
fi

# Install startup scripts
if [ -d /usr/lib/systemd ]; then
    mkdir -pm 755 /usr/lib/systemd/system
    cp ./scripts-install/startup-systemd.service /usr/lib/systemd/system/dsemu.service
    chown root:root /usr/lib/systemd/system/dsemu.service
    chmod u=rw,g=r,o=r /usr/lib/systemd/system/dsemu.service
    systemctl enable dsemu.service
    systemctl start dsemu.service
elif [ -d /etc/init.d ]; then
    cp ./scripts-install/startup-initd.sh /etc/init.d/dsemu
    chown root:root /etc/init.d/dsemu
    chmod u=rwx,g=rx,o=rx /etc/init.d/dsemu
    chkconfig --add dsemu
    service dsemu start
else
    echo "Installation failed: Cannot install startup script: Unknown system."
    exit 1
fi

# Add daemon user and group to the system
useradd -rU dsemu

# Install application binary executable
cp ./dsemu /usr/local/bin/dsemu
chown root:dsemu /usr/local/bin/dsemu
chmod u=rwx,g=rx,o=rx /usr/local/bin/dsemu

# Install program files for the application
mkdir -m 755 /usr/local/share/dsemu
chown root:root /usr/local/share/dsemu
cp *.py /usr/local/share/dsemu/
mkdir /usr/local/share/dsemu/lib
cp lib/*.py /usr/local/share/dsemu/lib/
chown root:dsemu /usr/local/share/dsemu/*.py
chown root:dsemu /usr/local/share/dsemu/lib/*.py
cp ./scripts-install/startup.sh /usr/local/share/dsemu/
cp ./scripts-install/shutdown.sh /usr/local/share/dsemu/
chown root:root /usr/local/share/dsemu/*.sh
chmod u=rwx,g=rx,o=rx /usr/local/share/dsemu/*
chmod u=rwx,g=rx,o=rx /usr/local/share/dsemu/lib/*

echo "Installation complete."
