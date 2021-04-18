# Správca pamäti

### Zadanie 1 | Predmet:  Dátove štruktúry a algoritmy

#### Implementácia v C štyroch funkcií:

```c
 void *memory_alloc(unsigned int size);
```
-  poskytuje služby analogické štandardnému malloc.
```c
 int memory_free(void *valid_ptr);
```
-  slúži na uvoľnenie vyhradeného bloku pamäti, podobne ako funkcia free.
```c
 int memory_check(void *ptr);
```
-  slúži na skontrolovanie, či parameter (ukazovateľ) je platný ukazovateľ, ktorý bol v nejakom z predchádzajúcich volaní vrátený funkciou memory_alloc a zatiaľ nebol uvoľnený funkciou memory_free. Funkcia vráti 0, ak je ukazovateľ neplatný, inak vráti 1.
```c
 void memory_init(void *ptr, unsigned int size);
```
- slúži na inicializáciu spravovanej voľnej pamäte. Funkcia sa volá práve raz pred všetkými inými volaniami memory_alloc, memory_free a memory_check.
