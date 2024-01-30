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
    FILE* input = fopen(argv[1], "r");
    if(!input){
        printf("Failed to open file.\n");
        return 2;
    }

    //Gets the number of seeds and stores them
    unsigned int nbSeeds = 0;
    unsigned long long* destNumbers = NULL;
    char currentChar;
    //Ignores the word "seed" and the ':'
    fscanf(input, "%*[^ ]");
    while(1){
        //Gets the next char
        currentChar = fgetc(input);
        //If it is the end of the line, the loop stops
        if(currentChar == '\n')
            break;
        //Adds one to the numbers of seeds
        ++nbSeeds;
        //Extends the size of the array containing the seeds
        destNumbers = (unsigned long long*) realloc(destNumbers, sizeof(unsigned long long) * nbSeeds);
        //Stores the number of the seed
        fscanf(input, "%lld", &destNumbers[nbSeeds - 1]);
    }

    //Creates a temp array of destination numbers
    long long* tempArray = (long long*) malloc(sizeof(long long) * nbSeeds);

    //Stores all the destination number
    unsigned long long sourceNum = 0, destNum = 0, range = 0;

    //Ignores everything except numbers
    fscanf(input, "%*[^0-9]");

    while(!feof(input)){
        //Fills the temp array with -1
        for(unsigned int i = 0; i < nbSeeds; ++i)
            tempArray[i] = -1;

        while(1){
            //Stores the source number, the destination number and the range
            fscanf(input, "%lld %lld %lld", &destNum, &sourceNum, &range);
            //Checks if the previous destination numbers are in the range
            for(unsigned int i = 0; i < nbSeeds; ++i){
                if(destNumbers[i] >= sourceNum && destNumbers[i] <= (sourceNum + range - 1))
                    //Stores the matching destination number in the tempArray
                    tempArray[i] = destNum + abs(sourceNum - destNumbers[i]);
            }
            //Ignores the next character
            fgetc(input);
            //Stores the next character
            currentChar = fgetc(input);
            //If it is the end of the map or of the file, the loop stops
            if(currentChar == '\n' || currentChar == EOF)
                break;
            //If it is a number, it goes back to the previous character
            else
                fseek(input, -1, SEEK_CUR);
        }

        //Fills the destination numbers array
        for(unsigned int i = 0; i < nbSeeds; ++i){
            //If tempArray[i] = -1, destNumbers[i] keeps its previous value
            if(tempArray[i] != -1)
                destNumbers[i] = tempArray[i];
        }

        //Ignores everything except numbers
        fscanf(input, "%*[^0-9]");
    }

    //Finds the smallest number in the destination numbers array
    unsigned long long lowestDestination = destNumbers[0];
    for(unsigned int i = 1; i < nbSeeds; ++i){
        if(destNumbers[i] < lowestDestination)
            lowestDestination = destNumbers[i];
    }

    //Prints the number
    printf("Here is the lowest location number : \033[1;34m%lld\033[0m\n", lowestDestination);

    return 0;
}