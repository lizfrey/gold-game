/*
 * set.c - implments a set of void* with char* keys
 *
 * Elizabeth Frey
 * Winter 2021
 * CS 50 Lab 3
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "set.h"

/********** file-local global variables **********/
/* none */

/********** local types **********/
typedef struct setnode {
    char* key;
    void *item; //pointer to data for this item
    struct setnode *next; //link to next node
} setnode_t;

/********** global types **********/
typedef struct set {
    struct setnode *head; // head of list of items in set
} set_t;

/********** global functions **********/
/* see set.h for comments about exported functions */

/********** local functions  **********/
static setnode_t *setnode_new(const char* key, void *item);

/********** set_new() **********/
/* see set.h for description*/
set_t *
set_new(void)
{
    set_t *set = (set_t*)malloc(sizeof(set_t));
    if(set == NULL){
        //if there was an error allocating memory
        //if so, free the set
        free(set);
        return NULL;
    }
    else{
        //set is empty
        set->head = NULL;
        return set;
    }
}


/********** setnode_new **********/
/* allocates and initializes a setnode */
static setnode_t *
setnode_new(const char *key, void* item)
{
    //allocate memory for node
    setnode_t *node = (setnode_t*)malloc(sizeof(setnode_t));

    if (node == NULL){
        //if there was an error allocating memory
        return NULL;
    }
    else{
        //make a copy of the string so that the caller can 
        //reuse or deallocate string
        node->key = (char*)malloc(strlen(key)+1);
        //check for error allocating key
        if(node->key == NULL){
            //if error, free the node
            free(node);
            return NULL;
        }
        strcpy(node->key, key);
        node->next = NULL;
        node->item = item;
        return node;
    }
}

/********** set_insert() **********/
/* see set.h for description*/
bool
set_insert(set_t *set, const char *key, void *item)
{
    if(set == NULL || key == NULL || item == NULL){
        return NULL;
    }
    else{
        //check if key exists
        for(setnode_t *node = set->head; node != NULL; node = node->next){
            if(strcmp(node->key, key) == 0){
                return false;
            }
        }
        //if not, add to the listf
        setnode_t *new = setnode_new(key,item);
        //check if there was error making node
        if(new == NULL){
            return false;
        }
        new->next = set->head;
        set->head = new;
    }
    return true;
}

/********** set_find() **********/
/* see set.h for description*/
void
*set_find(set_t *set, const char *key)
{
    if(set == NULL){
        return NULL;
    }
    else if(key == NULL){
        return NULL;
    }
    else{
        for(setnode_t *node = set->head; node != NULL; node = node-> next){
            //check for matching key
            if(strcmp(node->key, key) == 0){
                return node->item;
            }
        }
    }
    //key not found
    return NULL;
}

/********** set_print() **********/
/* see set.h for description*/
void
set_print(set_t *set, FILE *fp,
        void(*itemprint)(FILE *fp, const char* key, void *item) )
{
    if(fp != NULL){
        if(set != NULL){
            fputc('{',fp);
            for(setnode_t *node = set->head; node != NULL; node = node->next){
                if(itemprint != NULL){
                    (*itemprint)(fp, node->key, node->item);
                    if(node->next != NULL){ //if isn't the last node
                        fputc(',',fp);
                    }
                }
            }
            fputc('}',fp);
        }
        else{
            //if set is null
            fputs("(null)",fp);
        }
    }
}
/********** set_iterate() **********/
/* see set.h for description*/
void
set_iterate(set_t *set, void *arg,
        void(*itemfunc)(void *arg, const char *key, void *item) )
{
    if(set != NULL && itemfunc != NULL){
        //call item func with arg and key on every item
        for(setnode_t *node = set->head; node != NULL; node = node->next){
            (*itemfunc)(arg, node->key, node->item);
        }
    }
}
/********** set_delete() **********/
/* see set.h for description*/
void
set_delete(set_t *set, void (*itemdelete)(void *item) )
{
    if (set != NULL){
        for(setnode_t *node = set->head; node!=NULL;){
            if(itemdelete != NULL){        //if given
                (*itemdelete)(node->item);
            }
            setnode_t *next = node->next;  //remember next
            free(node->key);         //free all keys
            free(node);              //free the node
            node = next;
        }
        free(set);                   //free the set
    }
}



/* *********************************************************** */
/* Unit test: glass-box test of the set module.
 * based off of code form bag.c
 * Create a set, work with it, and verify its innards.
 */

#ifdef UNIT_TEST
#include <stdbool.h>

// file-local global variables
static int unit_tested = 0;     // number of test cases run
static int unit_failed = 0;     // number of test cases failed

#define EXPECT(cond) {unit_expect((cond), __LINE__);}
//from bag.c
void unit_expect(bool condition, int linenum){
    unit_tested++;
    if(condition){
        printf("PASS test %03d at line %d\n", unit_tested, linenum);
    }
    else{
        printf("FAIL test %03d at line %d\n", unit_tested, linenum);
        unit_failed++;
    }
}

/****** test item print ******/
/*
 * a random test itemprint used in the unittest
 */
static void 
testitemprint(FILE *fp, const char *key, void *item){
    fprintf(fp, "(%s -> %d)", key, (*(int*)item));
}

/****** test item func ******/
/*
 * a random test itemfunc used in the unittest
 */
static void
testitemfunc(void *arg, const char *key, void *item){
    (*((int*)item))++;
}

/****** maiin ******/
/*
 * main for unit test
 */
int 
main(){
    //make a new set
    set_t *set = set_new();
    EXPECT(set != NULL);
    EXPECT(set->head == NULL);

    //put things into the set, see if worked
    int a=1, b=2, c=3;
    const char *key1 = "item one";
    const char *key2 = "item two";
    const char *key3 = "item three";
    EXPECT(set_insert(set, key1, &a));
    EXPECT(set_insert(set, key2, &b));
    EXPECT(set_insert(set, key3, &c));
    EXPECT(set_insert(set, key3, &c) == false);

    //check the set to see if pointers are correct
    EXPECT(set->head->item == &c);
    EXPECT(set->head->next->next->item == &a);

    //test set_find
    EXPECT(set_find(set, "item one") == &a);
    EXPECT(set_find(set, "non-existent item") == NULL);
    EXPECT(set_find(NULL, "item one") == NULL);
    EXPECT(set_find(set, NULL) == NULL);

    //test printing to a file: output.txt
    FILE *fpwrite = fopen("output.txt", "w");
    fprintf(fpwrite, "test print one:");
    set_print(set, fpwrite, testitemprint);
    set_print(set, fpwrite, NULL);

    void *arg = NULL;
    //modify the stuff in the set with function that adds 1
    set_iterate(set, arg, testitemfunc);
    fprintf(fpwrite, "\ntest print two (after set_iterate):");
    EXPECT(*((int*)(set->head->item)) == (4));
    EXPECT(*((int*)(set->head->next->item)) == (3));
    set_print(set, fpwrite, testitemprint);

    //delete the set
    set_delete(set, NULL);
    fclose(fpwrite);

    //print a summary of results
    //from bag.c
    if(unit_failed >0){
        printf("FAILED %d of %d tests\n", unit_failed, unit_tested);
        return unit_failed;
    }
    else{
        printf("PASSED all of %d tests\n", unit_tested);
        return 0;
    }
}
#endif
