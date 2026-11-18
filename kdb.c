#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "lfsr.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <kdb file>", argv[0]);
        return 1;
    }

    const unsigned int INITIAL_VALUE = 0x4F574154;
    int fd = open(argv[1], O_RDONLY);
    unsigned char *head = malloc(10);
    read(fd, head, 10);
    char magic[6];
    uint32_t entryListAddress;
    memcpy(&entryListAddress, head+sizeof(magic), sizeof(entryListAddress));
    lseek(fd, entryListAddress, SEEK_SET);
    const size_t ENTRY_SIZE = 20;
    unsigned char *entry = malloc(ENTRY_SIZE);
    read(fd, entry, ENTRY_SIZE);
    int entryCount = 1;
    while (!(entry[0] == 0xFF && entry[1] == 0xFF && entry[2] == 0xFF && entry[3] == 0xFF))
    {
        char name[16];
        uint32_t blockListAddress;
        memcpy(name, entry, sizeof(name));
        memcpy(&blockListAddress, entry+sizeof(name), sizeof(blockListAddress));
        write(STDOUT_FILENO, name, strlen(name));
        write(STDOUT_FILENO, ": ", 2);
        lseek(fd, blockListAddress, SEEK_SET);
        const size_t BLOCK_SIZE = 6;
        unsigned char *block = malloc(BLOCK_SIZE);
        read(fd, block, BLOCK_SIZE);
        int blockCount = 1;
        while (!(block[0] == 0xFF && block[1] == 0xFF && block[2] == 0xFF && block[3] == 0xFF))
        {
            uint16_t size;
            uint32_t dataAddress;
            memcpy(&size, block, sizeof(size));
            memcpy(&dataAddress, block+sizeof(size), sizeof(dataAddress));
            lseek(fd, dataAddress, SEEK_SET);
            unsigned char *data = malloc(size);
            read(fd, data, size);
            Crypt(data, size, INITIAL_VALUE);
            write(STDOUT_FILENO, data, size);
            write(STDOUT_FILENO, " ", 1);
            lseek(fd, blockListAddress+blockCount*BLOCK_SIZE, SEEK_SET);
            read(fd, block, BLOCK_SIZE);
            blockCount++;
        }
        write(STDOUT_FILENO, "\n", 1);
        lseek(fd, entryListAddress+entryCount*ENTRY_SIZE, SEEK_SET);
        read(fd, entry, 20);
        entryCount++;
    }
}