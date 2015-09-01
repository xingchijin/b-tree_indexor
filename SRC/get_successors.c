/**
 * get_successors
 * print out all the k successes
 * @param key
 * @param k: number of successors
 * @param result: result where store successors
 * return the number of successors that have been found.
 */

#include "def.h"

extern int iscommon(char *word);
extern int check_word(char *word);
extern int strtolow(char *s);
extern int CompareKeys(char *Key, char *Word);
extern PAGENO treesearch_page(PAGENO PageNo, char *key);
extern int FreePage(struct PageHdr *PagePtr);
extern struct PageHdr *FetchPage(PAGENO Page);


int get_successors(char *key, int k, char *result[]) {

	/* Print an error message if strlen(key) > MAXWORDSIZE */
	    if (strlen(key) > MAXWORDSIZE) {
	        printf("ERROR in \"search\":  Length of key Exceeds Maximum Allowed\n");
	        printf(" and key May Be Truncated\n");
	    }
	    if (iscommon(key)) {
	        printf("\"%s\" is a common word - no searching is done\n", key);
	        return -1;
	    }
	    if (check_word(key) == FALSE) {
	        return -1;
	    }
	    /* turn to lower case, for uniformity */
	    strtolow(key);


	const PAGENO page = treesearch_page(ROOT, key);
	/* from page number we traverse the leaf page */
    struct PageHdr *PagePtr = FetchPage(page);

	int count=0;
    int find=0;

	struct KeyRecord *keyptr;


	  /* traverse all keys in this page where the key resides if the key exists*/
	    keyptr = PagePtr->KeyListPtr;
	    while (keyptr != NULL) {

	    	if(find==1 && count<k){ strcpy(result[count++],keyptr->StoredKey);}
	        if(CompareKeys(keyptr->StoredKey,key)==0){
	        	find=1;
	        }
	        if(count==k){ FreePage(PagePtr);return k;}
	        keyptr = keyptr->Next;
	    }
	 /*if the key does not exist in the tree, just return 0*/
	    if(find==0) return 0;

    while ((PagePtr->PgNumOfNxtLfPg) != NULLPAGENO){ /* get next valid page */

    	 struct PageHdr * tempp = PagePtr;
    	 PagePtr = FetchPage(PagePtr->PgNumOfNxtLfPg);
    	 FreePage(tempp);


    	 keyptr = PagePtr->KeyListPtr;
    	 while (keyptr != NULL) {
    		 /*if the key has been found but the k successors are not all found.*/
    		 if(find==1 && count<k) {strcpy(result[count++],keyptr->StoredKey);}
    		 if(count==k){ FreePage(PagePtr);return k;}
    	 	  keyptr = keyptr->Next;
    	 }

    }
    FreePage(PagePtr);

    printf("\n\n");
    return count;
}
