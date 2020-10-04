#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static const int BUFFER_SIZE = 1024;

int getNumber(int nDigits, char *message) {
   
    int a;
    char buf[nDigits+1];

    printf("%s", message);
    do {
        if (!fgets(buf, nDigits, stdin)) {
            // Reading input failed
            return -1;
        }
        a = atoi(buf);
    } while (a == 0); // Repeat until we get a number
    return a;
} 


int readCommand(int fd, char *buffer) {
    int numBytesToRead;
    numBytesToRead = \
            getNumber(4, "Enter the number of bytes you want to read: ");
    if(numBytesToRead) {
        buffer = calloc(numBytesToRead, sizeof(char));
        // TODO: Why isn't offset specified anywhere? How do I specify it?
        if(!read(fd, buffer, numBytesToRead)) {
            puts("Read failed");
            return -1;
        }
        puts(buffer);
    }
}

int writeCommand(int fd, char *buffer) {}
int seekCommand(int fd, char *buffer) {}

int main(int argc, int *argv[]) {
    
    int fd;
    FILE *fp;
    char *buffer;
    char testCommand[2];

    fp = fopen("/dev/chardev_test", "w+");
    if(fp == NULL) {
        perror("Error opening file");
        return -1;
    }
    fd = fileno(fp);
    while(1) {
        printf("Enter a command: ");
        fgets(testCommand, 2, stdin); 
        switch(testCommand[0]) {
            case 'r':
                readCommand(fd, buffer);
            case 'w':
                writeCommand(fd, buffer);
            case 's':
                seekCommand(fd, buffer);
            case 'e':
                exit(0);
            default:
                ; // Ignore
        }    
    }
    
    fclose(fp);
    return 0;
}
