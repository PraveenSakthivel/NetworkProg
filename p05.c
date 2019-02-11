#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv){
    if(argc < 3){
        printf("Error: Invalid Arguments\n");
        return -1;
    }
    FILE *file;
    file = fopen(argv[1],"r");
    if(file == NULL){
        printf("%s\n",strerror(errno));
        return -1;
    }
    int numSubs = argc - 2;
    int counts[numSubs];
    int i;
    for(i = 0; i < numSubs; i++){
        counts[i] = 0;
    }
    char *buffer = malloc(sizeof(char) * 100);
    char next;
    int size = 100;
    int length = -1;
    while(!feof(file)){
        next = getc(file);
        length++;
        if(next == ' ' || next == '\n'){
            if (length == size){
                size = size + 1;
                realloc(buffer,size * sizeof(char));
            }
            buffer[length] = '\0';
            for(i = 0; i < numSubs; i++){
                int count = 0;
                const char *temp = buffer;
                while(temp = strcasestr(temp, argv[i + 2]))
                {
                    count++;
                    temp++;
                }
                counts[i] += count;
            }
            length = -1;
            continue;
        }
        if (length == size){
            size = size * 2;
            realloc(buffer,size * sizeof(char));
        }
        buffer[length] = next;
    }
    if (length == size){
        size = size + 1;
        realloc(buffer,size * sizeof(char));
    }
    buffer[length] = '\0';
    for(i = 0; i < numSubs; i++){
        int count = 0;
        const char *temp = buffer;
        while(temp = strcasestr(temp, argv[i + 2]))
        {
            count++;
            temp++;
        }
        counts[i] += count;
    }
    for(i = 0; i < numSubs; i++){
        printf("%d\n",counts[i]);
    }

}