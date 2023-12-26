#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct block{
    unsigned long long int startNum;
    unsigned long long int length;
    struct block* previous;
    struct block* next;
}Block;

typedef Block* List;

Block* createBlock(unsigned long long sN, unsigned long long l){
    Block* new = (Block*) malloc(sizeof(Block));
    if(!new){
        printf("Failed to create the block.\n");
        return NULL;
    }
    new->startNum = sN;
    new->length = l;
    new->previous = NULL;
    new->next = NULL;
    return new;
} 

List addBlock(List list, Block* block){
    //If the list is empty
    if(!list)
        return block;
    //Saves the list
    List originalList = list;
    //Goes to the last block of the list
    while(list->next)
        list = list->next;
    list->next = block;
    block->previous = list;
    return originalList;
}

List deleteBlock(List list, Block* block){
    if((block->previous) && (block->next)){
        block->previous->next = block->next;
        block->next->previous = block->previous;
    }
    else if(block->previous)
        block->previous->next = NULL;
    else if(block->next){
        block->next->previous = NULL;
        //The first block of the list changes
        list = block->next;
    }
    else
        list = NULL;
    free(block);
    return list;
}

void setOverlap(List list, Block* list_Set, unsigned long long int firstNum_SourceSet_Map, unsigned long long int lastNum_SourceSet_Map){
    //Calculates the last number of the current set
    unsigned long long int lastNum_SourceSet_List = list_Set->startNum + (list_Set->length - 1);
    //If the set of the list is included in the source set, do nothing
    //If only a part of the set of the list is included in the source set
    if((list_Set->startNum < firstNum_SourceSet_Map) && ((lastNum_SourceSet_List >= firstNum_SourceSet_Map) && (lastNum_SourceSet_List <= lastNum_SourceSet_Map))){
        list = addBlock(list, createBlock(list_Set->startNum, (firstNum_SourceSet_Map - (list_Set->startNum))));
        list_Set->startNum = firstNum_SourceSet_Map;
        list_Set->length = (lastNum_SourceSet_List - firstNum_SourceSet_Map) + 1;
    }
    else if((lastNum_SourceSet_List > lastNum_SourceSet_Map) && ((list_Set->startNum >= firstNum_SourceSet_Map) && (list_Set->startNum <= lastNum_SourceSet_Map))){
        list = addBlock(list, createBlock((lastNum_SourceSet_Map + 1), (lastNum_SourceSet_List - lastNum_SourceSet_Map)));
        list_Set->length = (lastNum_SourceSet_Map - list_Set->startNum) + 1;
    }
    //If the source set is included in the set of the list
    else if((list_Set->startNum < firstNum_SourceSet_Map) && (lastNum_SourceSet_List > lastNum_SourceSet_Map)){
        list = addBlock(list, createBlock(list_Set->startNum, (firstNum_SourceSet_Map - (list_Set->startNum))));
        list = addBlock(list, createBlock((lastNum_SourceSet_Map + 1), (lastNum_SourceSet_List - lastNum_SourceSet_Map)));
        list_Set->startNum = firstNum_SourceSet_Map;
        list_Set->length = (lastNum_SourceSet_Map - firstNum_SourceSet_Map) + 1;        
    }
}

