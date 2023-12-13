#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//This is probably a really not efficent method (in terms of memory and time), but it works perfectly fine and i'm happy with it :) 

typedef struct number{
    unsigned int number;
    unsigned int line;
    unsigned int firstRow, lastRow;
    struct number* next;
}number;

typedef number* numberList;

number* createNumber(unsigned int n, unsigned int l, unsigned int fr, unsigned int lr){
    number* new = (number*) malloc(sizeof(number));
    if(!new){
        printf("Creation of the number failed.\n");
        return NULL;
    }
    new->number = n;
    new->line = l;
    new->firstRow = fr;
    new->lastRow = lr;
    new->next = NULL;
    return new;
}

numberList addNumber(numberList list, number* num){
    if(!list)
        return num;
    else{
        //Store the start of the list
        numberList startList = list;
        while(1){
            if(list->next)
                list = list->next;
            else
                break;
        }
        list->next = num;
        return startList;
    }
}   

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

    unsigned int numLines = 1, numRows = 1;

    //Find the number of lines in the file
    while(1){
        //Ignore everything except '\n'
        fscanf(input, "%*[^\n]");
        if(fgetc(input) == EOF)
            break;
        ++numLines;
    }
    {
        //Get the size of the file
        unsigned long int fileSize = ftell(input);
        //Find the number of rows in the file
        //We need to subract the number of '\n' and '\r' to the size of the file before dividing it by the number of lines
        numRows = (fileSize - ((numLines - 1) * 2)) / numLines;
    }
    //Go back to the beginning of the file
    fseek(input, 0, SEEK_SET);

    //Create the matrix that's going to contain a "map" of the engine
    char** matrix = (char**) malloc(sizeof(char*) * numLines);
    if(!matrix){
        printf("Creation of the matrix failed.\n");
        return 1;
    }
    for(size_t i = 0; i < numLines; ++i){
        matrix[i] = (char*) malloc(sizeof(char) * numRows);
        if(!matrix[i]){
            printf("Creation of one of the lines of the matrix failed.\n");
            return 2;
        }   
    }

    //Go trough all the file, fill the "map" of the engine and save the numbers
    char currentChar, previousChar = '.';
    char currentNum[6];
    int currentLine = 0, currentRow = 0, numNum = 0;
    numberList list = NULL;

    do{
        currentChar = fgetc(input);
        //printf("%d.%d : %c\n", currentLine, currentRow, currentChar);
        if(currentChar == '.')
            matrix[currentLine][currentRow] = '0'; 
        else if(currentChar >= '0' && currentChar <= '9'){
            matrix[currentLine][currentRow] = '2';
            //Save the number
            currentNum[numNum] = currentChar;
            ++numNum;
        }
        else if(currentChar == '\r')
            //Ignore the '\n'
            fgetc(input);
        else
            matrix[currentLine][currentRow] = '1';
        
        //Create a new number if needed
        if((previousChar >= '0' && previousChar <= '9') && (currentChar < '0' || currentChar > '9')){
            currentNum[numNum] = '\0';
            unsigned int tempNum = atoi(currentNum);
            number* tempStructNum = createNumber(tempNum, currentLine, (currentRow - numNum), (currentRow - 1));
            list = addNumber(list, tempStructNum);
            numNum = 0;
        }
        //Go the the next row or line
        if(currentChar != '\r')
            ++currentRow;
        else{
            currentRow = 0;
            ++currentLine;
        }
        //Save the current character
        previousChar = currentChar;
    }while(currentChar != EOF);

    //Check for each number if it is surrounded by at least one character
    unsigned long long int sum = 0;
    unsigned int startI, endI, isCharAround = 0;
    numberList tempList = list;
    while(tempList){
        //Check the line above the number (only if it is not out of bound)
        currentLine = tempList->line - 1;
        if(currentLine >= 0){
            currentRow = tempList->firstRow - 1;
            if(currentRow < 0)
                startI = tempList->firstRow;
            else
                startI = currentRow;
            currentRow = tempList->lastRow + 1;
            if(currentRow > (numRows - 1))
                endI = tempList->lastRow;
            else
                endI = currentRow;
            for(unsigned int i = startI; i <= endI; ++i){
                if(matrix[currentLine][i] == '1')
                    isCharAround = 1;
            }
        }
        //Check the number before (only if it is not out of bound)
        currentRow = tempList->firstRow - 1;
        if(currentRow >= 0){
            if(matrix[tempList->line][currentRow] == '1')
                isCharAround = 1;
        }
        //Check the number after (only if it is not out of bound)
        currentRow = tempList->lastRow + 1;
        if(currentRow <= (numRows - 1)){
            if(matrix[tempList->line][currentRow] == '1')
                isCharAround = 1;
        }
        //Check the line under the number (only if it is not out of bound)
        currentLine = tempList->line + 1;
        if(currentLine <= (numLines - 1)){
            currentRow = tempList->firstRow - 1;
            if(currentRow < 0)
                startI = tempList->firstRow;
            else
                startI = currentRow;
            currentRow = tempList->lastRow + 1;
            if(currentRow > (numRows - 1))
                endI = tempList->lastRow;
            else
                endI = currentRow;
            for(unsigned int i = startI; i <= endI; ++i){
                if(matrix[currentLine][i] == '1')
                    isCharAround = 1;
            }
        }
        if(isCharAround)
            sum += tempList->number;
        //printf("%d --> %d\n", tempList->number, isCharAround);
        isCharAround = 0;
        tempList = tempList->next;
    }

    //Prints to check if everything is alright
    /*for(int i = 0; i < numLines; ++i){
        for(int j = 0; j < numRows; ++j)
            printf("%c", matrix[i][j]);
        printf("\n");
    }*/

    /*numberList tempListPrint = list;
    while(tempListPrint){
        printf("%d %d.%d -> %d.%d\n", tempListPrint->number, tempListPrint->line, tempListPrint->firstRow, tempListPrint->line, tempListPrint->lastRow);
        tempListPrint = tempListPrint->next;
    }*/

    printf("Here is the sum of the engine : %lld\n", sum);

    //Free the memory
    //Matrix
    for(size_t i = 0; i < numLines; ++i){
        free(matrix[i]);
    }
    free(matrix);
    //Number list
    number* next;
    do{
        next = list->next;
        free(list);
        list = next;
    }while(list);
    //Close the file
    fclose(input);

    return 0;
}