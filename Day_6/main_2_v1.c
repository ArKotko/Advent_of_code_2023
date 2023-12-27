#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

    //Gets the time of the race
    char currentChar, currentString[10];
    char* raceTime_string, *distanceRecord_string;
    unsigned long long raceTime = 0, distanceRecord = 0;
    raceTime_string = (char*) malloc(sizeof(char) * 2);
    strcpy(raceTime_string, "");
    do{
        fscanf(input, "%*[^0-9]");
        fscanf(input, "%[0-9]", currentString);
        raceTime_string = (char*) realloc(raceTime_string, sizeof(char) * (strlen(currentString) + 1));
        strcat(raceTime_string, currentString);
        currentChar = fgetc(input);
    }while(currentChar != '\n');
    raceTime = strtoll(raceTime_string, NULL, 10);
    //Gets the distance distance record of the race
    distanceRecord_string = (char*) malloc(sizeof(char) * 2);
    strcpy(distanceRecord_string, "");
    do{
        fscanf(input, "%*[^0-9]");
        fscanf(input, "%[0-9]", currentString);
        distanceRecord_string = (char*) realloc(distanceRecord_string, sizeof(char) * (strlen(currentString) + 1));
        strcat(distanceRecord_string, currentString);
        currentChar = fgetc(input);
    }while(currentChar != EOF);
    distanceRecord = strtoll(distanceRecord_string, NULL, 10);

    //Gets the number of ways you can win the race
    unsigned long long it_higherDistance = 0, it_maxDistance, currentDistance, nbWaysToWinRace;
    //Calculates the first iteration where the record is beaten
    do{
        ++it_higherDistance;
        currentDistance = (raceTime - it_higherDistance) * it_higherDistance;
    }while(distanceRecord >= currentDistance);
    //Calculates the iteration where the maximum distance is going to be reached
    it_maxDistance = (raceTime) / 2;
    //Calculates the number of ways you can beat the distanceRecord on this race
    nbWaysToWinRace = ((it_maxDistance - it_higherDistance) + 1) * 2;
    //Subtracts one if the raceTime is even
    if((raceTime % 2) == 0)
        --nbWaysToWinRace;

    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &endTime);
    long executionraceTime_nsec = endTime.tv_nsec - startTime.tv_nsec;
    double executionraceTime_mlsec = executionraceTime_nsec / 1000000.0;
    printf("\033[1;37mExecution time (without prints) :\033[1;31m %lf ms\n", executionraceTime_mlsec);

    //Prints the number
    printf("\033[1;37mIn total, you have \033[1;34m%lld\033[1;37m different ways to win all the races.\033[0m\n", nbWaysToWinRace);

    return 0;
}