#include <stdio.h>
#include <string.h>
#include <math.h>

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
        tmp[i+1] = '\0';
    } else{
        strcpy(tmp, input);
    }

    return 10;
}

void main(){
    char input[MAX];
    scanf("%[^\n]", &input);

    //sanitize
    char formula[MAX];
    int count_in = 0;
    int count_res = 0;
    while(input[count_in] != '\0'){
        if(input[count_in] == OPERATOR_PLUS || input[count_in] == OPERATOR_MINUS || input[count_in] == OPERATOR_MULTIPLY || input[count_in] == BRACKET_OPEN || input[count_in] == BRACKET_CLOSE || (input[count_in] >= '0' && input[count_in] <= '9')){
            formula[count_res] = input[count_in];
            count_res++;
        }
        count_in++;
    }
    formula[count_res] = '\0';

    //validate
    int i = 0;
    while(formula[i] != '\0'){
        if(formula[i] == OPERATOR_PLUS || formula[i] == OPERATOR_MINUS || formula[i] == OPERATOR_MULTIPLY){
            if((formula[i+1] < '0' || formula[i+1] > '9')&&formula[i+1] != BRACKET_OPEN){
                //検証: 演算子の後に数字が続かない場合
                printf("Invalid formula");
                return;
            }
        }
        if(formula[i] == BRACKET_CLOSE && (formula[i+1] >= '0' && formula[i+1] <= '9')){
            //検証: )の後に数字が続く場合
            printf("Invalid formula");
            return;
        }
        if(formula[i] == BRACKET_OPEN){
            int j = i+1;
            if(formula[i-1] >= '0' && formula[i-1] <= '9'){
                //検証: 数字の後に(が続く場合
                printf("Invalid formula");
                return;
            }
            if(formula[j] == OPERATOR_PLUS || formula[j] == OPERATOR_MULTIPLY){
                //検証: (の後に+または*が続く場合
                printf("Invalid formula");
                return;
            }

            int count = 1;
            while(formula[j] != '\0'){
                if(formula[j] == BRACKET_OPEN){
                    count++;
                } else if(formula[j] == BRACKET_CLOSE){
                    count--;
                }
                if(count == 0){
                    break;
                }
                j++;
            }
            if(count != 0){
                //検証: 括弧の数が合わない場合
                printf("Invalid formula");
                return;
            }
        }
        i++;
    }
    printf("Sanitized %s\n", formula);

    char tmp[MAX];
    //calc
    while(1){
        i = 0;
        int j = 0;
        int flg = 0;
        while(formula[i] != '\0'){
            if(formula[i] == BRACKET_CLOSE){
                j = i-1;
                while(formula[j]!=BRACKET_OPEN){
                    j--;
                }
                strncpy(tmp, formula+j+1,i-j-1);
                tmp[i-j-1] = '\0';
                printf("%s\n",tmp);
                flg = 1;
                break;
            }
            i++;
        }

        if(flg){
            char res[10];
            int num = sprintf(res,"%d", calc(tmp));

            //計算結果の置き換え
            int m = 0;
            int k = 0;
            while(formula[k] != '\0'){
                if(j==k){
                    int l = 0;
                    while(l!=num){
                        tmp[m+l] = res[l];
                        l++;
                    }
                    k = i+1;
                    m=m+l;
                }
                tmp[m] = formula[k];
                k++;
                m++;
            }
            tmp[m]='\0';
            i = 0;
            while(tmp[i] != '\0'){
                formula[i] = tmp[i];
                i++;
            }
            formula[i] = '\0';
            printf("aaa %s\n", tmp);
        }
        if(flg == 0) break;
    }
    printf("Calculated: %s", formula);
}