#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

typedef struct galaxy{
    unsigned int line;
    unsigned int row;
}Galaxy;

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
    unsigned int nbRowsFile;

    fscanf(input, "%*[^\n]");
    nbRowsFile = ftell(input);

    fseek(input, 0, SEEK_SET); //Sends the pointer back at the start of the file

    //Finds which rows and which lines contain no galaxies and stores the galaxies
    char currentChar;
    char emptyLine; //'T' = true ; 'F' = false
    char* emptyRows; //'T' = true ; 'F' = false
    unsigned int nbEmptyLines = 0;
    unsigned int nbEmptyRows = 0;
    unsigned int nbGalaxies = 0;
    unsigned int currentLine = 1;
    unsigned int* idEmptyLines;
    unsigned int* idEmptyRows;
    Galaxy* galaxies;

    emptyRows = (char*) malloc(sizeof(char) * nbRowsFile);
    for(unsigned int i = 0; i < nbRowsFile; ++i)
        emptyRows[i] = 'T';
    idEmptyLines = (unsigned int*) malloc(sizeof(unsigned int) * 1);
    idEmptyRows = (unsigned int*) malloc(sizeof(unsigned int) * 1);
    galaxies = (Galaxy*) malloc(sizeof(Galaxy) * 1);

    //Line loop
    while(1){

        emptyLine = 'T';

        //Row loop
        for(unsigned int currentRow = 1; currentRow <= nbRowsFile; ++currentRow){
            
            currentChar = fgetc(input);

            //If the current char is a '.', nothing changes

            //If the current char is a galaxy ('#')
            if(currentChar == '#'){
                emptyLine = 'F';
                emptyRows[currentRow] = 'F';
                //Stores the galaxy
                ++nbGalaxies;
                galaxies = (Galaxy*) realloc(galaxies, (sizeof(Galaxy) * nbGalaxies));
                galaxies[nbGalaxies - 1].line = currentLine;
                galaxies[nbGalaxies - 1].row = currentRow;
            }
        }

        //Stores the id of the line if it contains no galaxy
        if(emptyLine == 'T'){
            ++nbEmptyLines;
            idEmptyLines = (unsigned int*) realloc(idEmptyLines, (sizeof(unsigned int) * nbEmptyLines));
            idEmptyLines[nbEmptyLines - 1] = currentLine;
        }

        //If the end of the file has been reached, ends the loop
        if(fgetc(input) == EOF)
            break;

        ++currentLine; //Next line
    }

    //Stores the id of the rows that contain no galaxies
    for(unsigned int currentRow = 1; currentRow <= nbRowsFile; ++currentRow){
        if(emptyRows[currentRow] == 'T'){
            ++nbEmptyRows;
            idEmptyRows = (unsigned int*) realloc(idEmptyRows, (sizeof(unsigned int) * nbEmptyRows));    
            idEmptyRows[nbEmptyRows - 1] = currentRow;
        }
    }

    free(emptyRows);
    emptyRows = NULL;

    //Changes the coordinates of the galaxies according to the expansion (empty lines and rows)
    unsigned int lineExpansion, rowExpansion;
    unsigned int currentEmptyLine, currentEmptyRow;

    for(unsigned int currentGalaxy = 0; currentGalaxy < nbGalaxies; ++currentGalaxy){
        //Expansion of the lines
        lineExpansion = 0;
        currentEmptyLine = 0;
        for(unsigned int currentLine = 1; currentLine < galaxies[currentGalaxy].line; ++currentLine){
            //If the current line is empty
            if(idEmptyLines[currentEmptyLine] == currentLine){
                ++lineExpansion;
                ++currentEmptyLine;
            }
        }
        galaxies[currentGalaxy].line += lineExpansion;
        //Expansion of the rows
        rowExpansion = 0;
        currentEmptyRow = 0;
        for(unsigned int currentRow = 1; currentRow < galaxies[currentGalaxy].row; ++currentRow){
            //If the current row is empty
            if(idEmptyRows[currentEmptyRow] == currentRow){
                ++rowExpansion;
                ++currentEmptyRow;
            }
        }
        galaxies[currentGalaxy].row += rowExpansion;
    }

    free(idEmptyLines);
    free(idEmptyRows);
    idEmptyLines = NULL;
    idEmptyRows = NULL;

    //Finds the shortest path for each pair of galaxy and calculates the sum
    unsigned int sum = 0;

    for(unsigned int i = 0; i < (nbGalaxies - 1); ++i){
        for(unsigned int j = (i + 1); j < nbGalaxies; ++j)
            sum += ((galaxies[j].line - galaxies[i].line) + abs(galaxies[i].row - galaxies[j].row));
    }

    free(galaxies);
    galaxies = NULL;

    //Closes the file
    fclose(input);

    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &endTime);
    long executionraceTime_nsec = endTime.tv_nsec - startTime.tv_nsec;
    double executionraceTime_mlsec = executionraceTime_nsec / 1000000.0;
    printf("\033[1;37mExecution time (without prints) : \033[1;31m%lf ms\n", executionraceTime_mlsec);

    //Prints the number
    printf("\033[1;37mHere is the sum of the lengths of the shortest paths between every pair of galaxies : \033[1;34m%d\033[0m\n", sum);
}