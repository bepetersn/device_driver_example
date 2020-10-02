#!/usr/bin/env bash

sudo true
make && \

# NOTE: Hack to make this idempotent 
# without my knowing bash very well

# Either rm, or if that fails, insert the driver
(rmmod deviceDriver.ko || insmod deviceDriver.ko) && \

# Now, we hopefully either rm'ed successfully or
# inserted successfully; try inserting 
# again to give the best chance the driver is 
# now inserted; this will complain but not fail
# if it was already inserted
insmod deviceDriver.ko && \

# then, mknod to attach the driver to a file 
mknod -m 777 /dev/chardev_test c 261 0

# Show devices to let us know what happened
cat /proc/devices | tail -n 30
ls -l /dev/chardev_test
