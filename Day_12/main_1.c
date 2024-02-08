#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

typedef struct group{
    size_t size;
    size_t minSizeAfter;
    unsigned long long currentPosition; //Last cell of the list occupied by the group
    unsigned long long currentAssociatedValue; //The decimal value associated to the binary representation of the group
}Group;

unsigned long long springsToBits(char* springsList, size_t nbSprings);
unsigned long long bitMaskGenerator(char* springsList, size_t nbSprings);
unsigned long long binaryToDecimal(unsigned long long startBit, unsigned long long lastBit);
void arrangementsGenerator(Group* groupList, size_t nbGroups, unsigned long long groupsValueSoFar, unsigned long long knownSpringsValue, unsigned long long bitMask, unsigned long long* finalSum);

int main(int argc, char* argv[]){

    struct timespec startTime;
    clock_gettime(CLOCK_REALTIME, &startTime);

    //Checks the function arguments
    if(argc != 2){
        printf("Not enough arguments !\n");
        return 1;
    }

    //Opens the file
    FILE* input = fopen(argv[1], "r");
    if(!input){
        printf("Failed to open file.\n");
        return 2;
    }

    char currentChar;

    char* springsList; //The list of springs given in the input
    size_t nbSprings;
    Group* groupList; //The list of contiguous groups given in the input
    size_t nbGroups;

    unsigned int previousInitialPosition;

    unsigned long long bitMask; //To only keep the springs of which the position is known
    unsigned long long knownSpringsValue; //The decimal value associated to the binary representation of the springs of which the position is known 

    unsigned long long finalSum = 0;

    //Deals with the lines of the file one by one
    while(1){

        nbSprings = 0;
        springsList = (char*) malloc(sizeof(char) * 1);

        //Stores the springs
        while(1){

            currentChar = fgetc(input); //Reads a character

            //If it is the end of the list of springs, ends the loop
            if(currentChar == ' ')
                break;

            ++nbSprings; //One more spring
            springsList = (char*) realloc(springsList, (sizeof(char) * nbSprings));
            springsList[nbSprings - 1] = currentChar;
        }

        //Calculates the decimal value associated to the binary representation of the springs of which the condition is known
        knownSpringsValue = springsToBits(springsList, nbSprings);
        
        //Generates the bit mask of the list of springs ('?' = 0 ; '.' and '#' = 1)
        bitMask = bitMaskGenerator(springsList, nbSprings);

        nbGroups = 0;
        groupList = (Group*) malloc(sizeof(Group) * 1);

        //Stores the contiguous groups
        while(1){

            ++nbGroups; //One more group
            groupList = (Group*) realloc(groupList, (sizeof(Group) * nbGroups));
            fscanf(input, "%lld", &(groupList[nbGroups - 1].size)); //Reads a value

            currentChar = fgetc(input); //Reads a character

            //If it is the end of the list of the contiguous groups or the end of the file, ends the loop
            if((currentChar == '\n') || (currentChar == EOF))
                break;
        }

        //Calculates the number of cells required after each group
        for(size_t i = 0; i < nbGroups; ++i){

            groupList[i].minSizeAfter = 0;

            for(size_t j = (i + 1); j < nbGroups; ++j)
                groupList[i].minSizeAfter += (1 + groupList[j].size);
        }

        //Calculates the initial position and the initial associated value of each group
        previousInitialPosition = nbSprings + 1;

        for(size_t i = 0; i < nbGroups; ++i){

            groupList[i].currentPosition = previousInitialPosition - groupList[i].size - 1;
            previousInitialPosition = groupList[i].currentPosition; 
            groupList[i].currentAssociatedValue = binaryToDecimal(groupList[i].currentPosition, (groupList[i].currentPosition + (groupList[i].size - 1)));
        }

        //Generates all the different arrangements and finds those who match the positions of the springs of which the condition is known
        arrangementsGenerator(groupList, nbGroups, 0, knownSpringsValue, bitMask, &finalSum);

        //Frees the memory
        free(springsList);
        free(groupList);
        springsList = NULL;
        groupList = NULL;

        //If it is the end of the file, ends the loop
        if(currentChar == EOF)
            break;
    }

    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &endTime);
    long executionraceTime_nsec = endTime.tv_nsec - startTime.tv_nsec;
    double executionraceTime_mlsec = executionraceTime_nsec / 1000000.0;
    printf("\033[1;37mExecution time (without prints) : \033[1;31m%lf ms\n", executionraceTime_mlsec);

    //Prints the number
    printf("\033[1;37mHere is the sum of all the counts : \033[1;34m%lld\033[0m\n", finalSum);
}

