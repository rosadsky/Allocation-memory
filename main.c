#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

    ukazovatel = ptr;

   // printf("MEMORY INIT : %d \n",size);


    memset(ukazovatel,0,size);

  //  ((HEAD *) ukazovatel)->size = 0;
 //   ((HEAD *) ukazovatel)->dalsi = tmp_pointer + sizeof(HEAD);


     ((HEAD *) ukazovatel)->size = size - sizeof(HEAD);
     ((HEAD *) ukazovatel)->dalsi = NULL;
     ((HEAD *) ukazovatel)->obsadeny = 0;




   // printf("HEAD %d , %d\n",ukazovatel,((HEAD *) ukazovatel)->dalsi);
    /*
        printf("Inicializácia pamate\n");
    ukazovatel = ptr;
    HEAD *tmp_pointer = (HEAD *)ukazovatel;


    tmp_pointer->size = size - sizeof(HEAD);
    tmp_pointer->dalsi = NULL;


    HEAD *temp = ukazovatel + sizeof(HEAD);
    temp->dalsi = NULL;
    temp->size = size - 2 * (sizeof(HEAD));
    temp->obsadeny = 0;
    tmp_pointer->dalsi = temp;
     */

}


void *best_fit(unsigned int size) {
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

/*

    while (hladam_moznost->dalsi != NULL) {
        if (hladam_moznost->size >= size) {
            printf("RETURN: %d || ADRESS: %d\n",hladam_moznost->size,hladam_moznost);
            return hladam_moznost;
        }
        hladam_moznost= hladam_moznost->dalsi;
    }

*/

}


void *memory_alloc(unsigned int size) {
    //printf("ALLOC(%d)\n",size + sizeof(HEAD));
    void *allokator = best_fit(size);

    //printf("BEST FIIT: %d\n",allokator);
    void *tmp_free = allokator;

    if (allokator == NULL){
        return NULL;
    }


    ((HEAD*)allokator)->obsadeny = 1; // obsadený

    //[ | payload | x ].

    tmp_free = tmp_free + sizeof(HEAD) + size; // presúvam sa za payload + hlavičku

    ((HEAD*)tmp_free)->size = ((HEAD*)allokator)->size - size - sizeof(HEAD);  //nastavujem veľkosť volného bloku pamate

    ((HEAD*)tmp_free)->obsadeny = 0;

    ((HEAD*)allokator)->size = size; // nastavím volnému bloku size
    //  []   [] –> []
    ((HEAD*)tmp_free)->dalsi = ((HEAD*)allokator)->dalsi; // - spojím dalej dalšej hlavičke vždy davam proste idem dalej
    //  []-> []   []
    ((HEAD*)allokator)->dalsi = tmp_free; // spájam obsadený blok s voľným




    //printf("MALLOC RETURN->(%d)   \n", allokator+sizeof(HEAD));
    return (void*) allokator + sizeof(HEAD) ;


}


