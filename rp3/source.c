#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <time.h>

#define ADVANCED 0 //発展課題（絞り込み検索）に対応する場合は1に変更

#define DATAFILE "data_sjis.csv" //data_utf.csvかdata_sjis.csvに変更
#define PREF_COUNT 47 //都道府県数
#define ZLEN 7 //郵便番号の最大バイト長
#define PLEN 8 //都道府県名の最大バイト長
#define CLEN 200 //市町村名の最大バイト長
#define ALEN 200 //町域の最大バイト長
#define MAX_SIZE 200000//住所録中の住所数の最大数

int mode; //検索モード 0:なし，1:郵便番号検索，2:文字列検索
int refine_flag; //絞り込み検索の有無 0:なし，1:あり
char query[ALEN]; //検索クエリ（郵便番号or文字列）

typedef struct address{
    char code[ZLEN+1]; //郵便番号
    char pref[PLEN+1]; //都道府県
    char city[CLEN+3]; //市町村
    char town[ALEN+3]; //町域
};

struct address *address;
int adress_size;

//住所データファイルを読み取り，配列に保存
void scan(){
    FILE *fp;
    char code[ZLEN + 1],pref[PLEN + 1],city[CLEN + 1],town[ALEN + 1];
    long line=0;

    //datasizeの計算
    if ((fp = fopen(DATAFILE, "r")) == NULL) {
        fprintf(stderr,"error:cannot read %s\n",DATAFILE);
        exit(-1);
    }
    address = (struct address *)malloc(sizeof(struct address)*MAX_SIZE);

    while(fscanf(fp, "%*[^,],%*[^,],\"%[^,^\"]\",%*[^,],%*[^,],%*[^,],\"%[^,^\"]\",%[^,],%[^,],%*s",code,pref,city,town) != EOF ){
        /*
          上のfscanfにより，code,pref,city,townにそれぞれ郵便番号，都道府県，市町村，町域を表す
          文字列が記憶される．この箇所にコードを加筆し，
    　　　これらの情報を用いて構造体の配列に住所データを記憶させる．
         */
        strcpy(address[line].code,code);
        strcpy(address[line].pref,pref);
        memcpy(address[line].city,city+1,strlen(city)-2);
        memcpy(address[line].town,town+1,strlen(town)-2);

        line++;
    }
    printf("load %ld rows of address\n",line);
    fclose(fp);

    adress_size = line;
}

void preprocess(){
    free(address);
    return;
}

double diff_time(clock_t t1, clock_t t2){
    return (double) (t2-t1)/CLOCKS_PER_SEC;
}

//初期化処理
void init(){
    clock_t t1,t2;

    t1 = clock();
    scan();
    preprocess();
    printf("Done initilization\n");
    t2 = clock();
    printf("\n### %f sec for initialization. ###\n",diff_time(t1,t2));
}

//郵便番号による住所検索．検索結果を出力．
void code_search(){
    return;
}

//文字列による住所検索．検索結果を出力．
void address_search(){
    return;
}

//絞り込み検索の実施
void refinement(){
    return;
}

void input(){
    printf("\n"
           "#########Top Menu#########\n"
           "# Search by postal code: 1\n"
           "# Search by address    : 2\n"
           "# Exit                 : 0\n"
           "> ");
    scanf("%d", &mode);
    if(mode == 1){
        printf("Postal code > ");
        scanf("%s", query);
    }else if(mode == 2){
        printf("Search String > ");
        scanf("%s", query);
    }
}

//絞り込み検索の有無を確認
void re_input(){
    printf("\n"
           "# Continue Searching: 1\n"
           "# Return to Top Menu: 0\n"
           "> ");
    scanf("%d", &refine_flag);
    if(refine_flag == 1){
        printf("String for Refinement> ");
        scanf("%s", query);
    }
    return;
}

//クエリへの応答
void respond(){
    clock_t t1,t2;
    mode = 1;
    while(1){
        input();
        if(mode == 1){
            t1 = clock();
            code_search();
            t2 = clock();
            printf("\n### %f sec for search. ###\n", diff_time(t1,t2));
        }
        else if(mode == 2){
            t1 = clock();
            address_search();
            t2 = clock();
            printf("\n### %f sec for search. ###\n", diff_time(t1,t2));
            if(!ADVANCED) continue;
            while(1){
                re_input();
                if(refine_flag == 0) break;
                t1 = clock();
                refinement();
                t2 = clock();
                printf("\n### %f sec for search. ###\n", diff_time(t1,t2));
            }
        }
        else break;
    }
}


int main()
{
    init();
    respond();
    return 0;
}