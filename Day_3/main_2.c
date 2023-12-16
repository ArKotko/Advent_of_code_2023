#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){

    //Checks the function arguments
    if(argc != 2){
        printf("Not enough arguments !");
        return 1;
    }

    //Opens the file
    FILE* input = fopen(argv[1], "rb");
    if(!input){
        printf("Failed to open file.\n");
        return 2;
    }

    unsigned int numCharLine = 0;
    unsigned long int fileSize = 0;

    //Finds the number of characters in one line
    //Ignores everything except '\n'
    fscanf(input, "%*[^\n]");
    numCharLine = ftell(input) + 1;
    //Gets the size of the file
    fseek(input, 0, SEEK_END);
    fileSize = ftell(input);
    //Goes back to the beginning of the file
    fseek(input, 0, SEEK_SET);

    unsigned long long sum = 0; //The sum we are trying to find
    unsigned int gearPosition = 0, targetLimit = 0, digitCount = 0, sameNum = 0, lineNumber = 0;
    unsigned int digitPos[2] = {0,0}; //Position of two of the digits surrounding the gear
    int targetPosition = 0;
    char currentChar;
    
    while(1){
        //Tries to find a "gear" in the file
        //Ignores everything except '*'
        fscanf(input, "%*[^*]");
        //Gets the position of the "gear" (only if we are not at the end of the file)
        if(fgetc(input) != EOF)
            gearPosition = ftell(input) - 1;
        else
            break;
        //Tries to find if there are two AND ONLY two numbers around the "gear"
        digitCount = 0;
        digitPos[0] = 0;
        digitPos[1] = 0;
        //CHARACTER BEFORE THE "GEAR" (only if it is possible)
        if((gearPosition - 1) >= 0){
            //Places the file pointer before the "gear"
            fseek(input, gearPosition - 1, SEEK_SET);
            //Checks if it is a number
            currentChar = fgetc(input);
            if(currentChar >= '0' && currentChar <= '9'){
                digitPos[digitCount] = gearPosition - 1;
                ++digitCount;
            }
        }
        //CHARACTER AFTER THE "GEAR" (only if it is possible)
        if((gearPosition + 1) < fileSize){
            //Places the file pointer after the "gear"
            fseek(input, gearPosition + 1, SEEK_SET);
            //Checks if it is a number
            currentChar = fgetc(input);
            if(currentChar >= '0' && currentChar <= '9'){
                digitPos[digitCount] = gearPosition + 1;
                ++digitCount;
            }
        }
        //LINES ABOVE AND UNDER THE "GEAR" (only if the two numbers have not already been found)
        //lineNumber = 1 --> line above ; lineNumber = 3 --> line below
        if(digitCount != 2){
            lineNumber = 1;
            while(lineNumber){
                //Calculates the target position and sets the value of the target limit
                if(lineNumber == 1){
                    targetPosition = gearPosition - numCharLine - 1;
                    if(targetPosition < 0)
                        targetLimit = 1;
                    else
                        targetLimit = 0;
                }
                else if(lineNumber == 3){
                    targetPosition = gearPosition + numCharLine - 1;
                    if(targetPosition >= fileSize)
                        targetLimit = 1;
                    else
                        targetLimit = 0;
                }      
                //Check the line ONLY if less than two digits have already been found AND if the target limit is not reached
                if(digitCount != 2 && !targetLimit){
                    for(unsigned int offset = 0; offset < 3; ++offset){
                        //Places the file pointer at that position
                        fseek(input, targetPosition + offset, SEEK_SET);
                        //Get the character at that position
                        currentChar = fgetc(input);
                        //If it is a number
                        if(currentChar >= '0' && currentChar <= '9'){
                            //If it is the first digit we found
                            if(!sameNum){
                                digitPos[digitCount] = targetPosition + offset;
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
                sameNum = 0;
            }
        }

        //printf("Gear : %d --> 1st num : %d, 2nd num : %d -- %d\n", gearPosition, digitPos[0], digitPos[1], digitCount);

        //If two DIGITS belonging to two different numbers have been found, finds the NUMBERS these digits belong to
        if(digitCount == 2){
            //To store the digits found (in ASCII)
            char asciiNum[2][10];
            unsigned long number1 = 0, number2 = 0; //Integer version of the final numbers

            while(digitCount != 0){
                //Sets the file pointer at the target position
                fseek(input, digitPos[digitCount - 1], SEEK_SET);
                while(1){
                    //Takes the digit
                    currentChar = fgetc(input);
                    //If the character is not a number
                    if(currentChar < '0' || currentChar > '9')
                        break;
                    //If the new position we will try to get to is out of bounds
                    if((ftell(input) - 2) < 0){
                        //Goes back to the digit (file pointer moved with fgetc())
                        fseek(input, -1, SEEK_CUR);
                        break;
                    }
                    //Goes to the previous digit
                    fseek(input, -2, SEEK_CUR);
                }
                //Takes the whole number
                fscanf(input, "%[0-9]", asciiNum[digitCount - 1]);
                --digitCount;
            }
            //Turns the ascii strings to integers
            number1 = atoi(asciiNum[0]);
            number2 = atoi(asciiNum[1]);
            //Updates the sum
            sum = sum + (number1 * number2);
            //printf("Number 1 : %ld; Number 2 : %ld; Sum : %Ld\n", number1, number2, sum);
        }
        //Goes back to the position after the "gear"
        fseek(input, gearPosition + 1, SEEK_SET);
    }
    
    //Prints the sum
    printf("Here is the sum of the gear ratios of the engine : %lld\n", sum);

    //Close the file
    fclose(input);

    return 0;
}