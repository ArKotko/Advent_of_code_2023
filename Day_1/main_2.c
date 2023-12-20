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

    unsigned int firstNum = 0, secondNum = -1, tempNum = -1, number = 0, charCount = 0;
    unsigned long int sum = 0;
    size_t it = 1;
    char currentChar, string[6];

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
        else if(currentChar >= 'a' && currentChar <= 'z'){
            //Add the current character to the string
            string[0] = currentChar;
            //Add the two next characters to the string
            string[1] = fgetc(input);
            if(string[1] == EOF){
                string[1] = '\0';
                charCount = 1;
            }
            else{
                string[2] = fgetc(input);
                if(string[2] == EOF){
                    string[2] = '\0';
                    charCount = 2;
                }
                else{
                    string[3] = '\0';
                    charCount = 3;
                }
            }
            //Three characters
            if(!strcmp(string, "one") || !strcmp(string, "two") || !strcmp(string, "six")){   
                if(!strcmp(string, "one"))
                    tempNum = 1;
                else if(!strcmp(string, "two"))
                    tempNum = 2;
                else if(!strcmp(string, "six"))
                    tempNum = 6;
            }
            else if(!strcmp(string, "thr") || !strcmp(string, "fou") || !strcmp(string, "fiv") || !strcmp(string, "sev") || !strcmp(string, "eig") || !strcmp(string, "nin")){
                //Add the next character to the string
                string[3] = fgetc(input);
                if(string[3] == EOF)
                    string[3] = '\0';
                else{
                    string[4] = '\0';
                    ++charCount;
                }
                //Four characters
                if(!strcmp(string, "four") || !strcmp(string, "five") || !strcmp(string, "nine")){   
                    if(!strcmp(string, "four"))
                        tempNum = 4;
                    else if(!strcmp(string, "five"))
                        tempNum = 5;
                    else if(!strcmp(string, "nine"))
                        tempNum = 9;        
                }
                //Five characters
                else if (!strcmp(string, "thre") || !strcmp(string, "seve") || !strcmp(string, "eigh")){ 
                    //Add the next character to the string
                    string[4] = fgetc(input);
                    if(string[4] == EOF)
                        string[4] = '\0';
                    else{
                        string[5] = '\0';
                        ++charCount;
                    }
                    //Five characters
                    if((charCount == 5) && (!strcmp(string, "three") || !strcmp(string, "seven") || !strcmp(string, "eight"))){   
                        if(!strcmp(string, "three"))
                            tempNum = 3;
                        else if(!strcmp(string, "seven"))
                            tempNum = 7;
                        else if(!strcmp(string, "eight"))
                            tempNum = 8;
                    }
                }
            }

            //Keep the number if one has been found
            if(tempNum != -1){
                if(it == 1)
                    firstNum = tempNum;
                else if(it > 1)
                    secondNum = tempNum;
                tempNum = -1;
                ++it;
            }

            //Replace the file pointer at the right position
            fseek(input, -(--charCount), SEEK_CUR);
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

    printf("Here is your sum big boy : %ld\n", sum);

    fclose(input);

    return 0;
}