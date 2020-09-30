
## How to build

    Make -C /lib/modules/$(uname -r)/build M=$PWD

### Where the basic devices are described:

/home/kernel/linux-hwe_4.15.0/Documentation/admin-guide/devices.txt 

### Command executed to create device file

    sudo mknod -m 770  /dev/chardev_test c 261 0



