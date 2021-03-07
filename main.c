#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/*
 *  DSA - MEMORY ALLOCATION PROJECT
 *
 *  Roman Osadsky
 *
 *  @rosadsky
 *
 *  FIIT STU
 *
 */

//globálny ukazovateľ na začiatok pamate
void *ukazovatel;

void *memory_alloc(unsigned int size);

int memory_free(void *valid_ptr);

int memory_check(void *ptr);

typedef struct hlavicka {
    void *dalsi;
    unsigned int size;
    unsigned int obsadeny;
} HEAD;


void memory_init(void *ptr, int size) {
/*
 * Nastavenie veľkosti pamate,
 * Nastavenie prvej hlavičky pamate
 */

    ukazovatel = ptr;

    memset(ukazovatel,0,size);

    ((HEAD *) ukazovatel)->size = size - sizeof(HEAD);
    ((HEAD *) ukazovatel)->dalsi = NULL;
    ((HEAD *) ukazovatel)->obsadeny = 0;

}


void *best_fit(unsigned int size) {

    /*
     * Hladám najlepšie možné miesto v pamati ktoré potom môžem alokovať v memory_alloc funkcií
     *
     */
    HEAD *hladam_moznost = ukazovatel;
    unsigned int najlepsia_moznost = 0;
    HEAD *najlepsi_blok = NULL;

    while (hladam_moznost != NULL) {
        if (hladam_moznost->size >= size+sizeof(HEAD) && hladam_moznost->obsadeny != 1) {
            if (hladam_moznost->size < najlepsia_moznost || najlepsia_moznost == 0) {
                najlepsia_moznost = hladam_moznost->size;
                najlepsi_blok = hladam_moznost;
            }
        }
        hladam_moznost = hladam_moznost->dalsi;
    }

    return najlepsi_blok;

}


void *memory_alloc(unsigned int size) {

    /*
     * Po zavolaní funckie memory alloc sa alloku blok v pamati o veľkosti size
     */

    void *allokator = best_fit(size); // do alokátora sa nám zapíše najlepšia možnosť v mapati
    void *tmp_free = allokator; // pomocná premenná

    if (allokator == NULL){
        return NULL;
    }

    ((HEAD*)allokator)->obsadeny = 1; // zapíšeme do bloku že je už obsadený

    tmp_free = tmp_free + sizeof(HEAD) + size; // presúvam sa za payload + hlavičku

    ((HEAD*)tmp_free)->size = ((HEAD*)allokator)->size - size - sizeof(HEAD);  //nastavujem veľkosť volného bloku pamate

    ((HEAD*)tmp_free)->obsadeny = 0; // obsadenosť volného bloku ktorý som rozdelil nastavujem že je volný

    ((HEAD*)allokator)->size = size; // nastavím volnému bloku size
    //  []   [] –> []
    ((HEAD*)tmp_free)->dalsi = ((HEAD*)allokator)->dalsi; // - spájam nasledovníka voľného vloku s dalším blokom v pamati
    //  []-> []   []
    ((HEAD*)allokator)->dalsi = tmp_free; // spájam obsadený blok s voľným

    return (void*) allokator + sizeof(HEAD);
}


