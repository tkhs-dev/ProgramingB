#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "MT.h"

typedef struct{
    long p_gen; //親の世代
    long p_id; //親のID
    long c_gen; //子の世代
} parent_and_child_gen;

typedef struct{
    long gen; //世代
    long id; //ID
    long p_id; //親のID
} individual;

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

parent_and_child_gen find_nearest_child_gen(individual *individuals, long *ids, long *tmp_ids, long start_gen, long num, long maxGen){
    parent_and_child_gen result;
    result.p_gen = start_gen;
    result.p_id = 0;
    result.c_gen = 0;

    if(start_gen >= maxGen){
        return result;
    }

    for (long k = 0; k < num; ++k) {
        ids[k] = k;
    }
    for (long j = start_gen+1; j < maxGen; ++j) {
        int flg = 1;
        for (long k = 0; k < num; ++k) {
            tmp_ids[k] = ids[individuals[j * num + k].p_id];
            if(k != 0 && tmp_ids[k - 1] != tmp_ids[k]){
                flg = 0;
            }
        }
        if(flg){
            result.c_gen = j;
            result.p_id = tmp_ids[0];
            return result;
        }
        memcpy(ids, tmp_ids, sizeof(long) * num);
    }
    return result;
}

parent_and_child_gen find_eve(individual *individuals, long *parents, long *tmp_parents,long start_gen, long num, long max_gen){
    parent_and_child_gen result;
    result.p_gen = start_gen;
    result.p_id = 0;
    result.c_gen = 0;

    for (long k = 0; k < num; ++k) {
        parents[k] = 1;
    }
    long last_bit = 0;
    for (long i = 0; i < start_gen; ++i) {
        long ctr = 0;
        for (int j = 0; j < num; ++j) {
            tmp_parents[j] = 0;
        }
        for (int j = 0; j < num; ++j) {
            if(parents[j]){
                ctr++;
                last_bit = j;
                tmp_parents[individuals[(start_gen-i) * num + j].p_id] = 1;
            }
        }
        if(ctr == 1){
            result.p_gen = start_gen - i;
            result.p_id = last_bit;
            result.c_gen = start_gen;
            break;
        }
        memcpy(parents, tmp_parents, sizeof(long) * num);
    }
    return result;
}

void simulate(long maxGen, long num){
    individual *individuals = malloc(sizeof(individual) * num * (maxGen+1));

    long times=0;
    long *ids = malloc(sizeof(long) * num);
    long *tmp_ids = malloc(sizeof(long) * num);

    for (long i = 1; i < maxGen; ++i) {
        for (long j = 0; j < num; ++j) {
            individuals[i * num + j].p_id = nextMother();
            individuals[i * num + j].gen = i;
            individuals[i * num + j].id = j;
        }
    }

    parent_and_child_gen res;
    res.p_gen = 1;

    while(1){
        res = find_nearest_child_gen(individuals, ids, tmp_ids, res.p_gen, num, maxGen);
        if(res.c_gen != 0){
            res = find_eve(individuals, ids, tmp_ids, res.c_gen, num, maxGen);
            times++;
            print_result(times, res.p_gen, res.p_id, res.c_gen);
            res.p_gen++;
        }else{
            break;
        }
    }
    free(individuals);
    free(ids);
    free(tmp_ids);
    printf("Eve occurrence rate is %f\n",(double) (times-1)/maxGen);
}

int main(int argc, char **argv){
    clock_t t1,t2;

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