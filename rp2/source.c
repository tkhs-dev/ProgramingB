#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 1000
#define OPERATOR_PLUS '+'
#define OPERATOR_MINUS '-'
#define OPERATOR_MULTIPLY '*'
#define OPERATOR_DIVIDE '/'
#define BRACKET_OPEN '('
#define BRACKET_CLOSE ')'
#define DOT '.'

struct block{
    double operand1;
    double operand2;
    char operator;
};

double calc_block(struct block block){
    if(block.operator == OPERATOR_PLUS){
        return block.operand1 + block.operand2;
    } else if(block.operator == OPERATOR_MINUS){
        return block.operand1 - block.operand2;
    } else if(block.operator == OPERATOR_MULTIPLY){
        return block.operand1 * block.operand2;
    }else if(block.operator == OPERATOR_DIVIDE){
        if(block.operand2 == 0){
            printf("ゼロ除算エラー\n");
            exit(1);
        }
        return block.operand1 / block.operand2;
    }
    return 0;
}

//括弧内の数式について処理する
//引数で与えられる数式に括弧が含まれないことは保証されている
double calc(char* input){
    char tmp[200];

    //先頭に0+を追加しておく for #1
    tmp[0] = '0';
    tmp[1] = OPERATOR_PLUS;
    int i = 0;
    while(input[i] != '\0'){
        tmp[i+2] = input[i];
        i++;
    }
    tmp[i+2] = '\0';

    //演算子の数を数え上げる
    int count = 0;
    i = 0;
    while(tmp[i] != '\0'){
        if(tmp[i] == OPERATOR_PLUS || tmp[i] == OPERATOR_MINUS || tmp[i] == OPERATOR_MULTIPLY || tmp[i] == OPERATOR_DIVIDE){
            if(tmp[i+1] == OPERATOR_MINUS){
                i++; //負の値としてのマイナス記号を無視する
            }
            count++;
        }
        i++;
    }

    //演算子ごとに分割して構造体に格納する
    struct block blocks[count];
    i = 0;
    count = 0;
    while(tmp[i] != '\0'){
        if(tmp[i] == OPERATOR_PLUS || tmp[i] == OPERATOR_MINUS || tmp[i] == OPERATOR_MULTIPLY || tmp[i] == OPERATOR_DIVIDE){
            blocks[count].operator = tmp[i];

            char tmp2[20];

            //演算子のあとの被演算子
            int j = i+1;
            if(tmp[j] == OPERATOR_MINUS){
                j++; //負の値としてのマイナス記号を無視する
            }
            while(tmp[j] != '\0' && tmp[j] != OPERATOR_PLUS && tmp[j] != OPERATOR_MINUS && tmp[j] != OPERATOR_MULTIPLY && tmp[j] != OPERATOR_DIVIDE){
                j++;
            }
            strncpy(tmp2, tmp+i+1, j-i-1);
            tmp2[j-i-1] = '\0';
            blocks[count].operand2 = atof(tmp2);

            //演算子の前の被演算子
            j = i-1;
            while(j>=0 && tmp[j] != OPERATOR_PLUS && tmp[j] != OPERATOR_MINUS && tmp[j] != OPERATOR_MULTIPLY && tmp[j] != OPERATOR_DIVIDE){
                j--;
            }
            if(tmp[j] == OPERATOR_MINUS && j!=0 && !(tmp[j-1] >= '0' && tmp[j-1] <= '9')){
                j--; //負の値としてのマイナス記号を無視する
            }
            strncpy(tmp2, tmp+j+1, i-j-1);
            tmp2[i-j-1] = '\0';
            blocks[count].operand1 = atof(tmp2);

            if(tmp[i+1] == OPERATOR_MINUS){
                i++; //負の値としてのマイナス記号を無視する
            }
            count++;
        }
        i++;
    }

    //掛け算と負の数を先に処理してすべて加算に変換する
    for(int i=count-1; i>=0; i--){
        if(blocks[i].operator == OPERATOR_MULTIPLY || blocks[i].operator == OPERATOR_DIVIDE){
            double res = calc_block(blocks[i]);
            blocks[i].operand1 = res;
            blocks[i].operator = OPERATOR_PLUS;
            blocks[i].operand2 = 0;

            //前後の被演算子を更新
            if(i+1 != count){
                blocks[i+1].operand1 = 0;
            }
            if(i-1 >= 0){
                blocks[i-1].operand2 = res;
            }
        }
        //減算を負の数の加算に変換
        if(blocks[i].operator == OPERATOR_MINUS){
            blocks[i].operator = OPERATOR_PLUS;
            blocks[i].operand2 = blocks[i].operand2 * -1;
            if(i+1 != count){
                blocks[i+1].operand1 = blocks[i+1].operand1 * -1;
            }
        }
    }

    //加算のみの数式を計算
    double res = 0;
    for(int i=count-1; i>=0; i--){
        res = res + blocks[i].operand1;
    }
    res = res + blocks[count-1].operand2;


    return res;
}

