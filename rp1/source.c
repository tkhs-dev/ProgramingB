#include <stdio.h>

#define MAX 100
#define OPERATOR_PLUS '+'
#define OPERATOR_MINUS '-'
#define OPERATOR_MULTIPLY '*'
#define BRACKET_OPEN '('
#define BRACKET_CLOSE ')'

void main(){
    char input[MAX];
    scanf("%[^\n]", &input);

    printf("Hello %s\n", input);

    //sanitize
    char formula[MAX];
    int count_in = 0;
    int count_res = 0;
    while(count_in < MAX){
        if(input[count_in] == OPERATOR_PLUS || input[count_in] == OPERATOR_MINUS || input[count_in] == OPERATOR_MULTIPLY || input[count_in] == BRACKET_OPEN || input[count_in] == BRACKET_CLOSE || (input[count_in] >= '0' && input[count_in] <= '9')){
            formula[count_res] = input[count_in];
            count_res++;
        }
        count_in++;
    }
    formula[count_res] = '\0';

    //validate


    printf("Hello %s", formula);
}