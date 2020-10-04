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
        a = atoi(buf);
    } while (a == 0 && buf != "0"); // Repeat until we get an int
    return a;
} 


int readCommand(int fd, char *buffer) {
    int numBytesToRead;
    numBytesToRead = \
            getNumber(4, "Enter the number of bytes you want to read: ");
    buffer = calloc(numBytesToRead, sizeof(char));
    if(!read(fd, buffer, numBytesToRead)) {
        puts("Read failed");
        return -1;
    }
    puts(buffer);
}

int writeCommand(int fd, char *buffer) {
       
}
int seekCommand(int fd) {
    int offset, whence, error;
    offset = getNumber(4, "Enter an offset value: ");
    whence = getNumber(1, "Enter a value for whence: ");
    error = lseek(fd, offset, whence);    
    if(!error) {
        printf("Seek position adjusted.\n"); 
    } else {
        printf("Seek failed.\n");
    }
    return error;
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
            case 'e':
                exit(0);
            default:
                ; // Ignore
        }
    }    
    
    fclose(fp);
    return 0;
}
