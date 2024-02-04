#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct pipe{
    char tile;
    unsigned int position;
}Pipe;

void bubbleSort(Pipe* array, size_t size);

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
    int nbRows;

    fscanf(input, "%*[^\n]");
    nbRows = ftell(input) + 2;

    //Counts the number of lines in the file
    int nbLines;

    fseek(input, 0, SEEK_END);
    nbLines = (ftell(input) / nbRows) + 1;

    int fileSize = (nbRows * nbLines) - 2;

    fseek(input, 0, SEEK_SET);

    //Finds the starting point in the map
    int startingPointPosition;

    fscanf(input, "%*[^S]");
    startingPointPosition = ftell(input) + 1;

    //Finds one of the two pipes connecting to the starting point
    char currentChar;
    char previousDirection; //'N' = north ; 'S' = south ; 'W' = west ; 'E' = east
    unsigned short pipeFound = 0;
    unsigned int movementOffset;

    //Goes above the starting point (if possible)
    if((startingPointPosition - nbRows) >= 0){
        fseek(input, (startingPointPosition - nbRows - 1), SEEK_SET); 
        currentChar = fgetc(input);
        if((currentChar == 'F') || (currentChar == '|') || (currentChar == '7')){
            movementOffset = ftell(input) - 1;
            previousDirection = 'S';
            pipeFound = 1;
        }
    }
    //Goes below the starting point (if needed AND possible)
    if((!pipeFound) && ((startingPointPosition + nbRows) <= fileSize)){
        fseek(input, (startingPointPosition + nbRows - 1), SEEK_SET); 
        currentChar = fgetc(input);
        if((currentChar == 'L') || (currentChar == '|') || (currentChar == 'J')){
            movementOffset = ftell(input) - 1;
            previousDirection = 'N';
            pipeFound = 1;
        }
    }
    //Goes before the starting point (if needed AND possible)
    if((!pipeFound) && ((startingPointPosition - 1) >= 0)){
        fseek(input, (startingPointPosition - 1), SEEK_SET);
        currentChar = fgetc(input);
        if((currentChar == 'F') || (currentChar == 'L') || (currentChar == '-')){
            movementOffset = ftell(input) - 1;
            previousDirection = 'E';
            pipeFound = 1;
        }
    }
    //Goes after the starting point (if needed AND possible)
    if((!pipeFound) && ((startingPointPosition + 1) <= fileSize)){
        fseek(input, (startingPointPosition + 1) - 1, SEEK_SET);
        currentChar = fgetc(input);
        if((currentChar == '-') || (currentChar == 'J') || (currentChar == '7')){
            movementOffset = ftell(input) - 1;
            previousDirection = 'W';
            pipeFound = 1;
        }
    }

    fseek(input, movementOffset, SEEK_SET); //Places the pointer on the pipe that has been found

    //Goes through the tunnel
    char currentPipe;
    unsigned int currentLineInFile;
    unsigned int nbPipesLoop = 1;

    //Two-dimensional array to store the pipes (position and tile)
    Pipe** pipes = (Pipe**) malloc(sizeof(Pipe*) * nbLines);
    for(unsigned int i = 0; i < nbLines; ++i){
        pipes[i] = NULL;
    }

    //Array to store the length of each line in the previous array
    size_t* linesLength = (size_t*) malloc(sizeof(size_t) * nbLines);
    for(unsigned int i = 0; i < nbLines; ++i){
        linesLength[i] = 0;
    }

    //Stores the starting point
    currentLineInFile = (startingPointPosition / nbRows) + 1;
    pipes[currentLineInFile - 1] = (Pipe*) malloc(sizeof(Pipe) * 1);
    pipes[currentLineInFile - 1][0].position = startingPointPosition; //Stores the position of starting point
    pipes[currentLineInFile - 1][0].tile = 'S'; //Stores the tile of the starting point
    ++linesLength[currentLineInFile - 1];

    while(1){

        movementOffset = 0; //Sets the movement offset back to 0

        //Gets the current pipe
        currentPipe = fgetc(input);

        //Finds the position of the next pipe according to the current pipe and the previous direction
        switch (currentPipe){
            //North <-> south
            case '|':
                //If we come from the north
                if(previousDirection == 'N')
                    movementOffset = nbRows - 1;
                //If we come from the south
                else
                    movementOffset = -nbRows - 1;
                break;
            //East <-> west
            case '-':
                //If we come from the west, nothing needs to be done
                //If we come from the east
                if(previousDirection == 'E')
                    movementOffset = -2;
                break;
            //North <-> east
            case 'L':
                //If we come from the north
                if(previousDirection == 'N')
                    previousDirection = 'W';
                //If we come from the east
                if(previousDirection == 'E'){
                    movementOffset = -nbRows - 1;
                    previousDirection = 'S';
                }
                break;
            //North <-> west
            case 'J':
                //If we come from the north
                if(previousDirection == 'N'){
                    movementOffset = -2;
                    previousDirection = 'E';
                }
                //If we come from the west
                if(previousDirection == 'W'){
                    movementOffset = -nbRows - 1;
                    previousDirection = 'S';
                }    
                break;            
            //South <-> west
            case '7':
                //If we come from the south
                if(previousDirection == 'S'){
                        movementOffset = -2;
                        previousDirection = 'E';
                    }
                //If we come from the west
                if(previousDirection == 'W'){
                    movementOffset = nbRows - 1;
                    previousDirection = 'N';
                }
                break;
            //South <-> east
            case 'F':
                //If we come from the south
                if(previousDirection == 'S')
                    previousDirection = 'W';
                //If we come from the east
                if(previousDirection == 'E'){
                    movementOffset = nbRows - 1;
                    previousDirection = 'N';
                }
                break;
            default:
                break;
        }

        //If the pointer is back to 'S', ends the loop
        if(currentPipe == 'S')
            break;
        else
            ++nbPipesLoop;

        //Finds on which line of the file the current pipe is located
        currentLineInFile = (ftell(input) / nbRows) + 1;
        ++linesLength[currentLineInFile - 1];

        //Stores the position of the current pipe in the array
        pipes[currentLineInFile - 1] = (Pipe*) realloc(pipes[currentLineInFile - 1], (sizeof(Pipe) * linesLength[currentLineInFile - 1]));
        
        pipes[currentLineInFile - 1][linesLength[currentLineInFile - 1] - 1].position = ftell(input);
        pipes[currentLineInFile - 1][linesLength[currentLineInFile - 1] - 1].tile = currentPipe;

        //Moves the pointer
        fseek(input, movementOffset, SEEK_CUR);
    }

    //Orders the lines of the array
    for(unsigned int i = 0; i < nbLines; ++i)
        bubbleSort(pipes[i], linesLength[i]);

    //Adds a "dummy pipe" at the end of each line
    for(unsigned int i = 0; i < nbLines; ++i){
        if(pipes[i] != NULL){
            ++linesLength[i];
            pipes[i] = (Pipe*) realloc(pipes[i], (sizeof(Pipe) * linesLength[i]));
            pipes[i][linesLength[i] - 1].position = 0;
            pipes[i][linesLength[i] - 1].tile = 'X';
        }
    }

    //Frees the array containing the length of the lines
    free(linesLength);
    linesLength = NULL;

    //Prints to check that everything is alright
    /*unsigned int x;

    printf("\n\n");

    for(unsigned int i = 0; i < nbLines; ++i){
        printf("\033[1;34mLine %d\033[0m : ", i);
        x = 0;
        if(pipes[i] != NULL){
        while(pipes[i][x].tile != 'X'){
            printf("\033[1;37m%d\033[0m [%c] ", pipes[i][x].position, pipes[i][x].tile);
            ++x;
        }
        }
        printf("\n\n");
    }*/

    //Creates an array with the tiles of which the position (inside our outside the loop) needs to be determined
    unsigned int j;
    unsigned int currentPosition;
    unsigned int lengthCurrentLine;
    unsigned int** tilesPosition = (unsigned int**) malloc(sizeof(unsigned int*) * nbLines);
    for(unsigned int i = 0; i < nbLines; ++i){
        tilesPosition[i] = NULL;
    }

    for(unsigned int i = 0; i < nbLines; ++i){
        if(pipes[i] != NULL){
            j = 0;
            lengthCurrentLine = 0;  
            currentPosition = pipes[i][j].position;
            while(pipes[i][j].tile != 'X'){
                if(pipes[i][j].position == currentPosition)
                    ++j;
                else{
                    ++lengthCurrentLine;
                    tilesPosition[i] = (unsigned int*) realloc(tilesPosition[i], (sizeof(unsigned int) * lengthCurrentLine));
                    tilesPosition[i][lengthCurrentLine - 1] = currentPosition;
                }
                ++currentPosition;
            }
        }
        //Adds a "dummy pipe" at the end of the line
        if(tilesPosition[i] != NULL){
            ++lengthCurrentLine;
            tilesPosition[i] = (unsigned int*) realloc(tilesPosition[i], (sizeof(unsigned int) * lengthCurrentLine));
            tilesPosition[i][lengthCurrentLine - 1] = 0;
        }
    }

    //Prints to check that everything is alright
    /*unsigned int y;

    printf("\n\n");

    for(unsigned int i = 0; i < nbLines; ++i){
        printf("\033[1;34mLine %d\033[0m : ", i);
        y = 0;
        if(tilesPosition[i] != NULL){
        while(tilesPosition[i][y]){
            printf("\033[1;37m%d\033[0m ", tilesPosition[i][y]);
            ++y;
        }
        }
        printf("\n\n");
    }*/
    
    //Determines the position (inside our outside the loop) of the tiles in the array
    char previousTile;
    unsigned int idNextPipe;
    unsigned int idCurrentTile;
    unsigned int nbIntersections;
    unsigned int nbConsecutiveTiles;
    unsigned int nbEnclosedTiles = 0;

    for(unsigned int currentLine = 0; currentLine < nbLines; ++currentLine){

        idCurrentTile = 0;

        if(tilesPosition[currentLine] != NULL){

            while(tilesPosition[currentLine][idCurrentTile] != 0){

                //Finds if there are any consecutive tiles
                nbConsecutiveTiles = 1;

                while((tilesPosition[currentLine][idCurrentTile] != 0) && (tilesPosition[currentLine][idCurrentTile] == (tilesPosition[currentLine][idCurrentTile + 1] - 1))){
                    ++nbConsecutiveTiles;
                    ++idCurrentTile;
                }

                idNextPipe = 0;

                //Finds the pipe belonging to the loop that follows the current tile
                while(pipes[currentLine][idNextPipe].position <= tilesPosition[currentLine][idCurrentTile])
                    ++idNextPipe;

                //Counts the number of intersections with the borders of the loop at the right of the current tile
                previousTile = '.';
                nbIntersections = 0;

                while(pipes[currentLine][idNextPipe].tile != 'X'){

                    switch (pipes[currentLine][idNextPipe].tile){
                    case '7':
                        if(previousTile == 'L')
                            break;
                        else
                            ++nbIntersections;
                        break;
                    case 'J':
                        if(previousTile == 'F')
                            break;
                        else
                            ++nbIntersections;
                        break;
                    case '-':
                        break;
                    default:
                        ++nbIntersections;
                        break;
                    }

                    if(pipes[currentLine][idNextPipe].tile != '-')
                        previousTile = pipes[currentLine][idNextPipe].tile;
                    else if(pipes[currentLine][idNextPipe].position != (pipes[currentLine][idNextPipe + 1].position - 1))
                        previousTile = '.';

                    ++idNextPipe;
                }

                //Adds the current tile(s) to the enclosed tiles if needed
                if((nbIntersections % 2) == 1)
                    nbEnclosedTiles += nbConsecutiveTiles;

                ++idCurrentTile; //Next tile
            }
        }
    }

    //Closes the file
    fclose(input);

    //Frees the memory
    for(unsigned int i = 0; i < nbLines; ++i){
        free(tilesPosition[i]);
        tilesPosition[i] = NULL;
    }
    free(tilesPosition);
    tilesPosition = NULL;
    for(unsigned int i = 0; i < nbLines; ++i){
        free(pipes[i]);
        pipes[i] = NULL;
    }
    free(pipes);
    pipes = NULL;

    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &endTime);
    long executionraceTime_nsec = endTime.tv_nsec - startTime.tv_nsec;
    double executionraceTime_mlsec = executionraceTime_nsec / 1000000.0;
    printf("\033[1;37mExecution time (without prints) : \033[1;31m%lf ms\n", executionraceTime_mlsec);

    //Prints the number
    printf("\033[1;37mHere is the number of tiles enclosed by the loop : \033[1;34m%d\033[0m\n", nbEnclosedTiles);
}

void bubbleSort(Pipe* array, size_t size){

    if(array == NULL)
        return;
    else{

        Pipe temp;

        for(unsigned int i = 0; i < (size - 1); ++i){
            for(unsigned int j = (size - 1); j > i; --j){
                if(array[j].position < array[j - 1].position){
                    temp = array[j - 1];
                    array[j - 1] = array[j];
                    array[j] = temp;
                }
            }
        }
    }
}