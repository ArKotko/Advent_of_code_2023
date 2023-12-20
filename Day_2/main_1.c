#include <stdlib.h>
#include <stdio.h>
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

    unsigned int colors[3] = {0, 0, 0}; //0 is red, 1 is green and 2 is blue
    unsigned int idGame, sumId = 0, numCubes;
    char colorCube[6], currentChar;

    while(!feof(input)){
        
        //Read the word "game"
        fscanf(input, "%*s");
        //"Consume" the blankspace
        fgetc(input);

        //Get the game ID
        fscanf(input, "%d", &idGame);

        //"Consume" the blankspace and the ':'
        fscanf(input, "%*[: ]");

        //Get the maximum number of cubes for each color in each game
        do{
            //Get the number of cubes
            fscanf(input, "%d", &numCubes);
            //"Consume" the blankspace
            fgetc(input);
            //Get the color of the cubes
            fscanf(input, "%[^,;\r]", colorCube);

            //Put the number in the right spot of the array
            if(!strcmp(colorCube, "red") && (numCubes > colors[0]))
                colors[0] = numCubes;
            else if(!strcmp(colorCube, "green") && (numCubes > colors[1]))
                colors[1] = numCubes;
            else if(!strcmp(colorCube, "blue") && (numCubes > colors[2]))
                colors[2] = numCubes;

            //Get the next character
            currentChar = fgetc(input);

            if(currentChar != EOF && (currentChar == ',' || currentChar == ';'))
                //"Consume" the blankspace
                fgetc(input);
            else if(currentChar != EOF && currentChar == '\r'){
                //"Consume" the '\n'
                currentChar = fgetc(input);
            }
        }while(currentChar != '\n' && currentChar != EOF);

        //char temp[3] = "NO";

        //Check if the game is possible
        if(colors[0] <= 12 && colors[1] <= 13 && colors[2] <= 14){
            sumId += idGame;
            //strcpy(temp, "OK");
        }

        //printf("Game %d --> red : %d; green : %d; blue %d -- %s\n", idGame, colors[0], colors[1], colors[2], temp);

        //Set the values of the array back to zero
        colors[0] = 0;
        colors[1] = 0;
        colors[2] = 0;
    }

    printf("Here is the sum of the IDs : %d\n", sumId);

    return 0;
}