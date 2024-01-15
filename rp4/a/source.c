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

parent_and_child_gen find_nearest_child_gen(individual *individuals, parent_and_child_gen* cache, long *ids, long *tmp_ids, long start_gen, long num, long maxGen){
    parent_and_child_gen result;
    result.p_gen = start_gen;
    result.p_id = 0;
    result.c_gen = 0;

    if(start_gen >= maxGen){
        return result;
    }

    if(cache[start_gen].p_gen != 0){
//        printf("cache hit %ld\n",start_gen);
        return cache[start_gen];
    }

    for (long k = 0; k < num; ++k) {
        ids[k] = k;
    }
    for (long j = start_gen; j <= maxGen; ++j) {
        int flg = 1;
        for (long k = 0; k < num; ++k) {
            printf("%ld",individuals[j * num + k].parent_id);
            tmp_ids[k] = ids[individuals[j * num + k].parent_id];
            if(k != 0 && tmp_ids[k - 1] != tmp_ids[k]){
                flg = 0;
            }
        }
        printf("\n");
        if(flg){
            result.c_gen = j;
            result.p_id = tmp_ids[0];
            cache[start_gen] = result;
            return result;
        }
        memcpy(ids, tmp_ids, sizeof(long) * num);
    }
    cache[start_gen] = result;
    return result;
}

parent_and_child_gen find_eve(individual *individuals, long start_gen, long num, long max_gen){
    parent_and_child_gen result;
    result.p_gen = start_gen;
    result.p_id = 0;
    result.c_gen = 0;

    long *parents = calloc(num,sizeof(long));
    long *tmp_parents = calloc(num,sizeof(long));

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
                tmp_parents[individuals[(start_gen-i) * num + j].parent_id] = 1;
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
    free(parents);
    free(tmp_parents);
    return result;
}

void simulate(long maxGen, long num){
    individual *individuals = malloc(sizeof(individual) * num * (maxGen+1));

    parent_and_child_gen* cache = (parent_and_child_gen *)calloc(maxGen, sizeof(parent_and_child_gen));
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

    parent_and_child_gen res;
    res.p_gen = 1;
    res = find_eve(individuals, 16, num, maxGen);
    res = find_nearest_child_gen(individuals, cache, ids, tmp_ids, 23, num, maxGen);
    print_result(1, res.p_gen, res.p_id, res.c_gen);

//    while(1){
//        res = find_nearest_child_gen(individuals, cache, ids, tmp_ids, res.p_gen, num, maxGen);
//        printf("phase1 %ld\n",res.c_gen);
//        if(res.c_gen != 0){
//            res = find_eve(individuals, res.c_gen, num, maxGen);
//            printf("phase2 %ld\n",res.p_gen);
//            print_result(times+1, res.p_gen, res.p_id, res.c_gen);
//            res.p_gen++;
//        }else{
//            break;
//        }
//    }

//    parent_and_child_gen last_result = find_nearest_child_gen(individuals, cache, ids, tmp_ids, 1, num, maxGen);
//    parent_and_child_gen* stack = malloc(sizeof(parent_and_child_gen) * num);
//    long stack_pointer = 0;
//    long step = num;
//    while(last_result.c_gen != 0){
//        long c_gen = last_result.p_gen + step;
//        if(c_gen > maxGen){
//            c_gen = last_result.p_gen+1;
//        }
////        printf("finding %ld\n",c_gen);
//        parent_and_child_gen res = find_nearest_child_gen(individuals, cache, ids, tmp_ids, c_gen, num, maxGen);
//        if(res.c_gen != last_result.c_gen){
//            for (int j = 0; j < step; ++j) {
////                printf(">>>finding %ld\n",last_result.p_gen + j);
//                res =  find_nearest_child_gen(individuals, cache, ids, tmp_ids, last_result.p_gen + j, num, maxGen);
//                if(res.c_gen != 0){
//                    if(res.c_gen > eveGen){
//                        if(times != 0){
//                            print_result(times, lastGen - 1, lastId, eveGen);
//                        }
//                        eveGen = res.c_gen;
//                        times++;
//                        break;
//                    }
//                    lastGen = res.p_gen;
//                    lastId = res.p_id;
//                }
//            }
//        }
//        last_result = res;
//    }
    free(individuals);
//    free(ids);
//    free(tmp_ids);
//    free(stack);
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