int main(int argc, char* argv[]){

    struct timespec startTime;
    clock_gettime(CLOCK_REALTIME, &startTime);

    //Checks the function arguments
    if(argc != 2){
        printf("Not enough arguments !");
        return 1;
    }

    //Opens the file
    FILE* input = fopen(argv[1], "r");
    if(!input){
        printf("Failed to open file.\n");
        return 2;
    }

    //Gets the range of seeds and store them
    List sourceList = NULL;
    char currentChar;
    unsigned long long int currentStartNum, currentLength;
    //Ignores the word "seed" and the ':'
    fscanf(input, "%*[^ ]");
    while(1){
        //Gets the next char
        currentChar = fgetc(input);
        //If it is the end of the line, the loop stops
        if(currentChar == '\n')
            break;
        //Stores the range of seeds
        fscanf(input, "%lld %lld", &currentStartNum, &currentLength);
        //Creates the block in the list
        sourceList = addBlock(sourceList, createBlock(currentStartNum, currentLength));
    }

    //Stores all the destination sets
    unsigned long long firstNum_SourceSet_Map = 0, lastNum_SourceSet_Map = 0, firstNum_DestSet_Map = 0, lengthMapSet = 0;
    unsigned long long lastNum_SourceSet_List = 0;
    unsigned long long firstNum_DestSet_List = 0;
    List destList = NULL;
    List currentSet_sourceList = NULL;
    Block* previousSet_sourceList = NULL;
 
    //Goes through all the file
    while(!feof(input)){
        //Ignores everything except numbers
        fscanf(input, "%*[^0-9]");
        //Goes trough each map
        while(1){
            //Sets the "cursor" back to the start of the source list
            currentSet_sourceList = sourceList;
            //Stores the first number of the destination set, the first number of the source set and the range of the sets
            fscanf(input, "%lld %lld %lld", &firstNum_DestSet_Map, &firstNum_SourceSet_Map, &lengthMapSet);
            //Calculates the last number of the current source set
            lastNum_SourceSet_Map = firstNum_SourceSet_Map + (lengthMapSet - 1);
            //For each block in the sourceList
            while(currentSet_sourceList){
                //Calculates the last number of the current set of the list
                lastNum_SourceSet_List = currentSet_sourceList->startNum + (currentSet_sourceList->length - 1);            
                //Checks if the current set of the list overlaps with the source set of the map
                if((currentSet_sourceList->startNum <= lastNum_SourceSet_Map) && (lastNum_SourceSet_List >= firstNum_SourceSet_Map)){
                    //Changes the current set of the list if neeeded
                    setOverlap(sourceList, currentSet_sourceList, firstNum_SourceSet_Map, lastNum_SourceSet_Map);
                    //Calculates the first number of the destination set
                    firstNum_DestSet_List = firstNum_DestSet_Map + (currentSet_sourceList->startNum - firstNum_SourceSet_Map);
                    //Adds the destination set in the destination list
                    destList = addBlock(destList, createBlock(firstNum_DestSet_List, currentSet_sourceList->length));
                    //Saves the current block
                    previousSet_sourceList = currentSet_sourceList;
                    //Goes to the next set in the list
                    currentSet_sourceList = currentSet_sourceList->next;
                    //Deletes the block we just dealt with
                    sourceList = deleteBlock(sourceList, previousSet_sourceList);
                }
                else
                    currentSet_sourceList = currentSet_sourceList->next;
            }
            //Ignores the next character
            fgetc(input);
            //Stores the next character
            currentChar = fgetc(input);
            //If it is the end of the map or of the file, the loop stops
            if((currentChar == '\n') || (currentChar == EOF))
                break;
            //If it is a number, it goes back to the previous character
            else
                fseek(input, -1, SEEK_CUR);
        }
        //Adds all the sets that have no matching set in the map to the end of the destination list
        if(sourceList)
            destList = addBlock(destList, sourceList);
        //Makes the destList the new sourceList
        sourceList = destList;
        //"Empties" the destList
        destList = NULL;     
    }

    //Finds the lowest starting number of all the sets in the final source list
    unsigned long long int lowestDestination = sourceList->startNum;
    sourceList = sourceList->next;
    while(sourceList){
        if(sourceList->startNum < lowestDestination)
            lowestDestination = sourceList->startNum;
        sourceList = sourceList->next;
    }

    //Prints the number
    printf("\033[1;37mHere is the lowest location number :\033[1;34m %lld \033[0m\n", lowestDestination);

    //Free the sourceList
    Block* nextSet_sourceList = NULL;
    while(sourceList){
        nextSet_sourceList = sourceList->next;
        free(sourceList);
        sourceList = nextSet_sourceList;
    }

    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &endTime);
    long executionTime_nsec = endTime.tv_nsec - startTime.tv_nsec;
    double executionTime_mlsec = executionTime_nsec / 1000000.0;
    printf("\033[1;37mExecution time :\033[1;31m %lf ms\033[0m\n", executionTime_mlsec);

    return 0;
}