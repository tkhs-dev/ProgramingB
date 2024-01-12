#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "MT.h"

//大域変数
long num, maxGen;


/*
  娘を出産する母親を決定．
  現役世代のi番目の女性が母親となる
  編集厳禁．ここ以外でgenrand_*を使用しない．
*/
long nextMother(){
    return (long) (genrand_real3()*num);
}

void print_result(long ordinal, long eveGen, long eveId, long foundGen){
    char *th;
    if((ordinal % 10 == 1) && (ordinal % 100 !=11)){
        th = "st";
    }else if((ordinal % 10 == 2) && (ordinal % 100 !=12)){
        th = "nd";
    }else if((ordinal % 10 == 3) && (ordinal % 100 !=13)){
        th = "rd";
    }
    else{
        th = "th";
    }
    char res[100];
    sprintf(res,"The %ld%s Eve (%ld,%ld) occurs at %ld th generation", ordinal,th,eveGen,eveId,foundGen);
    puts(res);
}

void simulate(long maxGen, long num){
    long *mito = (long *)malloc(sizeof(long) * num * (maxGen+1));
    long eveGen=0;
    long times=0;
    long *ids = malloc(sizeof(long) * num);
    long *tmp_ids = malloc(sizeof(long) * num);

    long lastGen = 0;
    long lastId = 0;

    for (long i = 1; i < maxGen; ++i) {
        for (long j = 0; j < num; ++j) {
            mito[i * num + j] = nextMother();
        }
    }
    for (long j = 0; j < num; ++j) {
        mito[maxGen * num + j] = 0;
    }
    for (long i = 1; i < maxGen; ++i) {
        for (long i = 0; i < num; ++i) {
            ids[i] = i;
        }
        for (long j = i; j <= maxGen; ++j) {
            int flg = 1;
            for (long k = 0; k < num; ++k) {;
                tmp_ids[k] = ids[mito[j * num + k]];
                if(k != 0 && tmp_ids[k - 1] != tmp_ids[k]){
                    flg = 0;
                }
            }
            if(flg){
                if(eveGen < j){
                    if(times != 0){
                        print_result(times, lastGen-1,lastId, eveGen);
                    }
                    eveGen = j;
                    times++;
                }else{
                    for (long k = 0; k < num; ++k) {
                        ids[k] = k;
                    }
                }
                lastGen = i;
                lastId = tmp_ids[0];
                break;
            }else{
                memcpy(ids, tmp_ids, sizeof(long) * num);
            }
        }
    }
    free(mito);
    free(ids);
    free(tmp_ids);
    printf("Eve occurrence rate is %f\n",(double) (times-1)/maxGen);
}

int main(int argc, char **argv){
    long times=0;
    clock_t t1,t2;
    char *th;

    /*******初期設定*******/
    if(argc != 4){
        printf("this program requires exactly three arguments\n");
        printf("arg1 = num, arg2 = max_gen, arg3 = seed\n");
        exit(-1);
    }
    num = atol(argv[1]); //1世代の人口N．大域変数．
    maxGen = atol(argv[2]); //世代数の上限G．大域変数．
    int seed = atoi(argv[3]);
    init_genrand(seed);//乱数生成の初期化．【編集厳禁!】
    t1 = clock();

    simulate(maxGen, num);

    t2 = clock();
    printf("Done within ### %f sec . ###\n", (double) (t2-t1)/CLOCKS_PER_SEC);

    return 0;
}