unsigned long long springsToBits(char* springsList, size_t nbSprings){

    if((springsList == NULL) || (nbSprings == 0))
        return 0;

    unsigned long long value = 0;

    for(int currentSpring = (nbSprings - 1); currentSpring >= 0; --currentSpring){
        if(springsList[currentSpring] == '#')
            value += (unsigned long long)pow(2, ((nbSprings - 1) - currentSpring));
    }
        
    return value;
}

unsigned long long bitMaskGenerator(char* springsList, size_t nbSprings){

    if((springsList == NULL) || (nbSprings == 0))
        return 0;

    unsigned long long value = 0;

    for(int currentSpring = (nbSprings - 1); currentSpring >= 0; --currentSpring){
        if((springsList[currentSpring] == '#') || (springsList[currentSpring] == '.'))
            value += (unsigned long long)pow(2, ((nbSprings - 1) - currentSpring));
    }
        
    return value;
}

unsigned long long binaryToDecimal(unsigned long long startBit, unsigned long long lastBit){

    if(startBit > lastBit)
        return 0;

    unsigned long long value = 0;

    for(unsigned long long currentBit = startBit; currentBit <= lastBit; ++currentBit)
        value += (unsigned long long)pow(2, currentBit);

    return value;
}

void arrangementsGenerator(Group* groupList, size_t nbGroups, unsigned long long groupsValueSoFar, unsigned long long knownSpringsValue, unsigned long long bitMask, unsigned long long* finalSum){

    unsigned long long endCell = groupList->minSizeAfter;

    //If it is the last group of the list, performs the calculations
    if(nbGroups == 1){
        unsigned long long currentGroupValue;

        while(1){
            currentGroupValue = groupsValueSoFar + (groupList->currentAssociatedValue); //The decimal value associated to the binary representation of the group
            //Applies the bit mask to the current group value and increments the final sum according to the result
            if((currentGroupValue & bitMask) == knownSpringsValue)
                ++(*finalSum);
            //If the group has reached its final position, ends the loop
            if((groupList->currentPosition) == endCell)
                break;
            else{
                --(groupList->currentPosition); //Shifts the group one cell to the right
                (groupList->currentAssociatedValue) = (groupList->currentAssociatedValue) >> 1; //Updates the current associated value of the group
            }
        }
    }

    //If it is not, calls the function again
    else{
        unsigned long long* previousPositions = (unsigned long long*) malloc(sizeof(unsigned long long) * 1);
        unsigned long long* previousAssociatedValues = (unsigned long long*) malloc(sizeof(unsigned long long) * 1);
        unsigned long long previousGroupsValueSoFar;

        while(1){
            //Stores the current position and the associated value of the next groups
            for(size_t i = 1; i <= (nbGroups - 1); ++i){
                previousPositions = (unsigned long long*) realloc(previousPositions, (sizeof(unsigned long long) * i));
                previousAssociatedValues = (unsigned long long*) realloc(previousAssociatedValues, (sizeof(unsigned long long) * i));
                previousPositions[i - 1] = (groupList + i)->currentPosition; 
                previousAssociatedValues[i - 1] = (groupList + i)->currentAssociatedValue; 
            }
            previousGroupsValueSoFar = groupsValueSoFar; //Stores the current total value of the groups
            groupsValueSoFar += (groupList->currentAssociatedValue); //Adds the associated value of the current group to the total value of the groups
            arrangementsGenerator((groupList + 1), (nbGroups - 1), groupsValueSoFar, knownSpringsValue, bitMask, finalSum);
            groupsValueSoFar = previousGroupsValueSoFar; //Restores the previous total value of the groups
            for(size_t i = 1; i <= (nbGroups - 1); ++i){
                (groupList + i)->currentPosition = (previousPositions[i - 1] - 1); //Restores the previous position of the next groups and shifts them one cell to the right
                (groupList + i)->currentAssociatedValue = (previousAssociatedValues[i - 1] >> 1); //Restores the previous associated value of the next groups and updates them 
            }
            //Frees the memory
            free(previousPositions);
            free(previousAssociatedValues);
            previousPositions = NULL;
            previousAssociatedValues = NULL;
            //If the group has reached its final position, ends the loop
            if((groupList->currentPosition) == endCell)
                break;
            else{
                --(groupList->currentPosition); //Shifts the group one cell to the right
                (groupList->currentAssociatedValue) = (groupList->currentAssociatedValue) >> 1; //Updates the current associated value of the group 
            }
        }
    }
}