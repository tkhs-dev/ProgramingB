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

typedef struct elem_group{
    unsigned int start;
    unsigned char size;
};

typedef struct tree_elem_pref{
    struct elem_group children;
    unsigned int text_start;
};

typedef struct tree_elem_city{
    struct tree_elem_pref *parent;
    struct elem_group children;
    unsigned int text_start;
};

typedef struct tree_elem_town{
    struct tree_elem_city *parent;
    unsigned int text_start;
    char zip_code[ZLEN+1];
};

struct address *address;
int address_size;
int city_num;

struct tree_elem_pref *pref;
struct tree_elem_city *city;
struct tree_elem_town *town;

char *text_pref;
char *text_city;
char *text_town;

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

    char city_tmp[CLEN+1];
    while(fscanf(fp, "%*[^,],%*[^,],\"%[^,^\"]\",%*[^,],%*[^,],%*[^,],\"%[^,^\"]\",%[^,],%[^,],%*s",code,pref,city,town) != EOF ){
        strcpy(address[line].code,code);
        strcpy(address[line].pref,pref);
        memcpy(address[line].city,city+1,strlen(city)-2);
        memcpy(address[line].town,town+1,strlen(town)-2);

        if(strcmp(address[line].city,city_tmp) != 0){
            strcpy(city_tmp,address[line].city);
            city_num++;
        }

        line++;
    }
    printf("load %ld rows of address\n",line);
    fclose(fp);

    address_size = line;
}

void preprocess(){
    unsigned char pref_index = 0;
    unsigned int city_index = 0, town_index = 0;

    unsigned int city_count = 0,town_count = 0;

    pref = (struct tree_elem_pref *)malloc(sizeof(struct tree_elem_pref)*PREF_COUNT);
    city = (struct tree_elem_city *)malloc(sizeof(struct tree_elem_city)*city_num);
    town = (struct tree_elem_town *)malloc(sizeof(struct tree_elem_town)*address_size);

    unsigned long pref_text_cursor = 1,city_text_cursor = 1,town_text_cursor = 1;

    text_pref = (char *)malloc(sizeof(char)*PLEN*PREF_COUNT);
    text_city = (char *)malloc(sizeof(char)*CLEN*city_num);
    text_town = (char *)malloc(sizeof(char)*ALEN*address_size);

    char pref_tmp[PLEN+1], city_tmp[CLEN+1];
    for (int i = 0; i < address_size; ++i) {
        if(strcmp(address[i].pref,pref_tmp) != 0){
            strcpy(pref_tmp,address[i].pref);

            pref[pref_index].children.start = city_index;
            pref[pref_index].children.size = city_count;

            city_count = 1;

            strcpy(text_pref+pref_text_cursor,pref_tmp);
            pref[pref_index].text_start = pref_text_cursor;
            pref_text_cursor += strlen(pref_tmp)+1;
            text_pref[pref_text_cursor-1] = '\0';
            pref_index++;
        }
        if(strcmp(address[i].city,city_tmp) != 0){
            strcpy(city_tmp,address[i].city);

            city[city_index].parent = &pref[pref_index-1];
            city[city_index].children.start = town_index;
            city[city_index].children.size = town_count;

            town_count = 1;

            strcpy(text_city+city_text_cursor,city_tmp);
            city[city_index].text_start = city_text_cursor;
            city_text_cursor += strlen(city_tmp)+1;
            text_city[city_text_cursor-1] = '\0';
            city_index++;
            city_count++;
        }

        town[town_index].parent = &city[city_index-1];
        strcpy(town[town_index].zip_code,address[i].code);
        strcpy(text_town+town_text_cursor,address[i].town);
        town[town_index].text_start = town_text_cursor;
        town_text_cursor += strlen(address[i].town)+1;
        text_town[town_text_cursor-1] = '\0';
        town_index++;
        town_count++;
    }
    realloc(text_pref,pref_text_cursor);
    realloc(text_city,city_text_cursor);
    realloc(text_pref,pref_text_cursor);
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

void dispose(){
    printf("Disposing all allocated resources...\n");
    free(pref);
    free(city);
    free(town);
    free(text_pref);
    free(text_city);
    free(text_town);
}


int main()
{
    init();
    respond();
    dispose();
    return 0;
}