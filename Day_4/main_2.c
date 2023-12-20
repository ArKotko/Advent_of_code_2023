#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Linked list for winning numbers
typedef struct block{
    unsigned int number;
    struct block* next_block;
}Block;

typedef Block* List;

Block* createBlock(unsigned int n){
    Block* newBlock = (Block*) malloc(sizeof(Block));
    if(!newBlock){
        printf("Problem encountered while creating the block\n");
        return NULL;
    }
    newBlock->number = n;
    newBlock->next_block = NULL;
    return newBlock;
}

List addBlock(List yourList, unsigned int n){

    if(!yourList)
        return createBlock(n);

    //Stores the first block of the list
    List savedList = yourList;
    Block* previousBlock;

    //Goes to the end of the list
    while(yourList){
        previousBlock = yourList;
        yourList = yourList->next_block;
    }

    //Adds the block to the end of the list
    previousBlock->next_block = createBlock(n);

    return savedList;
}

void freeList(List yourList){
    
    Block* nextBlock;

    while(yourList){
        nextBlock = yourList->next_block;
        free(yourList);
        yourList = nextBlock;
    }
}

//BST for our numbers
typedef struct node{
    unsigned int key;
    struct node* left_child;
    struct node* right_child;
}Node;

typedef Node* Tree;

Node* createNode(unsigned int k){
    Node* newNode = (Node*) malloc(sizeof(Node));
    if(!newNode){
        printf("Problem encountered while creating the node\n");
        return NULL;
    }
    newNode->key = k;
    newNode->left_child = NULL;
    newNode->right_child = NULL;
    return newNode;
}

Tree addNode(Tree yourTree, unsigned int k){

    if(!yourTree)
        return createNode(k);

    //Stores the root of the tree
    Tree root = yourTree;

    while(yourTree){
        if(yourTree->key > k){
            if(!yourTree->left_child){
                yourTree->left_child = createNode(k);
                break;
            }
            else
                yourTree = yourTree->left_child;
        }
        else if(yourTree->key < k){
            if(!yourTree->right_child){
                yourTree->right_child = createNode(k);
                break;
            }
            else
                yourTree = yourTree->right_child;
        }
    }

    return root;
}

unsigned int isKeyInTree(Tree yourTree, unsigned int k){

    unsigned int found = 0;

    while(yourTree){
        if(yourTree->key == k){
            found = 1;
            break;
        }
        else if(yourTree->key < k)
            yourTree = yourTree->right_child;
        else if(yourTree->key > k)
            yourTree = yourTree->left_child;
    }

    return found;
}

void freeTree(Tree yourTree){

    if(!yourTree)
        return;

    freeTree(yourTree->left_child);
    freeTree(yourTree->right_child);
    free(yourTree);
}

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

    //BST
    Tree ourTree = NULL;
    //Linked lists
    List ourList = NULL;

    unsigned long long totalSum = 0;
    unsigned int currentCardScore, currentCardId = -1, numberOfCards = 0;
    unsigned int* cardArray;
    int currentNumber;
    char currentChar;

    //Goes through all the file and count the number of cards
    while(1){
        //Ignores everything except '\n'
        fscanf(input, "%*[^\n]");
        //Adds one the number of cards
        ++numberOfCards;
        //If we are at the end of the file, stops the loop
        if(fgetc(input) == EOF)
            break;
    }

    //Creates the array of card
    cardArray = (unsigned int*) malloc(sizeof(unsigned int) * numberOfCards);

    //Fills the array with ones
    for(unsigned int i = 0; i < numberOfCards; ++i)
        cardArray[i] = 1;

    //Goes back to the start of the file
    fseek(input, 0, SEEK_SET);

    while(1){
        //Sees if we are at the end of the file
        if(fgetc(input) == EOF)
            break;

        //Ignores the word "Card" and the number of the card
        fscanf(input, "%*[^:]");
        //Adds one to the current card id
        ++currentCardId;
        //Ignores the ":" and then the blankspace
        fgetc(input);
        fgetc(input);

        //Reads and stores all the winning numbers
        while(1){
            //Ignores everything that is not a number or "|"
            fscanf(input, "%*[^0-9|]");
            //Takes the next char
            currentChar = fgetc(input);
            //If it is '|', it is the end of the loop
            if(currentChar == '|')
                break;
            //If it is a number, goes to back to the character before
            else if(currentChar >= '0' && currentChar <= '9')
                fseek(input, -1, SEEK_CUR);
            //Reads the number and stores it into the linked list
            fscanf(input, "%d", &currentNumber);
            //Adds the number to the list
            ourList = addBlock(ourList, currentNumber);
        }

        //Reads and stores all our number
        while(1){
            //Ignores everything that is not a number or '\n'
            fscanf(input, "%*[^0-9\n]");
            //Takes the next char
            currentChar = fgetc(input);
            //If it is '\n' or EOF, it is the end of the loop
            if(currentChar == '\n' || currentChar == EOF)
                break;
            //If it is a number, goes to back to the character before
            else if(currentChar >= '0' && currentChar <= '9')
                fseek(input, -1, SEEK_CUR);
            //Reads the number and stores it into the linked list
            fscanf(input, "%d", &currentNumber);
            //Adds the number to the link
            ourTree = addNode(ourTree, currentNumber);
        }

        currentCardScore = 0;

        //Compares our numbers and the winning numbers
        while(ourList){
            //If the winning number is in our tree of numbers
            if(isKeyInTree(ourTree, ourList->number))
                //Adds one to the score of the current card
                ++currentCardScore;
            //Goes to the next element in the list
            ourList = ourList->next_block;
        }

        //Adds all the card we won to the array of cards
        for(unsigned int i = 1; i <= currentCardScore; ++i){
            if((currentCardId + i) > numberOfCards)
                break;
            cardArray[currentCardId + i] += cardArray[currentCardId];
        }
        
        //Free the memory
        freeList(ourList);
        freeTree(ourTree);
        //Reset the BST and the linked list;
        ourTree = NULL;
        ourList = NULL;
    }

    //Calculates the total sum
    for(unsigned int i = 0; i < numberOfCards; ++i)
        totalSum += cardArray[i];

    //Prints the sum
    printf("Here is the sum of the cards :\033[1;34m %lld \033[0m\n", totalSum);

    return 0;
}