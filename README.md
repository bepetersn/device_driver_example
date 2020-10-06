
## How to build

    Make -C /lib/modules/$(uname -r)/build M=$PWD

### Where the basic devices are described:

/home/kernel/linux-hwe_4.15.0/Documentation/admin-guide/devices.txt 

### Command executed to create device file

    sudo mknod -m 777  /dev/chardev_test c 261 0

## How to run

sudo ./setup.sh does everything you need, including `make` `mknod` and `insmod`.
Afterwards, use the test program to test. It can be compiled with `make test`.

## Files

### deviceDriver.c

There's a lot here. Basically, driver_init and driver_exit are called at the init and exit times of the LKM load. These allocate / deallocate a buffer that is used to handle calls to a character device which is registered / unregistered as well.

These calls are handled in open, release, write, read, and seek functions as appropriate.


### test.c

This is my testing facility. It is incomplete / not exactly as desired, but can
do most of the work to show the device driver ... sort of. 

VERY IMPORTANT NOTE: the REPL functionality of the test program is buggy.
After the second pass through the REPL, you will be prompted twice
before input is accepted. In other words, the first prompt doesn't work.
