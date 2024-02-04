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

    //Counts the number of rows in the file
    unsigned int nbRows;

    fscanf(input, "%*[^\n]");
    nbRows = ftell(input) + 2;

    //Counts the number of lines in the file
    int nbLines;

    fseek(input, 0, SEEK_END);
    nbLines = (ftell(input) / nbRows) + 1;

    int fileSize = (nbRows * nbLines) - 2;

    fseek(input, 0, SEEK_SET);

    //Finds the starting point in the map
    unsigned int startingPointPosition;

    fscanf(input, "%*[^S]");
    startingPointPosition = ftell(input) + 1;

    //Finds the two pipes connecting to the starting point
    char currentChar;
    char previousDirection[2]; //'N' = north ; 'S' = south ; 'W' = west ; 'E' = east
    unsigned short pipesFound = 0;
    unsigned int previousPipesPosition[2];

    //Goes above the starting point (if possible)
    if((startingPointPosition - nbRows) >= 0){
        fseek(input, (startingPointPosition - nbRows - 1), SEEK_SET); 
        currentChar = fgetc(input);
        if((currentChar == 'F') || (currentChar == '|') || (currentChar == '7')){
            previousPipesPosition[pipesFound] = ftell(input) - 1;
            previousDirection[pipesFound] = 'S';
            ++pipesFound;
        }
    }
    //Goes below the starting point (if possible)
    if((startingPointPosition + nbRows) <= fileSize){
        fseek(input, (startingPointPosition + nbRows - 1), SEEK_SET);  
        currentChar = fgetc(input);
        if((currentChar == 'L') || (currentChar == '|') || (currentChar == 'J')){
            previousPipesPosition[pipesFound] = ftell(input) - 1;
            previousDirection[pipesFound] = 'N';
            ++pipesFound;
        }
    }
    //Goes before the starting point (if needed AND possible)
    if((pipesFound != 2) && ((startingPointPosition - 1) >= 0)){
        fseek(input, (startingPointPosition - 1), SEEK_SET);
        currentChar = fgetc(input);
        if((currentChar == 'F') || (currentChar == 'L') || (currentChar == '-')){
            previousPipesPosition[pipesFound] = ftell(input) - 1;
            previousDirection[pipesFound] = 'E';
            ++pipesFound;
        }
    }
    //Goes after the starting point (if needed AND possible)
    if((pipesFound != 2) && ((startingPointPosition + 1) <= fileSize)){
        fseek(input, (startingPointPosition + 1) - 1, SEEK_SET);
        currentChar = fgetc(input);
        if((currentChar == '-') || (currentChar == 'J') || (currentChar == '7')){
            previousPipesPosition[pipesFound] = ftell(input) - 1;
            previousDirection[pipesFound] = 'W';
            ++pipesFound;
        }
    }

    //Creates two file pointers, each one pointing to a different pipe
    FILE* pipePointers[2];

    pipePointers[0] = fopen(argv[1], "r");
    pipePointers[1] = fopen(argv[1], "r");

    fseek(pipePointers[0], previousPipesPosition[0], SEEK_SET);
    fseek(pipePointers[1], previousPipesPosition[1], SEEK_SET);

    //Goes through the tunnel in both directions at the same time
    //and stops when the two pointers point to the same position in the file
    char currentPipe;
    unsigned int movementOffset;
    unsigned int nbSteps = 1;

    while(1){

        for(unsigned int i = 0; i < 2; ++i){

            movementOffset = 0; //Sets the movement offset back to 0

            //Gets the current pipe
            currentPipe = fgetc(pipePointers[i]);

            //Moves to the next pipe according to the current pipe and the previous direction
            switch (currentPipe){
                //North <-> south
                case '|':
                    //If we come from the north
                    if(previousDirection[i] == 'N')
                        movementOffset = nbRows - 1;
                    //If we come from the south
                    else
                        movementOffset = -nbRows - 1;
                    break;
                //East <-> west
                case '-':
                    //If we come from the west, nothing needs to be done
                    //If we come from the east
                    if(previousDirection[i] == 'E')
                        movementOffset = -2;
                    break;
                //North <-> east
                case 'L':
                    //If we come from the north
                    if(previousDirection[i] == 'N')
                        previousDirection[i] = 'W';
                    //If we come from the east
                    if(previousDirection[i] == 'E'){
                        movementOffset = -nbRows - 1;
                        previousDirection[i] = 'S';
                    }
                    break;
                //North <-> west
                case 'J':
                    //If we come from the north
                    if(previousDirection[i] == 'N'){
                        movementOffset = -2;
                        previousDirection[i] = 'E';
                    }
                    //If we come from the west
                    if(previousDirection[i] == 'W'){
                        movementOffset = -nbRows - 1;
                        previousDirection[i] = 'S';
                    }    
                    break;            
                //South <-> west
                case '7':
                    //If we come from the south
                    if(previousDirection[i] == 'S'){
                        movementOffset = -2;
                        previousDirection[i] = 'E';
                    }
                    //If we come from the west
                    if(previousDirection[i] == 'W'){
                        movementOffset = nbRows - 1;
                        previousDirection[i] = 'N';
                    }
                    break;
                //South <-> east
                case 'F':
                    //If we come from the south
                    if(previousDirection[i] == 'S')
                        previousDirection[i] = 'W';
                    //If we come from the east
                    if(previousDirection[i] == 'E'){
                        movementOffset = nbRows - 1;
                        previousDirection[i] = 'N';
                    }
                    break;
            }

            //Saves the position of the pointer
            previousPipesPosition[i] = ftell(pipePointers[i]);

            //Moves the pointer
            fseek(pipePointers[i], movementOffset, SEEK_CUR);
        }

        //If both pointers point to the same position in the file, ends the loop
        if(previousPipesPosition[0] == previousPipesPosition[1])
            break;
        else
            ++nbSteps;
    } 

    //Closes the file
    fclose(input);
    fclose(pipePointers[0]);
    fclose(pipePointers[1]);

    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &endTime);
    long executionraceTime_nsec = endTime.tv_nsec - startTime.tv_nsec;
    double executionraceTime_mlsec = executionraceTime_nsec / 1000000.0;
    printf("\033[1;37mExecution time (without prints) : \033[1;31m%lf ms\n", executionraceTime_mlsec);

    //Prints the number
    printf("\033[1;37mHere is the number of steps required to go to the farthest point from the starting position : \033[1;34m%d\033[0m\n", nbSteps);
}