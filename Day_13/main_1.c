#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

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
    unsigned int lineStart, currentLine;
    unsigned int nbLines, nbRows;
    unsigned int idLine, idLine_temp, idRow, idRow_temp;
    unsigned int nbReflections_lines, nbReflections_rows, nbReflections_temp;
    int pointerDown, pointerUp;
    unsigned long long* lines;
    unsigned long long* rows;
    unsigned long long finalSum = 0;

    //Treats the patterns one by one
    while(1){

        //Finds the number of rows of the current pattern
        nbRows = 0;
        lineStart = ftell(input);

        fscanf(input, "%*[^\n]");
        nbRows = ftell(input) - lineStart;

        fseek(input, -nbRows, SEEK_CUR); //Places the file pointer back at the start of the line

        //Stores the lines and the rows of the patterns
        currentLine = 0;
        lines = (unsigned long long*) malloc(sizeof(unsigned long long) * 1);
        rows = (unsigned long long*) malloc(sizeof(unsigned long long) * nbRows);
        for(unsigned int currentRow = 0; currentRow < nbRows; ++currentRow)
            rows[currentRow] = 0;
        
        while(1){

            currentChar = fgetc(input); //Reads a character
            if((currentChar == '\n') || (currentChar == EOF))
                break;

            lines = (unsigned long long*) realloc(lines, (sizeof(unsigned long long) * (currentLine + 1)));
            lines[currentLine] = 0;

            for(unsigned int currentRow = 0; currentRow < nbRows; ++currentRow){
                if(currentChar == '#'){
                    lines[currentLine] += (unsigned long long)pow(2, currentRow); //Lines
                    rows[currentRow] += (unsigned long long)pow(2, currentLine); //Rows
                }
                currentChar = fgetc(input); //Reads a character
            }

            ++currentLine; //One more line
        }

        nbLines = currentLine;

        //Finds the contiguous lines in the pattern that have identical values and counts the number of reflections
        nbReflections_lines = 0;
        idLine = 0;

        for(unsigned int currentLine = 0; currentLine < (nbLines - 1); ++currentLine){
            
            //If two contiguous lines with the same value are identified
            if(lines[currentLine] == lines[currentLine + 1]){

                nbReflections_temp = 0;

                pointerDown = currentLine;
                pointerUp = currentLine + 1;
                
                idLine_temp = pointerDown;
            
                while((pointerDown >= 0) && (pointerUp <= (nbLines - 1)) && (lines[pointerDown] == lines[pointerUp])){
                    ++nbReflections_temp; //One more reflection
                    --pointerDown;
                    ++pointerUp;
                }
                //If the number of reflections found is higher than the previous one AND the reflection is perfect
                if(((pointerDown < 0) || (pointerUp > (nbLines - 1))) && (nbReflections_temp > nbReflections_lines)){
                    nbReflections_lines = nbReflections_temp;
                    idLine = idLine_temp;
                }
            }
        }

        //Finds the contiguous rows in the pattern that have identical values and counts the number of reflections
        nbReflections_rows = 0;
        idRow = 0;

        for(unsigned int currentRow = 0; currentRow < (nbRows - 1); ++currentRow){

            //If two contiguous rows with the same value are identified
            if(rows[currentRow] == rows[currentRow + 1]){

                nbReflections_temp = 0;

                pointerDown = currentRow;
                pointerUp = currentRow + 1;

                idRow_temp = currentRow;
            
                while((pointerDown >= 0) && (pointerUp <= (nbRows - 1)) && (rows[pointerDown] == rows[pointerUp])){
                    ++nbReflections_temp; //One more reflection
                    --pointerDown;
                    ++pointerUp;
                }
            
                //If the number of reflections found is higher than the previous one AND the reflection is perfect
                if(((pointerDown < 0) || (pointerUp > (nbRows - 1))) && (nbReflections_temp > nbReflections_rows)){
                    nbReflections_rows = nbReflections_temp;
                    idRow = idRow_temp;
                }
            }
        }

        //Updates the final sum
        if(nbReflections_rows > nbReflections_lines)
            finalSum += idRow + 1;
        else
            finalSum += 100 * (idLine + 1);

        //Frees the memory
        free(lines);
        free(rows);
        lines = NULL;
        rows = NULL;

        //If it is the end of the file, ends the loop
        if(currentChar == EOF)
            break;
    }

    //Closes the file
    fclose(input);
    
    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &endTime);
    long executionraceTime_nsec = endTime.tv_nsec - startTime.tv_nsec;
    double executionraceTime_mlsec = executionraceTime_nsec / 1000000.0;
    printf("\033[1;37mExecution time (without prints) : \033[1;31m%lf ms\n", executionraceTime_mlsec);

    //Prints the number
    printf("\033[1;37mHere is the number obtained after the notes have been summarized : \033[1;34m%lld\033[0m\n", finalSum);
}