int memory_free(void *valid_ptr) {

    HEAD *prehladavac = ukazovatel ;
    //príde mi tam adresa payloadu

    HEAD *tester = ukazovatel ;

     void* uvolnovany = valid_ptr - sizeof(HEAD);

     void* tmp_predosly= ukazovatel;
     int i = 0;

    HEAD *tmp1_pointer;

    //prechádza sa  celá zatial alokovaná pamať
    /*
     * ak moja pomocná premená ktorou prehládavm pamať narazí na blok ktorý chcem uvolniť nastaví mu obsadenosť že je volny
     *
     * AK -> blok za ukazovateľom je volný tak sa tie dva bloky spoja do jedného
     *
     * AK –> blok pred ukazovatelom ja veľný tak sa tie dva bloky spoja do jedného
     */
    while (prehladavac->dalsi!=NULL){
        if (prehladavac == uvolnovany){
            ((HEAD*)uvolnovany)->obsadeny = 0;
            HEAD *tmp_dalsi = prehladavac->dalsi;
            if (tmp_dalsi->obsadeny == 0){
                ((HEAD*)uvolnovany)->dalsi = uvolnovany + (2*sizeof(HEAD)) + ((HEAD*)uvolnovany)->size + tmp_dalsi->size;
                ((HEAD*)uvolnovany)->size += sizeof(HEAD) + tmp_dalsi->size;
                break;
            }


            if (((HEAD*)tmp_predosly)->obsadeny == 0 && i >= 1){
                ((HEAD*)tmp_predosly)->dalsi = tmp_predosly + (2*sizeof(HEAD)) + ((HEAD*)tmp_predosly)->size + ((HEAD*)uvolnovany)->size;  // posuniem si pointer dalej
                ((HEAD*)tmp_predosly)->size += sizeof(HEAD) + prehladavac->size;
                break;
            }
        }
        tmp_predosly = prehladavac;
        prehladavac= prehladavac->dalsi;
        i++;

    }
    tester= ukazovatel;
};


int memory_check(void *ptr){

    /*
     * memory_check funcia kontrolu či daný pointer ktorý nam prichádza do funkcie je korektný
     *
     * Kontroluje nasledujúce scenáre:
     *
     * 1. Pointer sa nesmie nachádzať za ani pred alokovanou pamaťou
     *
     * 2. Pointer sa nesmie nachádzať v žiadnej hlavičke nijakého bloku
     *
     *
     *
     */

    HEAD *tmp = ptr;

    HEAD *prehladavac = ukazovatel;

    HEAD *zaciatok = ukazovatel;

    HEAD *koniec = ukazovatel;

    void *tmp_testik;



    // x  |ZACIATOK|    |    |    |   |     |
    if (ptr < zaciatok || ptr == NULL ){
        return 0;
    }

    while (prehladavac->dalsi != NULL){
        tmp_testik = prehladavac;

        // ak sa pointer náhodou nachádza v hlavičke  |    |    |[[x]      ]|   |    |
            if (tmp_testik < ptr && tmp_testik+sizeof(HEAD) > ptr){
                return 0;
            }

        koniec = prehladavac;
        prehladavac= prehladavac->dalsi;

        //    ||    |    |    |   |     |KONIEC|   X
        if (prehladavac->dalsi == NULL){
            if (ptr > koniec+ sizeof(HEAD) + koniec->size){
                return 0;
            }
        }

    }

    return 1;
};

int generate_random_number(int l, int r) { //generovanie náhodného čísla
    int rand_num = (rand() % (r - l + 1)) + l;
    return rand_num;
}



