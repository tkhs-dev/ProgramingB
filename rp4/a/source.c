#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "MT.h"

typedef struct{
    long eveGen; //世代
    long eveId; //ID
    long foundGen; //発見された世代
} eve_gen;

typedef struct{
    long gen; //世代
    long id; //ID
    long parent_id; //親のID
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

eve_gen get_eve_gen(individual *individuals, eve_gen* cache, long *ids, long *tmp_ids, long start_gen, long num, long maxGen){
    eve_gen result;
    result.eveGen = start_gen;
    result.eveId = 0;
    result.foundGen = 0;

    if(start_gen >= maxGen){
        return result;
    }

    if(cache[start_gen].eveGen != 0){
//        printf("cache hit %ld\n",start_gen);
        return cache[start_gen];
    }

    for (long k = 0; k < num; ++k) {
        ids[k] = k;
    }
    for (long j = start_gen; j <= maxGen; ++j) {
        int flg = 1;
        for (long k = 0; k < num; ++k) {;
            tmp_ids[k] = ids[individuals[j * num + k].parent_id];
            if(k != 0 && tmp_ids[k - 1] != tmp_ids[k]){
                flg = 0;
            }
        }
        if(flg){
            result.foundGen = j;
            result.eveId = tmp_ids[0];
            cache[start_gen] = result;
            return result;
        }else{
            memcpy(ids, tmp_ids, sizeof(long) * num);
        }
    }
    cache[start_gen] = result;
    return result;
}

void simulate(long maxGen, long num){
    individual *individuals = malloc(sizeof(individual) * num * (maxGen+1));

    eve_gen* cache = (eve_gen *)calloc(maxGen, sizeof(eve_gen));
    long eveGen=0;
    long times=0;
    long *ids = malloc(sizeof(long) * num);
    long *tmp_ids = malloc(sizeof(long) * num);

    long lastGen = 1;
    long lastId = 0;

    for (long i = 1; i < maxGen; ++i) {
        for (long j = 0; j < num; ++j) {
            individuals[i * num + j].parent_id = nextMother();
            individuals[i * num + j].gen = i;
            individuals[i * num + j].id = j;
        }
    }
    for (long j = 0; j < num; ++j) {
        individuals[maxGen * num + j].parent_id = 0;
        individuals[maxGen * num + j].gen = maxGen;
        individuals[maxGen * num + j].id = j;
    }
    eve_gen last_result = get_eve_gen(individuals, cache, ids, tmp_ids, 1, num, maxGen);
    eve_gen* stack = malloc(sizeof(eve_gen) * num);
    long stack_pointer = 0;
    long step = num;
    while(last_result.foundGen != 0){
        long gen = last_result.eveGen + step;
        if(gen > maxGen){
            gen = last_result.eveGen+1;
        }
//        printf("finding %ld\n",gen);
        eve_gen res = get_eve_gen(individuals, cache, ids, tmp_ids, gen, num, maxGen);
        if(res.foundGen != last_result.foundGen){
            for (int j = 0; j < step; ++j) {
//                printf(">>>finding %ld\n",last_result.eveGen + j);
                res =  get_eve_gen(individuals, cache, ids, tmp_ids, last_result.eveGen + j, num, maxGen);
                if(res.foundGen != 0){
                    if(res.foundGen > eveGen){
                        if(times != 0){
                            print_result(times, lastGen - 1, lastId, eveGen);
                        }
                        eveGen = res.foundGen;
                        times++;
                        break;
                    }
                    lastGen = res.eveGen;
                    lastId = res.eveId;
                }
            }
        }
        last_result = res;
    }
    free(individuals);
    free(ids);
    free(tmp_ids);
    free(stack);
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