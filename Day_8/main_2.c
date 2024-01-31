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
unsigned long long gcd(unsigned long long a, unsigned long long b);
unsigned long long lcm(unsigned long long a, unsigned long long b);

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
        alphabet[currentNode[2] - 'A'] = addNode(alphabet[currentNode[2] - 'A'], newNode);

    }while(fgetc(input) != EOF);

    //Finds the cycle of every starting node
    char nextNode[4];
    unsigned short nbNodes = 0;
    unsigned int currentInstruction;
    unsigned long long nbSteps;
    unsigned long long* cycles = (unsigned long long*) malloc(sizeof(unsigned long long) * 1);
    NodeList currentList;

    NodeList listA = alphabet[0]; //Gets the first list of the array ('A')

    while(listA != NULL){

        strcpy(currentNode, "   "); //"Initialization" of the current node
        strcpy(nextNode, listA->node); //"Initialization" of the next node
        currentList = NULL; //"Initialization" of the current list
        currentInstruction = 0;
        nbSteps = 0;

        //Searches the node in the right list
        while(1){
            
            currentList = alphabet[nextNode[2] - 'A']; //Finds the right list

            //Searches the node
            while(strcmp(currentList->node, nextNode))
                currentList = currentList->next;

            strcpy(currentNode, nextNode);
 
            //Gets the next node
            if(sequence[currentInstruction] == 'L')
                strcpy(nextNode, currentList->leftNode);
            else
                strcpy(nextNode, currentList->rightNode);

            ++nbSteps; //Increments the number of steps

            //Check if an end node has been reached
            if(nextNode[2] == 'Z'){
                cycles = realloc(cycles, (sizeof(unsigned long long) * (nbNodes + 1)));
                cycles[nbNodes] = nbSteps; //Saves the number of steps (the period of the cycle)
                ++nbNodes;
                break;
            }

            //Goes back to the start of the instruction sequence if needed
            if(currentInstruction == (sequenceLength - 1))
                currentInstruction = 0;
            else
                ++currentInstruction;
        }
        
        listA = listA->next; //Goes to the next node in the A list
    }

    unsigned long long result = cycles[0];

    //Calculates the lcm of the cycles
    for(unsigned int i = 1; i < nbNodes; ++i)
        result = lcm(result, cycles[i]);

    struct timespec endTime;
    clock_gettime(CLOCK_REALTIME, &endTime);
    long executionraceTime_nsec = endTime.tv_nsec - startTime.tv_nsec;
    double executionraceTime_mlsec = executionraceTime_nsec / 1000000.0;
    printf("\033[1;37mExecution time (without prints) : \033[1;31m%lf ms\n", executionraceTime_mlsec);

    //Prints the number
    printf("\033[1;37mHere are the number of steps required before you're only on nodes that end with 'Z' : \033[1;34m%lld\033[1;37m\n--> Just so you know, there were \033[1;32m%hd\033[1;37m different starting nodes\033[0m", result, nbNodes);

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

        //Compares the second and first (if needed) characters of the node 
        //with the ones of the current node of the list
        short idCharacter = 1;

        while(idCharacter > -1){

            if(node->node[idCharacter] > list->node[idCharacter]){
                previous = list;
                list = list->next;
                break;
            }
            else if(node->node[idCharacter] == list->node[idCharacter])
                --idCharacter;
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

unsigned long long gcd(unsigned long long a, unsigned long long b){

    unsigned long long temp;

    while(b){
        temp = a % b;
        a = b;
        b = temp;
    }

    return a;
}

unsigned long long lcm(unsigned long long a, unsigned long long b){

    return (a * b) / gcd(a, b);
}