int memory_free(void *valid_ptr) {

    HEAD *prehladavac = ukazovatel ;
    //príde mi tam adresa toho payloadu

    HEAD *tester = ukazovatel ;

     void* uvolnovany = valid_ptr - sizeof(HEAD);

     void* tmp_predosly= ukazovatel;
    int i = 0;

    HEAD *tmp1_pointer;
    //printf(" | %d | MEMORY FREE \n",uvolnovany);
    //printf("*-----PRECHDZANIE PAMATE----*\n");
    while (tester->dalsi!=NULL){
      //  printf("ukazovatel (%d) dalsi-> (%d)\n ",tester,tester->dalsi);
        if (tester->obsadeny==0){
            //printf("FREE - %d\n",tester);
        }
        tester =tester->dalsi;
    }




    while (prehladavac->dalsi!=NULL){
       // printf("PREDOSLY - (%d)\n",tmp_predosly);
        //printf("PREHALDAVAC[%d]->[%d]\n ",prehladavac, prehladavac->dalsi);

        if (prehladavac == uvolnovany){
          //  printf("UVOLNUJEM ----------->> %d\n",((HEAD*)uvolnovany));
            ((HEAD*)uvolnovany)->obsadeny = 0;
            HEAD *tmp_dalsi = prehladavac->dalsi;
            if (tmp_dalsi->obsadeny == 0){
                ((HEAD*)uvolnovany)->dalsi = uvolnovany + (2*sizeof(HEAD)) + ((HEAD*)uvolnovany)->size + tmp_dalsi->size;
                ((HEAD*)uvolnovany)->size += sizeof(HEAD) + tmp_dalsi->size;
            //    printf("Uvolnuje blok...\n");
                break;
            }

            //ak je predosly free tak...
            if (((HEAD*)tmp_predosly)->obsadeny == 0 && i >= 1){
                ((HEAD*)tmp_predosly)->dalsi = tmp_predosly + (2*sizeof(HEAD)) + ((HEAD*)tmp_predosly)->size + ((HEAD*)uvolnovany)->size;  // posuniem si pointer dalej
                ((HEAD*)tmp_predosly)->size += sizeof(HEAD) + prehladavac->size;
                //printf("Uvolnujem blok + free pred ním...\n");
                break;
            }




        }
        tmp_predosly = prehladavac;
        prehladavac= prehladavac->dalsi;
        i++;

    }

    tester= ukazovatel;
/*
    printf("*-----PRECHDZANIE PO FREE----*\n");


    while (tester->dalsi!=NULL){
        printf("ukazovatel (%d) dalsi-> (%d)\n ",tester,tester->dalsi);
        if (tester->obsadeny==0){
            printf("FREE - %d - %d\n",tester, tester->size);
        }
        tester =tester->dalsi;
    }
*/



/*
    while (1) {
        if (prehladavac->dalsi == tmp_pointer) { // ak épointer čo prehladava nájde ten môj čo potrebujem free tak

            tmp1_pointer = tmp_pointer->dalsi;


            //Ako fungujú pointre podla mojej hlavy
            //[prehladavac] [tmp_pointer] [ tmp1_pointer]



            //[ FREE ] [ x ] [ ]
            if (prehladavac->obsadeny < 1) { // ak je predošlý blok prázdny
                //[ FREE ] [ ] [ ]
                //   |----------^
                prehladavac->dalsi = tmp_pointer->dalsi;
                return 1;
            }



            //[FULL] [ x ] [ FULL ]
            if (prehladavac->obsadeny == 1 && tmp1_pointer->obsadeny == 1) {
                tmp_pointer->obsadeny = 0;
                return 1;
            }



            //[FULL] [ x ] [    ]
            if (tmp1_pointer->obsadeny < 0) {
                tmp_pointer = tmp_pointer->size + sizeof(HEAD) + tmp1_pointer;
                return 1;
            }
        }
        // ak je prehladavac prazdny
        prehladavac = prehladavac->dalsi;
    }
*/


};


