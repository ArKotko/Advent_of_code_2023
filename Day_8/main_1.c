#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct node{
    char node[4];
    char leftNode[4];
    char rightNode[4];
    struct node* next;
}Node;

typedef Node* NodeList;

static NodeList alphabet[26];

Node* createNode(char* node, char* leftNode, char* rightNode);
NodeList addNode(NodeList list, Node* node);
void freeList(NodeList list);

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

    //Calculates the length of the left/right sequence of instructions
    size_t sequenceLength = 0;
    char* sequence;

    fscanf(input, "%*[ \t\r\n]"); //Ignores useless characters (just in case)
    fscanf(input, "%*[^ \t\r\n]"); //Goes to the end of the sequence
    sequenceLength = ftell(input);
    fseek(input, 0, SEEK_SET); //Sets the cursor back to the start of the file
    sequence = (char*) malloc(sizeof(char) * (sequenceLength + 1)); //Allocates the right space in memory
    fscanf(input, "%*[ \t\r\n]"); //Ignores useless characters (just in case)
    fscanf(input, "%[^ \t\r\n]", sequence); //Stores the sequence
    fscanf(input, "%*[ \t\r\n]"); //Goes to the start of the node list

    //Fills the alphabet array with 'NULL'
    for(unsigned int i = 0; i < 26; ++i)
        alphabet[i] = NULL;

    //Stores the node in the lists
    char currentNode[4];
    char currentLeftNode[4];
    char currentRightNode[4];     

    do{
        fscanf(input, "%[^ ]", currentNode); //Reads the current node
        fscanf(input, "%*[^A-Z]"); //Ignores useless characters
        fscanf(input, "%[^,]", currentLeftNode); //Reads the current left node
        fscanf(input, "%*[^A-Z]"); //Ignores useless characters
        fscanf(input, "%[^)]", currentRightNode); //Reads the current right node
        fgetc(input); //Ignores the '('  

        //Creates the node and adds it to the right list
        Node* newNode = createNode(currentNode, currentLeftNode, currentRightNode);
        alphabet[currentNode[0] - 'A'] = addNode(alphabet[currentNode[0] - 'A'], newNode);

    }while(fgetc(input) != EOF);

    //Counts the number of steps to reach 'ZZZ'
    NodeList currentList;
    unsigned int nbSteps = 0;
    unsigned int currentInstruction = 0;
    char nextNode[4] = "AAA";

    strcpy(currentNode, "");

    while(strcmp(currentNode, "ZZZ") != 0){
        
        //Searches the node in the right list
        currentList = alphabet[nextNode[0] - 'A'];

        while(strcmp(currentList->node, nextNode))
            currentList = currentList->next;
        
        strcpy(currentNode, nextNode);

        //Gets the next node
        if(sequence[currentInstruction] == 'L')
            strcpy(nextNode, currentList->leftNode);
        else
            strcpy(nextNode, currentList->rightNode);

        ++nbSteps; //Increments the number of steps

        //Goes back to the start of the instruction sequence if needed
        if(currentInstruction == (sequenceLength - 1))
            currentInstruction = 0;
        else
            ++currentInstruction;
    }

    --nbSteps; //Calculates the real number of steps

    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &endTime);
    long executionraceTime_nsec = endTime.tv_nsec - startTime.tv_nsec;
    double executionraceTime_mlsec = executionraceTime_nsec / 1000000.0;
    printf("\033[1;37mExecution time (without prints) : \033[1;31m%lf ms\n", executionraceTime_mlsec);

    //Prints the number
    printf("\033[1;37mHere are the number of steps required to reach 'ZZZ' : \033[1;34m%d\033[0m\n", nbSteps);

    //Frees the memory
    free(sequence);
    for(unsigned int i = 0; i < 26; ++i)
        freeList(alphabet[i]);
}

//Functions

Node* createNode(char* node, char* leftNode, char* rightNode){

    Node* new = (Node*) malloc(sizeof(Node));
    if(new != NULL){
        strcpy(new->node, node);
        strcpy(new->leftNode, leftNode);
        strcpy(new->rightNode, rightNode);
        new->next = NULL;
    }

    return new;
}

NodeList addNode(NodeList list, Node* node){

    if(list == NULL)
        return node;

    NodeList startList = list; //Saves the start of the list
    Node* previous = NULL;

    while(1){

        //Compares the second and third (if needed) characters of the node 
        //with the ones of the current node of the list
        unsigned short idCharacter = 1;

        while(idCharacter < 3){

            if(node->node[idCharacter] > list->node[idCharacter]){
                previous = list;
                list = list->next;
                break;
            }
            else if(node->node[idCharacter] == list->node[idCharacter])
                ++idCharacter;
            else
                break;
        }
            
        if((list == NULL) || (node->node[idCharacter] <= list->node[idCharacter]))
            break;
    }

    //Places the current node at the right position in the list
    if(previous == NULL)
        startList = node;
    else
        previous->next = node;
    if(list != NULL)
        node->next = list;

    return startList;
}

void freeList(NodeList list){

    Node* next = NULL;

    while(list != NULL){
        next = list->next;
        free(list);
        list = next;
    }
}
