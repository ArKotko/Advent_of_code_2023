#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct hand{
    char cards[6];
    unsigned int bid;
    unsigned short type;
    struct hand* next;
}Hand;

typedef Hand* HandList;

Hand* createHand(char* cards, unsigned int bid, unsigned short type);
HandList addHand(HandList ourList, Hand* hand);
unsigned short determineType(char* cards);
short compareLabels(char firstLabel, char secondLabel);

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

    HandList ourList = NULL;

    char currentHand[6];
    unsigned int currentBid;
    unsigned short currentType;

    do{
        //Ignores useless characters (just in case)
        fscanf(input, "%*[ \t\r\n]");

        //Reads and saves the current hand
        fgets(currentHand, 6, input);

        //Reads and saves the current bid
        fscanf(input, "%d", &currentBid);

        //Determines the type of the card
        currentType = determineType(currentHand);

        //Creates a new hand and adds it to the list
        ourList = addHand(ourList, createHand(currentHand, currentBid, currentType));

    }while(fgetc(input) != EOF);

    //Calculates the total winnings
    unsigned int counter = 1;
    unsigned long long totalWinnings = 0;

    while(ourList != NULL){
        totalWinnings += ((ourList->bid) * counter);
        ++counter;
        ourList = ourList->next;
    }

    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &endTime);
    long executionraceTime_nsec = endTime.tv_nsec - startTime.tv_nsec;
    double executionraceTime_mlsec = executionraceTime_nsec / 1000000.0;
    printf("\033[1;37mExecution time (without prints) : \033[1;31m%lf ms\n", executionraceTime_mlsec);

    //Prints the number
    printf("\033[1;37mHere are the total winnings : \033[1;34m%lld\033[0m\n", totalWinnings);
}

//Functions

Hand* createHand(char* cards, unsigned int bid, unsigned short type){

    Hand* new = (Hand*) malloc(sizeof(Hand));
    if(new != NULL){
        strcpy(new->cards, cards);
        new->bid = bid;
        new->type = type;
        new->next = NULL;
    }

    return new;
}

HandList addHand(HandList ourList, Hand* hand){

    if(ourList == NULL)
        return hand;

    //Saves the start of the list
    HandList root = ourList;

    Hand* previous = NULL;

    //Goes to the first hand in the list with a type greater or equal than the type of the current hand
    while((ourList != NULL) && (hand->type > ourList->type)){
        previous = ourList;
        ourList = ourList->next;
    }

    //Second ordering rule if needed
    while((ourList != NULL) && (hand->type == ourList->type)){

        unsigned short idLabel = 0;
        short result;

        //Compares the labels of the two hands
        while(idLabel < 5){
            
            result = compareLabels(hand->cards[idLabel], ourList->cards[idLabel]);

            //First higher than second
            if(result > 0){
                previous = ourList;
                ourList = ourList->next;
                break;
            }
            //First lower than second
            if(result < 0)
                break;
            //Two equals
            else
                ++idLabel;
        }

        if((result == 0) || (result < 0))
            break;
    }

    //Places the current hand in the list at the right position
    if(previous == NULL)
        root = hand;
    else
        previous->next = hand;
    if(ourList != NULL)
        hand->next = ourList;

    return root;
}

typedef struct label{
    char label;
    unsigned short appearances;
}Label;

unsigned short determineType(char* cards){

    Label tabLabels[5];
    short nbLabels = 0;
    unsigned short alreadyInHand = 0;
    unsigned short type;

    //Stores the cards of the hand
    unsigned short idCard = 0;

    while(cards[idCard] != '\0'){

        //Checks if the label of the current card is already in the hand
        for(unsigned short idLabel = 0; idLabel <= nbLabels; ++idLabel){
            if(cards[idCard] == tabLabels[idLabel].label){
                ++tabLabels[idLabel].appearances;
                alreadyInHand = 1;
                break;
            }
        }

        //Adds the label to the list if it is not already in the hand
        if(!alreadyInHand){
            ++nbLabels;
            tabLabels[nbLabels - 1].label = cards[idCard];
            tabLabels[nbLabels - 1].appearances = 1;
        }

        alreadyInHand = 0;
        ++idCard;
    }

    //Determines the type
    switch (nbLabels){
    case 1 :
        type = 6;
        break;
    case 2:
        if(tabLabels[0].appearances == 4 || tabLabels[0].appearances == 1)
            type = 5;
        else
            type = 4;
        break;
    case 3 :
        unsigned short i = 0;
        while(tabLabels[i].appearances == 1)
            ++i;
        if(tabLabels[i].appearances == 3)
            type = 3;
        else
            type = 2;
        break;       
    case 4 :
        type = 1;
        break;
    case 5 :
        type = 0;
        break;
    }

    return type;
}

short compareLabels(char firstLabel, char secondLabel){

    //First higher than second = positive ; first lower than second = negative ; two equals = zero

    short result;

    //If both labels are letters
    if(((firstLabel >= 'A') && (firstLabel <= 'Z')) && ((secondLabel >= 'A') && (secondLabel <= 'Z'))){

        char letters[] = {'T', 'J', 'Q', 'K', 'A'};
        short valueFirstLabel = 0, valueSecondLabel = 0;

        //Searches the value of the first label
        while(firstLabel != letters[valueFirstLabel])
            ++valueFirstLabel;
        //Searches the value of the second label
        while(secondLabel != letters[valueSecondLabel])
            ++valueSecondLabel;
        
        result = valueFirstLabel - valueSecondLabel;
    }
    else{
        result = firstLabel - secondLabel;
    }

    return result;
}