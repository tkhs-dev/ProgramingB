#include <stdio.h>
#include <string.h>

#define MAX 100
#define OPERATOR_PLUS '+'
#define OPERATOR_MINUS '-'
#define OPERATOR_MULTIPLY '*'
#define BRACKET_OPEN '('
#define BRACKET_CLOSE ')'

//括弧内の数式についてのみ処理する
//引数で与えられる数式に括弧が含まれないことは保証されている
int calc(char* input){
    //負の数で始まる場合を処理
    char tmp[200];
    if(input[0] == '-'){
        tmp[0] = '0';
        int i = 0;
        while(input[i] != '\0'){
            tmp[i+1] = input[i];
            i++;
        }
    } else{
        strcpy(tmp, input);
    }


}

void main(){
    char input[MAX];
    scanf("%[^\n]", &input);

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


    printf("Sanitized %s", formula);
}