void roman_test_1(char *region, char **pointer, int minBlock, int maxBlock, int minMemory, int maxMemory,int spajanie,int vypis){
    memset(region, 0, 100000);
    int lower = minBlock, upper = maxBlock;
    srand(time(0)); //current time as seed of random number generator

    unsigned int velkost_pamate = 0;
    velkost_pamate=600;

    unsigned int alokacie_random = 0;
    unsigned int pamat_random = generate_random_number(minMemory,maxMemory);



    memory_init(region , pamat_random);
    unsigned int mallocovana_pamat = 0;
    unsigned int mallocovana = 0;
    unsigned int memory = 0;
    unsigned int alokovana = 0;
    unsigned int pocet_alokacii = 0;

    int i = 0;
    if(vypis==1){
        printf("***** ALOKOVANE BLOKY *****\n");
    }
    while ( alokovana < pamat_random-minBlock) {

        alokacie_random =  generate_random_number(lower, upper);
         //všetko čo sa snažím allokovať može byť usmešne nemusí byť
        pointer[i] = memory_alloc(alokacie_random);

        if (pointer[i] != 0 && vypis==1)
            printf("MALLOC [%d]\n",pointer[i]-sizeof(HEAD));

        alokovana += alokacie_random;
        pocet_alokacii++;

        if (pointer[i]) {
            i++;
            mallocovana_pamat++; //
            mallocovana += alokacie_random; // či to prišlo
        }

    }
    if(spajanie==1){
        printf("*** UVOLNOVANIE BLOKOV ****\n");
        printf("MEMORY FREE [%d]\n",pointer[1]-sizeof(HEAD));
        printf("MEMORY FREE [%d]\n",pointer[2]-sizeof(HEAD));
        if (memory_check(pointer[1]) && memory_check(pointer[2])) {
            memory_free(pointer[1]);
            memory_free(pointer[2]);
        }
        printf("->[1.UVOLNENY]->[2.UVOLNENY]->\n");
    }

    if(spajanie==2){
        printf("*** UVOLNOVANIE BLOKOV ****\n");
        printf("MEMORY FREE [%d]\n",pointer[2]-sizeof(HEAD));
        printf("MEMORY FREE [%d]\n",pointer[1]-sizeof(HEAD));
        if (memory_check(pointer[1]) && memory_check(pointer[2])) {
            memory_free(pointer[2]);
            memory_free(pointer[1]);
        }
        printf("->[2.UVOLNENY]->[1.UVOLNENY]->\n");
    }

    if(spajanie==3){
        printf("*** UVOLNOVANIE BLOKOV ****\n");
        printf("MEMORY FREE [%d]\n",pointer[1]-sizeof(HEAD));
        printf("MEMORY FREE [%d]\n",pointer[2]-sizeof(HEAD));
        printf("MEMORY FREE [%d]\n",pointer[3]-sizeof(HEAD));
        printf("MEMORY FREE [%d]\n",pointer[4]-sizeof(HEAD));
        if (memory_check(pointer[1]) && memory_check(pointer[2])&&
            memory_check(pointer[1]) && memory_check(pointer[2])) {

            memory_free(pointer[1]);
            memory_free(pointer[2]);
            memory_free(pointer[3]);
            memory_free(pointer[4]);
        }
        printf("->[1.U]->[2.U]->[3.U]->[4.U]->\n");
    }



if(vypis == 1){
    HEAD* tmp1 = ukazovatel;
    printf("****** TEST PAMATE ******** \n",tmp1);
    while (tmp1->dalsi != NULL){
        printf("(%d) ",tmp1,tmp1->dalsi);
        if (tmp1->obsadeny == 0){
            printf(" is FREE");
        }
        printf("\n");
        tmp1 = tmp1->dalsi;
    }

}




    float vysledok = ((float)mallocovana_pamat / pocet_alokacii) * 100;
    float vysledok_bity = ((float)mallocovana / alokovana) * 100;



    int fragmentacia = mallocovana_pamat*(sizeof(HEAD));


    float fragmentacia_percenta = ((float )fragmentacia / (float )pamat_random ) * 100.00;

    printf("*----------- VYSLEDOK TESTU -----------*\n"
           "VELKOST PAMATE:    %d bitov \n"
           "USPEŠNE ALOK.:     %.2f%% pokusov\n"
           "ALOKOVANE BLOKY:   %d%/%d\n"
           "POCET BYTOV:       %.2f%%\n"
           "POCET HLAVICIEK:   %d\n"
           "FRAGMENTACIA:      %d/%d [%.2f%%]\n",
           pamat_random, vysledok,mallocovana,pamat_random,vysledok_bity,
           mallocovana_pamat,fragmentacia,pamat_random
            ,fragmentacia_percenta);
    printf("*----------- KONIEC TESTU ------------*\n");
}

