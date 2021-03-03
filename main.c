#include <stdio.h>
#include <string.h>

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
                printf("X\n");
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


int main() {
    char pole[1000];
    memory_init(pole, 10);
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






