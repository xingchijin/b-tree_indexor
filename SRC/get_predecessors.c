/**
 * get_predecessor
 * print out all the k successes
 * @param key
 * @param k: number of successors
 * @param result: result where store predecessors
 * return the number of predecessors that have been found.
 */

#include <stdio.h>
#include <string.h>
#include "def.h"

extern int CompareKeys(char *Key, char *Word);
extern struct PageHdr *FetchPage(PAGENO Page);
extern int FreePage(struct PageHdr *PagePtr);
extern int iscommon(char *word);
extern int check_word(char *word);
extern int strtolow(char *s);

static int exist=0;


typedef struct Stack{
	PAGENO* s;
	int top;
	int stack_size;
}Stack;


Stack* init_stack(int size);
PAGENO* pop(Stack* s);
void push(Stack* s, PAGENO ele);
void free_stack(Stack* s);
int dfs(char *key, char* result[],PAGENO pgnum,int need, int k);

int get_predecessors(char *key, int k, char *result[]) {

	int findNum=0;
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

	findNum=dfs(key,result,ROOT,k,k);
	if(exist==0) return 0;
	return findNum;
}

int dfs(char *key, char* result[],PAGENO pgnum,int need, int k){
	if(need<=0) return 0;

	struct PageHdr *PagePtr=FetchPage(pgnum);
	char *Word; /* Key stored in B-Tree */
	struct KeyRecord* KeyListTraverser=PagePtr->KeyListPtr;
	/* put keyrecord into stack*/

	int cmp_result;
	int NumKeys=PagePtr->NumKeys;
	int count=0;


	/*stack is only used for nonleaf pages*/
	Stack* stack;

	if (IsLeaf(PagePtr)){
		while (KeyListTraverser != NULL) {
			        if(CompareKeys(KeyListTraverser->StoredKey,key)==0){
			        	exist=1;
			        	break;
			        }
			        /*write this string into result. start from an empty place and, if get more string than needed, replace the oldest one*/
			        strcpy(result[count%need+k-need],KeyListTraverser->StoredKey);
			        count++;

			        KeyListTraverser = KeyListTraverser->Next;
		}

		FreePage(PagePtr);

        if( exist==0) return k;

		if(count>need) return need;
		else return count;

	}else{
		/*Use stack to do backtrack and keep searching predecessor leaf pages*/
		stack=init_stack(NumKeys+1);

		while( NumKeys>0){
			Word=KeyListTraverser->StoredKey;
			(*(Word + KeyListTraverser->KeyLen)) = '\0';

			cmp_result=CompareKeys(key, Word);
			NumKeys--;

			if(cmp_result<2){

				/*put PAGENO into stack*/
				push(stack,KeyListTraverser->PgNum);

				break;
			}

			/* put PAGENO into stack*/
			push(stack,KeyListTraverser->PgNum);

			KeyListTraverser = KeyListTraverser->Next;

		}

		if(cmp_result==2 && NumKeys==0){

			push(stack,PagePtr->PtrToFinalRtgPg);

		}

		while(stack->top>0){
			PAGENO tovisit= *(pop(stack));

			count+= dfs(key,result,tovisit,need-count,k);


			if(count>=need) return need;
		}
		free_stack(stack);
		FreePage(PagePtr);

		return count;
	}

}

/*
 * Functions to implement a stack
 */


Stack* init_stack(int size){
	if(size<=0) return NULL;
	Stack * s=(Stack*)malloc(sizeof(Stack));
	s->stack_size=size;
	s->top=0;
	s->s=(PAGENO*)malloc(sizeof(PAGENO)*size);
	return s;
}

PAGENO* pop(Stack* s){
	if(s->top<=0) return NULL;
	else {
		s->top--;

		return s->s+s->top;
	}
}

void push(Stack* s, PAGENO ele){
	if(s->top>=s->stack_size) {printf("Stack is full!\n");return;}
	else{
		*(s->s+s->top)=ele;

		s->top++;
	}
	return;
}

void free_stack(Stack* s){
	free(s->s);
	free(s);
}


