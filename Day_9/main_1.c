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

    char currentChar;
    unsigned short allZeros;
    unsigned int nbNumbers, nbHist;
    long long result = 0;
    long long* currentHistory;
    long long* nextHistory;
    long long* lastNumsHistory;
    long long* nextNumsHistory;
    
    do{
        allZeros = 0;
        nbNumbers = 0;
        nbHist = 0;

        currentHistory = (long long*) malloc(sizeof(long long) * 1);
        lastNumsHistory = (long long*) malloc(sizeof(long long) * 1);

        //Reads the current line and stores its values
        do{
            ++nbNumbers;
            currentHistory = (long long*) realloc(currentHistory, (sizeof(long long) * nbNumbers));
            fscanf(input, "%lld", &currentHistory[nbNumbers - 1]);
            currentChar = fgetc(input);
        }while((currentChar != '\n') && (currentChar != EOF));

        while((nbNumbers >= 2) && (allZeros != 1)){

            allZeros = 1;

            ++nbHist; //One more history

            //Allocates the next history
            nextHistory = (long long*) malloc(sizeof(long long) * (nbNumbers - 1));

            //Fills the next history (substractions)
            for(unsigned short i = nbNumbers - 1; i != 0; --i){
                nextHistory[i - 1] = currentHistory[i] - currentHistory[i - 1];
                //Checks if the result was a zero
                if(nextHistory[i - 1] != 0)
                    allZeros = 0;
            }

            //Saves the last number of the current history
            lastNumsHistory = (long long*) realloc(lastNumsHistory, (sizeof(long long) * nbHist));
            lastNumsHistory[nbHist - 1] = currentHistory[nbNumbers - 1];

            //The next history becomes the next history
            free(currentHistory);
            currentHistory = nextHistory;
            nextHistory = NULL;

            --nbNumbers; //Decrements the number of numbers (lol)
        }

        //Allocates the array of the extrapolated values
        nextNumsHistory = (long long*) malloc(sizeof(long long) * nbHist);

        //Places the last number of the lastNumsHistory array into the last cell of the nextNumsHistory array
        nextNumsHistory[nbHist - 1] = lastNumsHistory[nbHist - 1];

        //Calculates the extrapolated values
        for(unsigned short i = nbHist - 1; i != 0; --i)
            nextNumsHistory[i - 1] = nextNumsHistory[i] + lastNumsHistory[i - 1];

        //Calculates the new result with the values obtained
        result += nextNumsHistory[0];

        //Frees the memory
        free(nextHistory);
        free(lastNumsHistory);
        free(nextNumsHistory);

    }while(currentChar != EOF);

    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &endTime);
    long executionraceTime_nsec = endTime.tv_nsec - startTime.tv_nsec;
    double executionraceTime_mlsec = executionraceTime_nsec / 1000000.0;
    printf("\033[1;37mExecution time (without prints) : \033[1;31m%lf ms\n", executionraceTime_mlsec);

    //Prints the number
    printf("\033[1;37mHere is the sum of all the extrapolated values : \033[1;34m%lld\033[0m\n", result);
}