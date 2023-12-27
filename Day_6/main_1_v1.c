#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct race{
    unsigned int time;
    unsigned int distanceRecord;
}Race;

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

    //Gets the time of each race
    char currentChar;
    unsigned int nbRaces = 0, currentTime = 0, currentRecord = 0;
    Race* racesArray = NULL;
    do{
        ++nbRaces;
        fscanf(input, "%*[^0-9]");
        fscanf(input, "%d", &currentTime);
        racesArray = (Race*) realloc(racesArray, sizeof(Race) * nbRaces);
        racesArray[nbRaces - 1].time = currentTime;
        currentChar = fgetc(input);
    }while(currentChar != '\n');
    //Gets the distance record of each race
    for(unsigned int i = 0; i < nbRaces; ++i){
        fscanf(input, "%*[^0-9]");
        fscanf(input, "%d", &currentRecord);
        racesArray[i].distanceRecord = currentRecord;
        currentChar = fgetc(input);
    }

    //Gets the number of ways you can win each race
    unsigned int it_higherDistance, it_maxDistance, currentDistance = 0, nbWaysToWin_currentRace = 0, nbWaysToWin_total = 1;
    for(unsigned int i = 0; i < nbRaces; ++i){
        it_higherDistance = 0;
        //Calculates the first iteration where the record is beaten
        do{
            ++it_higherDistance;
            currentDistance = (racesArray[i].time - it_higherDistance) * it_higherDistance;
        }while(racesArray[i].distanceRecord >= currentDistance);
        //Calculates the iteration where the maximum distance is going to be reached
        it_maxDistance = ((racesArray[i].time) / 2);
        //Calculates the number of ways you can beat the record on this race
        nbWaysToWin_currentRace = ((it_maxDistance - it_higherDistance) + 1) * 2;
        //Subtracts one if the raceTime is even
        if((racesArray[i].time % 2) == 0)
            --nbWaysToWin_currentRace;
        nbWaysToWin_total = nbWaysToWin_total * nbWaysToWin_currentRace;
    }

    //Frees the memory
    free(racesArray);

    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &endTime);
    long executionraceTime_nsec = endTime.tv_nsec - startTime.tv_nsec;
    double executionraceTime_mlsec = executionraceTime_nsec / 1000000.0;
    printf("\033[1;37mExecution time (without prints) :\033[1;31m %lf ms\n", executionraceTime_mlsec);

    //Prints the number
    printf("\033[1;37mIn total, you have \033[1;34m%d\033[1;37m different ways to win all the races.\033[0m\n", nbWaysToWin_total);

    return 0;
}