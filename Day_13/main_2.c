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
    unsigned short smudgeRepaired;
    unsigned int lineStart, currentLine;
    unsigned int nbLines, nbRows;
    unsigned int idLine, idLine_temp, idRow, idRow_temp;
    unsigned int nbReflections_lines, nbReflections_rows, nbReflections_temp;
    int pointerDown, pointerUp;
    unsigned long long* lines;
    unsigned long long* rows;
    unsigned long long diff;
    unsigned long long finalSum = 0;
    double bitToSwitch;

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

            nbReflections_temp = 0;
            smudgeRepaired = 0;

            pointerDown = currentLine;
            pointerUp = currentLine + 1;
                
            idLine_temp = pointerDown;
        
            while((pointerDown >= 0) && (pointerUp <= (nbLines - 1))){

                //If the values of the contiguous lines are different
                if(lines[pointerDown] != lines[pointerUp]){

                    //If the smudge has already been repaired
                    if(smudgeRepaired != 0)
                        break;

                    //Calculates the difference between the two values
                    diff = _abs64(lines[pointerDown] - lines[pointerUp]);

                    //Sees if the values can become equal by switching only one bit in one of them
                    bitToSwitch = log2((double)diff);

                    if(((bitToSwitch - (long)bitToSwitch) == (double)0) && ((long)bitToSwitch >= 0) && ((long)bitToSwitch <= (nbRows - 1)))
                        smudgeRepaired = 1;
                    else
                        break;
                }

                ++nbReflections_temp; //One more reflection
                --pointerDown;
                ++pointerUp;
            }

            //If the number of reflections found is higher than the previous one AND the reflection is perfect AND a smudge has been repaired
            if((smudgeRepaired != 0) && ((pointerDown < 0) || (pointerUp > (nbLines - 1))) && (nbReflections_temp > nbReflections_lines)){
                nbReflections_lines = nbReflections_temp;
                idLine = idLine_temp;
            }
        }

        //Tries to find a new reflection line starting from each pair of contiguous rows
        nbReflections_rows = 0;
        idRow = 0;

        for(unsigned int currentRow = 0; currentRow < (nbRows - 1); ++currentRow){

            nbReflections_temp = 0;
            smudgeRepaired = 0;

            pointerDown = currentRow;
            pointerUp = currentRow + 1;

            idRow_temp = currentRow;
            
            while((pointerDown >= 0) && (pointerUp <= (nbRows - 1))){

                //If the values of the contiguous rows are different
                if(rows[pointerDown] != rows[pointerUp]){

                    //If the smudge has already been repaired
                    if(smudgeRepaired != 0)
                        break;

                    //Calculates the difference between the two values
                    diff = _abs64(rows[pointerDown] - rows[pointerUp]);

                    //Sees if the values can become equal by switching only one bit in one of them
                    bitToSwitch = log2((double)diff);

                    if(((bitToSwitch - (long)bitToSwitch) == (double)0) && ((long)bitToSwitch >= 0) && ((long)bitToSwitch <= (nbLines - 1)))
                        smudgeRepaired = 1;
                    else
                        break;
                }

                ++nbReflections_temp; //One more reflection
                --pointerDown;
                ++pointerUp;
            }
            
            //If the number of reflections found is higher than the previous one AND the reflection is perfect AND a smudge has been repaired
            if((smudgeRepaired != 0) && ((pointerDown < 0) || (pointerUp > (nbRows - 1))) && (nbReflections_temp > nbReflections_rows)){
                nbReflections_rows = nbReflections_temp;
                idRow = idRow_temp;
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