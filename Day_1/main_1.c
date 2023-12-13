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

    unsigned int firstNum = 0, secondNum = -1, number = 0;
    unsigned long int sum = 0;
    size_t it = 1;
    char currentChar;

    while(1){
        //Get the current argument
        currentChar = fgetc(input);
        if(currentChar >= '0' && currentChar <= '9'){
            if(it == 1)
                firstNum = atoi(&currentChar);
            else if(it > 1)
                secondNum = atoi(&currentChar);
            ++it;
        }
        else if(currentChar == '\n' || currentChar == EOF){
            if(secondNum == -1)
                secondNum = firstNum;
            number = firstNum * 10 + secondNum;
            sum += number;
            if(currentChar == EOF)
                break;
            secondNum = -1;
            number = 0;
            it = 1;
        }
    }

    printf("Here is your sum big boy : %ld", sum);

    fclose(input);

    return 0;
}