#ifndef FLEXARRAY_H_
#define FLEXARRAY_H_

typedef struct flexarrayrec *flexarray;

/*
 * Function: flexarray_append
 * --------------------------
 * Inserts the specified character string to the next free space in the
 * flexarray. Memory is allocated for the string as needed. Insertion
 * sort is called after the item has been inserted, to keep the items in
 * sorted order. 
 *
 * parameters: flexarray f - the flexarray to append the item to
 *             char * word - the word to append to the flexarray
 *
 * returns: void
 */
extern void      flexarray_append(flexarray f, char *s);

/*
 * Function: flexarray_free
 * ------------------------
 * Frees all memory that was allocated in the creation and manipulation
 * of the flexarray.
 *
 * parameters: flexarray f - the flexarray to be freed
 *             
 * returns: void
 */
extern void      flexarray_free(flexarray f);

/*
 * Function: flexarray_new
 * -----------------------
 * Creates a new flexarray. The default capacity is set to 2,
 * the number of items in the new flexarray is initialised to 0,
 * and memory is allocated for the pointers.
 *
 * parameters: none
 *
 * returns:   flexarray result - the new flexarray
 */
extern flexarray flexarray_new();


/*
 * Function: flexarray_sort
 * ------------------------
 * Sorts the elements in the specified flexarray.
 *
 * parameters: flexarray f - the flexarray to be sorted
 *
 * returns: void
 */
extern void      flexarray_sort(flexarray f);

/*
 * Function: flexarray_visit
 * ---------------
 * Traverses through the flexarray visiting each element, preforming
 * the task specified by the parameter
 *
 * parameters : flexarry f - the flexarray to be traversed
 *              void func(char *s) - the function to perform at each element in the flexarray
 *
 * returns: void
 */
extern void      flexarray_visit(flexarray f, void func(char *s) );

/*
 * Function: flexarray_search
 * --------------------
 * Searches the flexarray for a specified word, using a binary
 * search.
 *
 * parameters: flexarray f - the flexarray to search
 *             char * word - the flexarray is searched for word
 *
 * returns:    int - 1 if found, 0 if not found 
*/
extern int       flexarray_search(flexarray f, char *s);

/*
 * Function: get_itemcount
 * ------------------
 * returns the itemcount of a flexarray 
 * 
 * parameters: flexarray f 
 *
 * returns: void 
 */
extern int       get_itemcount(flexarray f);

#endif
