#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "MT.h"

#define MEMORY_ALLOCATION 2900 //1MB

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

typedef struct{
    long start_gen;
    long end_gen;
} loaded_gen;

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

parent_and_child_gen find_nearest_child_gen(individual *individuals,loaded_gen *loaded, long *ids, long *tmp_ids, long start_gen, long num, long maxGen){
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
        //memory allocation
        if(j >= loaded->end_gen){
            loaded->start_gen = start_gen;
            if(MEMORY_ALLOCATION > sizeof(individual) * num * (maxGen - start_gen)){
                loaded->end_gen = maxGen;
            }else{
                loaded->end_gen = start_gen + MEMORY_ALLOCATION / (sizeof(individual) * num);
            }
            memmove(individuals, individuals + num * (j-loaded->start_gen+4), sizeof(individual) * num * (j-loaded->start_gen+1));

            for (long i = j-loaded->start_gen; i < loaded->end_gen - loaded->start_gen; ++i) {
                for (long k = 0; k < num; ++k) {
                    individuals[i * num + k].p_id = nextMother();
                    individuals[i * num + k].gen = i;
                    individuals[i * num + k].id = k;
                }
            }
        }


        int flg = 1;
        for (long k = 0; k < num; ++k) {
            tmp_ids[k] = ids[individuals[(j-loaded->start_gen) * num + k].p_id];
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

parent_and_child_gen find_eve(individual *individuals, loaded_gen *loaded, long *parents, long *tmp_parents,long start_gen, long num, long max_gen){
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
        for (long j = 0; j < num; ++j) {
            tmp_parents[j] = 0;
        }
        for (long j = 0; j < num; ++j) {
            if(parents[j]){
                ctr++;
                last_bit = j;
                tmp_parents[individuals[(start_gen-i-loaded->start_gen) * num + j].p_id] = 1;
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
    individual *individuals;
    long times=0;
    long *ids = malloc(sizeof(long) * num);
    long *tmp_ids = malloc(sizeof(long) * num);

    loaded_gen loaded;
    loaded.start_gen = 0;
    long load_num;

    if(MEMORY_ALLOCATION > sizeof(individual) * num * maxGen){
        load_num = maxGen;
    }else{
        load_num = MEMORY_ALLOCATION / (sizeof(individual) * num);
    }
    individuals = malloc(sizeof(individual) * num * load_num);
    for (long i = 1; i < load_num; ++i) {
        for (long j = 0; j < num; ++j) {
            individuals[i * num + j].p_id = nextMother();
            individuals[i * num + j].gen = i;
            individuals[i * num + j].id = j;
        }
    }
    loaded.end_gen = load_num;

    parent_and_child_gen res;
    res.p_gen = 1;
    res.c_gen = 1;

    while(res.c_gen != 0){
        res = find_nearest_child_gen(individuals, &loaded, ids, tmp_ids, res.p_gen, num, maxGen);
        res = find_eve(individuals, &loaded, ids, tmp_ids, res.c_gen, num, maxGen);
        times++;
        print_result(times, res.p_gen, res.p_id, res.c_gen);
        res.p_gen++;
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