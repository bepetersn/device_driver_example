#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static const int BUFFER_SIZE = 1024;

int getNumber(int nDigits, char *message) {
    int a;
    char buf[nDigits+1]; // 1 for null at the end 

    printf("%s", message);
    do {
        if (!fgets(buf, nDigits+1, stdin)) {
            // Reading input failed
            exit(-1);
        }
        // TODO: not clearing buffer totally here
        a = atoi(buf);
    } while (a == 0 && buf != "0"); // Repeat until we get an int
    return a;
} 


int readCommand(int fd, char *buffer) {
    int numBytesToRead, rv;
    numBytesToRead = \
            getNumber(4, "Enter the number of bytes you want to read: ");
    buffer = calloc(numBytesToRead, sizeof(char));
    rv = read(fd, buffer, numBytesToRead);
    if (rv != -1) {
        puts(buffer);
    } else {
        puts("Read failed");
    }
    return rv;
}

int writeCommand(int fd, char *buffer) {
    int rv;
    buffer = calloc(BUFFER_SIZE, sizeof(char)); 
    if(!fgets(buffer, BUFFER_SIZE, stdin)) {
        puts("Write failed");
        exit(-1);
    }
    rv = write(fd, buffer, strlen(buffer));
    if(rv != -1) {
        puts("Wrote succesfully.");
    } else {
        puts("Write failed");
    }
    return rv; 
}
int seekCommand(int fd) {
    int offset, whence, rv;
    offset = getNumber(4, "Enter an offset value: ");
    whence = getNumber(1, "Enter a value for whence [1 for SEEK_SET, 2 for SEEK_CUR, 3 for SEEK_END]: ");
    rv = lseek(fd, offset, whence-1); // get around atoi's limitation    
    if(rv != -1) {
        printf("Seek position adjusted.\n"); 
    } else {
        printf("Seek failed.\n");
    }
    return rv;
}
 
int showOffset(int fd) {
    // Doesn't work...
    printf("Offset: %li\n", lseek(fd, (off_t) 0, 1 /* SEEK_CUR */));
    return 0;
}

int main(int argc, int *argv[]) {
    
    int fd;
    FILE *fp;
    char *buffer;
    char testCommand;

    fp = fopen("/dev/chardev_test", "w+");
    if(fp == NULL) {
        perror("Error opening file");
        return -1;
    }
    fd = fileno(fp);
    while(1) {
        printf("Enter a command: ");
        testCommand = getchar(); 
        while(getchar() != '\n'); // Mostly clear buffer; sometimes fails because C
        switch(testCommand) {
            case 'r':
                readCommand(fd, buffer);
                break;
            case 'w':
                writeCommand(fd, buffer);
                break;
            case 's':
                seekCommand(fd);
                break;
            case 'o':
                showOffset(fd);
                break;
            case 'e':
                exit(0);
            default:
                ; // Ignore
        }
    }    
    
    fclose(fp);
    return 0;
}
