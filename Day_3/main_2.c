#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){

    //Check the function arguments
    if(argc != 2){
        printf("Not enough arguments !");
        return 1;
    }

    //Open the file
    FILE* input = fopen(argv[1], "rb");
    if(!input){
        printf("Failed to open file.\n");
        return 2;
    }

    unsigned int numCharLine = 0;
    unsigned long int fileSize = 0;

    //Find the number of characters in one line
    //Ignore everything except '\n'
    fscanf(input, "%*[^\n]");
    numCharLine = ftell(input) + 1;
    //Get the size of the file
    fseek(input, 0, SEEK_END);
    fileSize = ftell(input);
    //Go back to the beginning of the file
    fseek(input, 0, SEEK_SET);

    unsigned int gearPosition = 0, targetPosition = 0, targetLimit = 0, digitCount = 0, sameNum = 0, lineNumber = 0;
    unsigned int digitPos[2] = {0,0}; //Position of the numbers surrounding the gear
    char currentChar;
    
    while(1){
        //Try to find a "gear" in the file
        //Ignore everything except '*'
        fscanf(input, "%*[^*]");
        //Get the position of the "gear" (only if we are not at the end of the file)
        if(fgetc(input) != EOF)
            gearPosition = ftell(input);
        else
            break;
        //Try to find if there are two AND ONLY two numbers around the gear
        digitCount = 0;
        
        digitPos[0] = 0;
        digitPos[1] = 0;
        //CHARACTER BEFORE THE "GEAR"
        //Place the file pointer before the "gear"
        fseek(input, -2, SEEK_CUR);
        //Check if it is a number
        if(fgetc(input) >= '0' && fgetc(input) <= '9'){
            digitPos[digitCount] = gearPosition - 1;
            ++digitCount;
        }
        //CHARACTER AFTER THE "GEAR"
        //Check if it is a number
        if(fgetc(input) >= '0' && fgetc(input) <= '9'){
            digitPos[digitCount] = gearPosition + 1;
            ++digitCount;
        }
        //LINE ABOVE AND UNDER THE "GEAR" (only if the two numbers have not already been found)
        if(digitCount != 2){
            sameNum = 0;
            lineNumber = 1;
            while(lineNumber){
                //Calculate the target position and set the value of the target limit
                if(lineNumber == 1){
                    targetPosition = gearPosition - numCharLine - 2;
                    if(targetPosition < 0)
                        targetLimit = 1;
                }
                else if(lineNumber == 3){
                    targetPosition = gearPosition + numCharLine - 2;
                    if(targetPosition >= fileSize)
                        targetLimit = 1;
                }         
                //Check the line ONLY if less than two digits have already be found AND if the target limit is not reached
                if(digitCount != 2 && !targetLimit){
                    for(unsigned int offset = 0; offset < 3; ++offset){
                        //Place the file pointer at that position
                        fseek(input, targetPosition + offset, SEEK_SET);
                        //Get the character at that position
                        currentChar = fgetc(input);
                        //If it is a number
                        if(currentChar >= '0' && currentChar <= '9'){
                            //If it is the first digit we found
                            if(!sameNum){
                                digitPos[digitCount] = targetPosition + 1;
                                ++digitCount;
                                sameNum = 1;
                            }
                        }   
                        //If it is not a number
                        else{
                            //If we had already found a number
                            if(sameNum)
                                sameNum = 0;
                        }
                        //If we have found two digits belonging to two different numbers
                        if(digitCount == 2)
                            break;  
                    }
                }
                if(lineNumber == 1)
                    lineNumber = 3;
                else if(lineNumber == 3)
                    lineNumber = 0;
            }
        }

        
        //If two DIGITS belonging to two different numbers have been found, try to find the NUMBERS of which these numbers belong
        if(digitCount == 2){
            //TO DO
        }

        unsigned long long int sum = 0;
    }
    
    //printf("Here is the sum of the engine : %lld\n", sum);

    //Close the file
    fclose(input);

    return 0;
}