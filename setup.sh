#!/usr/bin/env bash

sudo true
make && \
# Either rm or if that fails, insert
(rmmod deviceDriver.ko || insmod deviceDriver.ko) && \
insmod deviceDriver.ko && \
# Now, ^ we either rm'ed successfully or
# inserted it successfully; 
# try inserting again to make sure it was the former;

# then, make sure to mknod again if insertion 
# occurred in second line above
mknod -m 777 /dev/chardev_test c 261 0
cat /proc/devices | tail -n 30
ls -l /dev/chardev_test
