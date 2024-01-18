#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#define ADD "add"
#define CHK "check"
#define DEL "delete"
#define EXIT "exit"
#define C_MAX 30 //コマンド用バッファ長．6で十分だが入力ミスに備えて30としておく．
#define K_MAX 50 //キー用バッファ長．
#define V_MAX 200 //バリュー用バッファ長．
#define TABLE_SIZE 1000000 //テーブルサイズ．

//構造体
typedef struct Item item;
struct Item{
    char *key;
    char *value;
    item *next;
};

typedef struct{
    item **table;
    long size;
} hash_map;

/*
必要に応じて大域変数を利用．
大域変数の利用は最小限に抑えること．
大域変数を用いる前に，
本当に大域変数にする必要があるかどうかを考える．
*/
hash_map *map;

//ハッシュ関数
long hash(char *key, long size){
    long h = 0;
    long len = strlen(key);
    for (int i = 0; i < len; ++i) {
        h = h+key[i]*i;
    }
    return 1;
}

//Itemの追加が正常に完了した場合，1を返す
//keyが辞書中に既に存在し，追加しなかった場合は0を返す
int add(char *key, char *value){
    long h = hash(key, map->size);
    item *p = &(map->table)[h];
    while(p->next != NULL){
        printf("%s\n", p->next->key);
        if(strcmp(p->next->key, key) == 0){
            return 0;
        }
        p = p->next;
    }
    item *new_item = malloc(sizeof(item));
    new_item->key = key;
    new_item->value = value;
    new_item->next = NULL;
    if(p == NULL){
        p = new_item;
    }else{
        p->next = new_item;
    }
    return 1;
}

//keyに合致するItemのポインタを返す
//keyが辞書中になければNULLを返す
struct item *check(char *key){
    /*中身を記述．必要に応じて引数も増やす*/
    long h = hash(key, map->size);
    item *p = &(map->table)[h];
    while(p->next != NULL){
        if(strcmp(p->next->key, key) == 0){
            return p->next;
        }
        p = p->next;
    }
    return NULL;
}

//Itemの削除を正常に完了した場合，1を返す．
//keyが辞書中になく，削除しなかった場合は0を返す
int delete(char *key){
    /*中身を記述．必要に応じて引数も増やす*/
    long h = hash(key, map->size);
    item *p = &(map->table)[h];
    while(p->next != NULL){
        if(strcmp(p->next->key, key) == 0){
            item *tmp = p->next;
            p->next = p->next->next;
            free(tmp);
            return 1;
        }
        p = p->next;
    }
    return 0;
}

/*
main関数はこのままでも動作するが，
必要に応じて変更を加えても良い．
ただし，入出力に関わる箇所は
変更を加えないことを推奨する．
（動作確認をパスしなくなる可能性がある．）
*/
int main(int argc, char **argv){
    clock_t t1,t2;
    FILE *fp;

    //initialize
    map = malloc(sizeof(hash_map));
    map->table = malloc(sizeof(item) * TABLE_SIZE);
    map->size = TABLE_SIZE;

    if(argc >= 2){
        if ((fp = fopen(argv[1], "r")) == NULL) {
            printf("file open error!!\n");
            exit(-1);
        }
    }
    else fp = stdin;
    t1 = clock();


    while(1){
        char buf[C_MAX+K_MAX+V_MAX+5];
        char command[C_MAX+1];
        char key[K_MAX+1];
        char value[V_MAX+1];

        printf(">");
        if(fgets(buf,C_MAX+K_MAX+V_MAX+5,fp)==NULL) break;
        sscanf(buf,"%[^,\r\n],%[^,\r\n],%[^\r\n]",command,key,value);
        /*
          上記処理により，xxx,yyy,zzzと入力した場合に
          command,key,valueにそれぞれxxx,yyy,zzzが代入される．
          入力に第三項がない，あるいは，第二・第三項ともにない場合は，
          存在する項のみが代入される．
          たとえば，xxx,yyy（コンマがひとつ）と入力すればcommand,keyにそれぞれxxx,yyyが代入される．
          また，xxx（コンマなし）と入力すればcommandにxxxが代入される．
         */
        if(strcmp(command,ADD)==0){
            if(add(key,value)){
                printf("Added item (%s,%s)\n",key,value);
            }
            else{
                printf("Item associated with \'%s\' already exists\n",key);
            }
        }
        else if(strcmp(command,CHK)==0){
            item *res;
            //辞書を検索し，keyに対応するvalue（のポインタ）をresに保存
            if((res=check(key))){
                printf("Found:(%s:%s)\n",res->key,res->value);
            }
            else{
                printf("No item associated with \'%s\'\n",key);
            }
        }
        else if(strcmp(command,DEL)==0){
            if(delete(key)){
                printf("Deleted item associated with \'%s\'\n",key);
            }
            else{
                printf("No item associated with \'%s\'\n",key);
            }
        }
        else if(strcmp(command,EXIT)==0){
            break;
        }
        else{
            printf("Enter Valid Command\n");
        }
    }

    t2 = clock();
    printf("Done within ### %f sec . ###\n", (double) (t2-t1)/CLOCKS_PER_SEC);

    return 0;
}