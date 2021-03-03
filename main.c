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
    unsigned int  obsadeny;
} HEAD;

void memory_init(void *ptr, int size) {
    ukazovatel = ptr;
    ((HEAD*)ukazovatel)->size = size - sizeof(HEAD);
    ((HEAD*)ukazovatel)->dalsi = ukazovatel + sizeof(HEAD);

    HEAD *temp = ((HEAD*)ukazovatel)->dalsi;
    temp->dalsi = NULL;
    temp->size = size - 2*(sizeof(HEAD));
    temp->obsadeny = 0;

}


void *best_fit(unsigned int size) {
    HEAD *hladam_moznost = ((HEAD *) ukazovatel)->dalsi;
     unsigned int najlepsia_moznost = 0;
     HEAD *najlepsi_blok = NULL;



    while (hladam_moznost != NULL) {
        if (hladam_moznost->size >= size && hladam_moznost->obsadeny != 1) {
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
        hladam_moznost->dalsi;
    }
    */


}

//'''''''''''''''//
//              //
// ______________________________________________________________________
// |              |                  |                 |                 |
// |FUCK MALLOC   |                  |                 |                 |
// '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//                                   //
//-----------------------------------//


void *memory_alloc(unsigned int size) {

      HEAD *ptr = best_fit(size);

      HEAD *tmp_pointer = ptr;

      // ošetriť ak sa mi vráti null

      ptr->obsadeny = 1; // obsadený

     tmp_pointer = tmp_pointer + sizeof(HEAD) + size; // presúvam sa za payload + hlavičku

     tmp_pointer->size= ptr->size - size; //nastavujem veľkosť volného bloku pamate

     ptr->size = size; // nastavím volnému bloku size
     //  []   [] –> []
     tmp_pointer->dalsi = ptr->dalsi; // - spojím dalej dalšej hlavičke vždy davam proste idem dalej
     //  []-> []   []
     ptr->dalsi = tmp_pointer; // posúvam sa dalej prifávam ešte komentár

     return tmp_pointer + sizeof(HEAD);


}

int memory_free(void *valid_ptr){

    HEAD * prehladavac =ukazovatel;
    //príde mi tam adresa toho payloadu

    HEAD *tmp_pointer = valid_ptr;

    while (1){
        if (prehladavac->dalsi == tmp_pointer){ // ak pointer čo prehladava nájde ten môj čo potrebujem free tak
            if(prehladavac->obsadeny < 1){ // ak je predošlý blok prázdny
           //[ FREE ] [ ] [ ]
           //   |----------^
            prehladavac->dalsi = tmp_pointer->dalsi;
            }

            if(prehladavac->obsadeny == 1){
                tmp_pointer->obsadeny = 0;
            }
        }
        // ak je prehladavac prazdny





        break;
    }










};

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

    memset(region + 500, 0, random_memory);
    for (int j = 0; j < 100000; j++) {
        if (region[j] != 0) {
            region[j] = 0;
            printf("Error: Modified memory outside the managed region. index: %d\n",j-500);
        }
    }
    float result = ((float)mallocated_count / allocated_count) * 100;
    float result_bytes = ((float)mallocated / allocated) * 100;
    printf("Memory size of %d bytes: allocated %.2f%% blocks (%.2f%% bytes).\n", random_memory, result, result_bytes);
}

int main() {
    char region[100000];
    char* pointer[13000];
    z1_testovac(region, pointer, 8, 24, 50, 100, 1);
    z1_testovac(region, pointer, 8, 1000, 10000, 20000, 0);
    z1_testovac(region, pointer, 8, 35000, 50000, 99000, 0);
    return 0;
}
/*
int main() {
    char pole[1000];
    //memory_init(pole, 10);
    void *ptr = ukazovatel;
    //memory_init(pole, 10);

    memory_alloc(10);
    memset(ptr, 0, 1000);


    while (ptr != NULL) {
        printf("Velkost %d na bloku %p\n", ((HEAD *) ptr)->size, ptr);
        ptr = ((HEAD *) ptr)->dalsi;
    }


    return 0;

}




*/