int memory_check(void *ptr){

    HEAD *tmp = ptr;

    HEAD *prehladavac = ukazovatel;

    HEAD *zaciatok = ukazovatel;

    HEAD *koniec = ukazovatel;

    void *tmp_testik;

  //  printf("MEMORY CHECKUJEM: ( %d ) *------------------------------*\n", tmp );

    // ak je pred začiatkom
    // x  |ZACIATOK|    |    |    |   |     |
    if (ptr < zaciatok || ptr == NULL ){
        printf("Pointer je pred začiatkom...\n");
        return 0;
    }

    while (prehladavac->dalsi != NULL){
        tmp_testik = prehladavac; // kvoli pretypovaniu mal som bordel v rozmedzí medzi začiatkom hlavičky a konci

        // ak sa pointer náhodou nachádza v hlavičke  |    |    |[[x]      ]|   |    |

            if (tmp_testik < ptr && tmp_testik+sizeof(HEAD) > ptr){
                printf("Pointer je v hlavičke...\n");
                return 0;
            }

            // ak sa pointer nachádza nie nazačiatku payloadu ale troška dalej  |    |    |[[HEAD]   X    ]|   |    |
            /*
            if (tmp_testik+sizeof(HEAD)<ptr && ptr < tmp_testik+sizeof(HEAD)+((HEAD*)tmp_testik)->size){
                printf("Pointer je v polke payloadu...\n");
                return 0;
            }
*/


        koniec = prehladavac;
        prehladavac= prehladavac->dalsi;
        //ak je pointer za koncom
        //    ||    |    |    |   |     |KONIEC|   X

        if (prehladavac->dalsi == NULL){
            //printf("KONIEC: %d  > %d \n",ptr,koniec);
            if (ptr > koniec+ sizeof(HEAD) + koniec->size){
                printf("Pointer je za koncom...\n");
                return 0;
            }
        }

    }



    // idem robit memory check
    // musím zistiť či náhodou ten pointer, ktorý príde nieje v neolakovanom priestore
    // ak je NULL ruturn 0
    // či nieje pred hlavičkou
    // či nieje mimo celého memory poolu
    // či sa nenachádza v payloade
    // on musí byť nazačiatku payloadu
    // nemože byť na miestach hlavička + velkosť hlavičky
    // ak je v strede payloadu tak rip
    // ak je na začiatku payloadu tak veľkosť hlavičky, ktorá prislúcha payloadu je volna  tak return NULL;

    return 1;
};

int generate_random_number(int l, int r) { //this will generate random number in range l and r
    int i;
    int rand_num = (rand() % (r - l + 1)) + l;
    //printf("%d ", rand_num);
    return rand_num;
}


#include <time.h>

void z1_testovac(char *region, char **pointer, int minBlock, int maxBlock, int minMemory, int maxMemory, int testFragDefrag) {
    srand(time(0));
    unsigned int allocated = 0;
    unsigned int mallocated = 0;
    unsigned int allocated_count = 0;
    unsigned int mallocated_count = 0;
    unsigned int i = 0;
    int random_memory = 0;
    int random = 0;
    memset(region, 0, 100000);
    random_memory = (rand() % (maxMemory-minMemory+1)) + minMemory;
    memory_init(region + 500, random_memory);
    if (testFragDefrag) {
        do {
            pointer[i] = memory_alloc(8);
            if (pointer[i])
                i++;
        } while (pointer[i]);
        for (int j = 0; j < i; j++) {
            if (memory_check(pointer[j])) {
                memory_free(pointer[j]);
            }
            else {
                printf("Error: Wrong memory check.\n");
            }
        }
    }
    i = 0;
    while (allocated <= random_memory-minBlock) {
        random = (rand() % (maxBlock-minBlock+1)) + minBlock;
        if (allocated + random > random_memory)
            continue;
        allocated += random;
        allocated_count++;
        pointer[i] = memory_alloc(random);
        if (pointer[i]) {
            i++;
            mallocated_count++;
            mallocated += random;
        }
    }
    for (int j = 0; j < i; j++) {
        if (memory_check(pointer[j])) {
            memory_free(pointer[j]);
        }
        else {
            printf("Error: Wrong memory check.\n");
        }
    }
    memset(region + 500, 0, random_memory);
    for (int j = 0; j < 100000; j++) {
        if (region[j] != 0) {
            region[j] = 0;
            printf("Error: Modified memory outside the managed region. index: %d\n",j-500);
        }
    }
    float result = ((float)mallocated_count / allocated_count) * 100;
    float result_bytes = ((float)mallocated / allocated) * 100;
    printf("\nMemory size of %d bytes: allocated %.2f%% blocks (%.2f%% bytes).\n", random_memory, result, result_bytes);
}