void scenar_1(char *region, char **pointer, int minBlock, int maxBlock, int minMemory, int maxMemory,int isRandom){
    memset(region, 0, 100000);
    int lower = minBlock, upper = maxBlock;
    srand(time(0)); //current time as seed of random number generator

    unsigned int velkost_pamate = 0;
    velkost_pamate=600;
    unsigned int pamat_random;
    unsigned int alokacie_random = 0;
    if (isRandom == 1){
        pamat_random = generate_random_number(minMemory,maxMemory);
    } else{
        pamat_random = maxMemory;
    }



    memory_init(region , pamat_random);
    unsigned int mallocovana_pamat = 0;
    unsigned int mallocovana = 0;
    unsigned int malocovane_byty = 0;
    unsigned int alokovana = 0;
    unsigned int pocet_alokacii = 0;


    int i = 0;

    printf("***** ALOKOVANE BLOKY *****\n");
    while (alokovana<pamat_random-minBlock+1) {
        if (isRandom == 1 ){
            alokacie_random =  generate_random_number(lower, upper);
        } else {
            alokacie_random =  maxBlock;
        }
        //všetko čo sa snažím allokovať može byť usmešne nemusí byť
        pointer[i] = memory_alloc(alokacie_random);


        if (pointer[i] != 0)
            printf("MALLOC [%d]\n",pointer[i]-sizeof(HEAD));

        alokovana += alokacie_random;
        pocet_alokacii++;

        if (pointer[i]) {
            i++;
            mallocovana_pamat++;
            mallocovana += alokacie_random; // či to prišlo
        }

    }
    HEAD* tmp1 = ukazovatel;

    float vysledok = ((float)mallocovana_pamat/ pocet_alokacii) * 100;

    float vysledok_bity = ((float)mallocovana / (float)pamat_random) * 100;

    int fragmentacia = mallocovana_pamat*(sizeof(HEAD));

    float fragmentacia_percenta = ((float )fragmentacia / (float )pamat_random ) * 100.00;

    printf("*----------- VYSLEDOK TESTU -----------*\n"
           "VELKOST PAMATE:    %d bitov \n"
           "USPEŠNE ALOK.:     %.2f%% pokusov\n"
           "ALOKOVANE BLOKY:   %d%/%d\n"
           "POCET BYTOV:       %.2f%%\n"
           "POCET HLAVICIEK:   %d\n"
           "FRAGMENTACIA:      %d/%d [%.2f%%]\n",
           pamat_random, vysledok,mallocovana,pamat_random,vysledok_bity,
           mallocovana_pamat,fragmentacia,pamat_random
            ,fragmentacia_percenta);
    printf("*----------- KONIEC TESTU ------------*\n");
}



int main(){
    char region[100000];
    char* pointer[13000];

    //SPAJANIE 0- bez memory free
    //         1- spajanie [VOLNY][UVOLNOVANY]
    //         2- spajanie [UVOLNOVANY][VOLNY]
    //         3- spajanie [1.UVOLNOVANY][2.UVOLNOVANY][3.UVOLNOVANY][4.UVOLNOVANY]
    //VYPIS 1- ANO

    scenar_1(region, pointer, 8, 8,24,50,0);
    scenar_1(region, pointer, 8, 16,24,100,0);
    scenar_1(region, pointer, 8, 24,24,200,0);

    scenar_1(region, pointer, 8, 24,24,50,1);
    scenar_1(region, pointer, 8, 24,24,100,1);
    scenar_1(region, pointer, 8, 24,24,200,1);


    scenar_1(region, pointer, 500, 5000,1000,30000,0);
    scenar_1(region, pointer, 8, 50000,60000,100000,0);

    printf("\n///////////// MOJE TESTY ////////////////\n\n");
    roman_test_1(region, pointer, 8, 24,150,500,1,1);
    roman_test_1(region, pointer, 8, 24,150,500,2,1);
    roman_test_1(region, pointer, 8, 24,300,1000,3,1);




    return 0;
}

/*
int main() {
    char region[100000];
    char* pointer[13000];


    z1_testovac(region, pointer, 8, 24, 50, 300, 1);
    z1_testovac(region, pointer, 8, 1000, 10000, 20000, 0);
    z1_testovac(region, pointer, 8, 35000, 50000, 99000, 0);
    scenar_2(region, pointer, 500, 5000,1000,30000);
    return 0;
}
*/
