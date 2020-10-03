#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int getNumber(int nDigits, char *message) {
   
    int a;
    char buf[nDigits+1];

    do
    {
        puts(message);
        if (!fgets(buf, nDigits, stdin))
        {
            // Reading input failed
            return -1;
        }
        a = atoi(buf);
    } while (a == 0); // repeat until we got a valid number
    return a;
} 

int main(int argc, int *argv[]) {
    
    FILE *fp;
    const int BUFFER_SIZE = 1024;
    char *buffer;
    int success;

    /* For getting a command*/
    char testCommand[2];
    /* For read operation */
    int numBytesToRead;
    /* For write operation */
    char* dataToWrite;
    /* For seek operation */
    loff_t offset;
    int whence;    

    fp = fopen("/dev/chardev_test", "w+");
    if(fp == NULL) {
        perror("Error opening file");
        return -1;
    }
    printf("Enter a command: ");
    fgets(testCommand, 2, stdin); 
    if(testCommand == "r") {
   
        numBytesToRead = getNumber(4, "Enter the number of bytes you want to read: ");
        if(numBytesToRead == 0) 
        {
            perror("You either entered an invalid number or requested 0 bytes. Try again next time.");  
            return -1;
        }

        buffer = calloc(numBytesToRead, sizeof(char));
        // TODO: Why isn't offset specified anywhere? How do I specify it?
        success = read(fileno(fp), buffer, numBytesToRead); 
        if(!success) {
            puts("Read failed");
            return -1;
        }
        puts(buffer);
    }
    if(testCommand == "w") {
        
        printf("Enter data you want to write: ");
        //fgets("%d", &dataToWrite);
    }
    fclose(fp);
    return 0;
}