void roman_test_1(char *region, char **pointer, int minBlock, int maxBlock, int minMemory, int maxMemory){
    memset(region, 0, 100000);


    int lower = minBlock, upper = maxBlock, count = 1;
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

    printf("***** ALOKOVANE BLOKY *****\n");
    while ( alokovana < pamat_random-minBlock) {

        alokacie_random =  generate_random_number(lower, upper);
         //všetko čo sa snažím allokovať može byť usmešne nemusí byť
        pointer[i] = memory_alloc(alokacie_random);

        if (pointer[i] != 0)
            printf("MALLOC [%d]\n",pointer[i] -sizeof(HEAD));

        alokovana += alokacie_random;
        pocet_alokacii++;

        if (pointer[i]) {
            i++;
            mallocovana_pamat++; //
            mallocovana += alokacie_random; // či to prišlo
        }

    }
    printf("*** UVOLNOVANIE BLOKOV ****\n");
    printf("MEMORY FREE [%d]\n",pointer[1]);
    printf("MEMORY FREE [%d]\n",pointer[2]);
    if (memory_check(pointer[1]) && memory_check(pointer[2])) {
        memory_free(pointer[1]);
        memory_free(pointer[2]);
    }



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

/*
    for (int j = 0; j < i; j++) {
        if (memory_check(pointer[j])) {
            memory_free(pointer[j]);
        }
        else {
            printf("Error: Wrong memory check.\n");
        }
    }
*/

    float vysledok = ((float)mallocovana_pamat / pocet_alokacii) * 100;
    float vysledok_bity = ((float)mallocovana / alokovana) * 100;
    printf("*-----------TEST 1-----------*\nVELKOST PAMATE:    %d bitov \nALOKOANYCH BLOKOV: %.2f%%\nPOCET BITOV:       %.2f%%\n", pamat_random, vysledok, vysledok_bity);




}

/*
int main() {
    char region[100000];
    char* pointer[13000];


    z1_testovac(region, pointer, 8, 24, 50, 300, 1);
    //z1_testovac(region, pointer, 8, 1000, 10000, 20000, 0);
    //z1_testovac(region, pointer, 8, 35000, 50000, 99000, 0);

    return 0;
}
*/



int main(){
    char region[100000];
    char* pointer[13000];


    roman_test_1(region, pointer, 8, 24,50,300);
    //roman_test_1(region, pointer, 24, 48,500,1000);
    //roman_test_1(region, pointer, 100, 256,10000,100000);



    return 0;
}

/*
int main() {
    char array[200];
    memory_init(array, 200);

    void *pointer;

    void *blok1;
    void *blok2;
    void *blok3;
    void *blok4;
    void *blok5;


    pointer = ukazovatel;// ukazuje na zaciatok pola

    //VYHRADIT MIESTO AZ POTOM SPAJAT
    blok1 = memory_alloc(10);
    blok2 = memory_alloc(2);
    blok3 = memory_alloc(4);
    blok4 = memory_alloc(7);
    blok5 = memory_alloc(2);
    //blok6 = memory_alloc(12);

    // memory_check(blok2);

      memory_free(blok1);
      memory_free(blok2);
    // memory_free(blok3);
    // memory_free(blok2);
 //  memory_free(blok1);


  //memory_free(blok4);
    memory_free(blok3);
    //memory_free(blok5);
  // memory_check(blok1);


    printf("velkost hlavicky %d\n", sizeof(HEAD));

//vyprintujeme obsah struktur
    while (pointer != NULL){
        if(((HEAD*)pointer)->obsadeny == 0)
            printf("blok free ");
        else
            printf("blok je obsadeny ");

        printf("velkost %d, na zaciatku: %d  na konci: %d\n", ((HEAD*) pointer)->size, pointer, pointer + sizeof(HEAD) + ((HEAD*)pointer)->size);
        pointer = ((HEAD*) pointer)->dalsi;
    }


    return 0;

}

*/