//括弧を含む式を再帰的に計算する
double recursive_calc(char* input){
    //括弧がなくなるまで繰り返す
    int i = 0;
    while(input[i] != '\0'){
        if(input[i] == BRACKET_OPEN){
            int j = i+1;
            int count = 1;
            while(count != 0){
                if(input[j] == BRACKET_OPEN){
                    count++;
                }
                if(input[j] == BRACKET_CLOSE){
                    count--;
                }
                j++;
            }
            char tmp[200];
            strncpy(tmp, input+i+1, j-i-2);
            tmp[j-i-2] = '\0';
            double res = recursive_calc(tmp);

            char tmp2[200]; //括弧の前の部分
            strncpy(tmp2, input, i);
            tmp2[i] = '\0';
            char tmp3[200]; //括弧の後の部分
            strncpy(tmp3, input+j, strlen(input)-j);
            tmp3[strlen(input)-j] = '\0';
            sprintf(input, "%s%lf%s", tmp2, res, tmp3); //括弧を計算結果に置き換える
            i = 0;
        }
        i++;
    }

    return calc(input);
}

int main(){
    char input[MAX];
    scanf("%[^\n]", &input);

    //sanitize
    char formula[MAX];
    int count_in = 0;
    int count_res = 0;
    while(input[count_in] != '\0'){
        if(input[count_in] == OPERATOR_PLUS || input[count_in] == OPERATOR_MINUS || input[count_in] == OPERATOR_MULTIPLY || input[count_in] == OPERATOR_DIVIDE || input[count_in] == BRACKET_OPEN || input[count_in] == BRACKET_CLOSE || input[count_in] == DOT|| (input[count_in] >= '0' && input[count_in] <= '9')){
            formula[count_res] = input[count_in];
            count_res++;
        }
        count_in++;
    }
    formula[count_res] = '\0';

    //validate
    int i = 0;
    while(formula[i] != '\0'){
        if(formula[i] == OPERATOR_PLUS || formula[i] == OPERATOR_MINUS || formula[i] == OPERATOR_MULTIPLY || formula[i] == OPERATOR_DIVIDE){
            if((formula[i+1] < '0' || formula[i+1] > '9')&&formula[i+1] != BRACKET_OPEN){
                //検証: 演算子の後に数字が続かない場合
                printf("Invalid formula");
                return -1;
            }
        }
        if(formula[i] == BRACKET_CLOSE && (formula[i+1] >= '0' && formula[i+1] <= '9')){
            //検証: )の後に数字が続く場合
            printf("Invalid formula");
            return -1;
        }
        if(formula[i] == BRACKET_OPEN){
            int j = i+1;
            if(formula[i-1] >= '0' && formula[i-1] <= '9'){
                //検証: 数字の後に(が続く場合
                printf("Invalid formula");
                return -1;
            }
            if(formula[j] == OPERATOR_PLUS || formula[j] == OPERATOR_MULTIPLY || formula[j] == OPERATOR_DIVIDE){
                //検証: (の後に+または*が続く場合
                printf("Invalid formula");
                return -1;
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
                return -1;
            }
        }
        if(formula[i] == DOT){
            if(formula[i-1] < '0' || formula[i-1] > '9'){
                //検証: .の前に数字が続かない場合
                printf("Invalid formula");
                return -1;
            }
            if(formula[i+1] < '0' || formula[i+1] > '9'){
                //検証: .の後に数字が続かない場合
                printf("Invalid formula");
                return -1;
            }
            int j = 1;
            while(formula[i-j] != OPERATOR_PLUS && formula[i-j] != OPERATOR_MINUS && formula[i-j] != OPERATOR_MULTIPLY && formula[i-j] != OPERATOR_DIVIDE && formula[i-j] != BRACKET_OPEN && formula[i-j] != BRACKET_CLOSE && i-j >= 0){
                if(formula[i-j] == DOT){
                    //検証: .が2つ以上続く場合
                    printf("Invalid formula");
                    return -1;
                }
                j++;
            }
            j=1;
            while(formula[i+j] != OPERATOR_PLUS && formula[i+j] != OPERATOR_MINUS && formula[i+j] != OPERATOR_MULTIPLY && formula[i+j] != OPERATOR_DIVIDE && formula[i+j] != BRACKET_OPEN && formula[i+j] != BRACKET_CLOSE && formula[i+j] != '\0'){
                if(formula[i+j] == DOT){
                    //検証: .が2つ以上続く場合
                    printf("Invalid formula");
                    return -1;
                }
                j++;
            }
        }
        i++;
    }
    //printf("Sanitized %s\n", formula);

    //calculate
    double res = recursive_calc(formula);
    printf("%lf", res);

    return 0;
}