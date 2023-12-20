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
    //Linked list
    List ourList = NULL;

    unsigned long long totalSum = 0;
    unsigned int cardSum;
    int currentNumber;
    char currentChar;

    while(1){
        //Sees if we are at the end of the file
        if(fgetc(input) == EOF)
            break;

        //Ignores the word "Card" and the number of the card
        fscanf(input, "%*[^:]");
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
            //Adds the number to the link
            ourList = addBlock(ourList, currentNumber);
        }

        //Reads and stores all our number
        while(1){
            //Ignores everything that is not a number or "\n"
            fscanf(input, "%*[^0-9\n]");
            //Takes the next char
            currentChar = fgetc(input);
            //If it is '\n', it is the end of the loop
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

        cardSum = 0;

        //Compares our numbers and the winning numbers
        while(ourList){
            //If the winning number is in our tree of numbers
            if(isKeyInTree(ourTree, ourList->number)){
                if(cardSum == 0)
                    cardSum = 1;
                else
                    cardSum = cardSum * 2;
            }
            //Goes to the next element in the list
            ourList = ourList->next_block;
        }

        totalSum += cardSum;

        //Free the memory
        freeList(ourList);
        freeTree(ourTree);
        //Reset the BST and the linked list;
        ourTree = NULL;
        ourList = NULL;
    }

    //Prints the sum
    printf("Here is the sum of the cards :\033[1;34m %lld \033[0m\n", totalSum);

    return 0;
}