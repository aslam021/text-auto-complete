/*  ASLAM M.M.M
    E/15/021
    CO322 Lab4  */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define ALPHABET_SIZE 26
#define MAX_WORD_SIZE 20

typedef struct trienode{
    struct trienode *children[ALPHABET_SIZE]; 
    bool isLeaf;
    bool isWordEnd;
}TrieNode;

TrieNode* createNode(){
    TrieNode *pNode = NULL;

    //allocate memory for new node
    pNode = (TrieNode *)malloc(sizeof(TrieNode)); 

    //set values for new node
    pNode->isLeaf = false;
    pNode->isWordEnd = false;
    for (int i = 0; i < ALPHABET_SIZE; i++) 
        pNode->children[i] = NULL;

    return pNode;
}

TrieNode* insertWord(char *str, TrieNode *head){

    // start from root node
	TrieNode* curr = head;

    //go through each character  and exit if character is a newline
	while (*str){
		// create a new node if path doesn't exists 
		if (curr->children[*str - 'a'] == NULL ){
            curr->children[*str - 'a'] = createNode();
            curr->isLeaf=false;
        }

        // go to next node
        curr = curr->children[*str - 'a'];

		// move to next character
		str++;
	}

    // mark current node as leaf and end of word
	curr->isLeaf = true;
    curr->isWordEnd=true;
}

void printAll(TrieNode *head, char c[]){
    TrieNode *currentNode;

    //go through all the existing child nodes
    for (int i = 0; i < ALPHABET_SIZE; i++){
        currentNode = head;

        if(currentNode->children[i]!=NULL){ //if child exist (character exist)
            currentNode = currentNode->children[i];
            
            //add new nodes's character to form a suggestion
            char temp[MAX_WORD_SIZE];
            memset(temp, '\0', sizeof(temp));
            strcpy(temp, c);
            temp[strlen(temp)] = 'a'+i;
            
            //if current node is not leaf, go through the branches of it
            if(!currentNode->isLeaf){
                printAll(currentNode, temp);
            }
            
            if(currentNode->isWordEnd) printf("%s\n", temp);
        }
    }
    return;
}

int printSuggestions(TrieNode *head, char *word){
    char str [MAX_WORD_SIZE];
    memset(str, '\0', sizeof(str));
    strcpy(str,word);

    if (head==NULL){
        return 0;
    }

    TrieNode *currentNode=head;
    while (*word){
        // go to next node
        currentNode=currentNode->children[*word-'a'];

        // if string is invalid (reached end of path in Trie)
        if(currentNode==NULL){
            return 0;
        }

        // move to next character
        word++;
    }
    
    //get balance characters from trie and print suggestions
    printAll(currentNode, str);
    return 1;
}

int main(int argc, char *argv[]){
    
    //exit program when enough arguments are not given
    if (argc!=2) {
        printf("usage: ./textAutoComplete filename.txt\n");
        return 0;
    }

    FILE *file;
    file = fopen(argv[1], "r"); //open the file for reading

    if (!file) {
        printf("Cannot open file %s\n", argv[1]);
        return 0; //exit if there is no such file
    }

    TrieNode *trie =  createNode(); //create root node

    //insert all words into the trie data structure
    char str[MAX_WORD_SIZE];
    int c;
    char ch[MAX_WORD_SIZE]; //for the purpose of filtering

    clock_t start, end;
    double cpu_time_used;
    
    start = clock();
    while (fgets(str, MAX_WORD_SIZE, file) != NULL){
        //remove some characters 
        c=0;
        memset(ch, '\0', sizeof(ch));
        for (int i = 0; i < strlen(str); i++)
        {
            if(str[i]>='[' && str[i]<='~'){
                ch[c]=str[i];
                c++;
            } else if (str[i]>='A' && str[i]<='Z'){
                ch[c]=tolower(str[i]);
                c++;
            }
        }
        
        //insert filtered word into trie DS
        insertWord(ch, trie);
    }
    end =clock();
    cpu_time_used = ((double)(end-start)) / CLOCKS_PER_SEC;
    printf("Time taken to store the dictionary: %f seconds\n", cpu_time_used);

    fclose(file); //close the file after all words updated into the trie
    
    while (1){
        //get input
        printf("-------------------------------------------------------");
        printf("\n\nEnter first n characters of a word : ");
        memset(str, '\0', sizeof(str));
        fgets(str, MAX_WORD_SIZE, stdin);
        
        //remove some characters
        c=0;
        memset(ch, '\0', sizeof(ch));
        for (int i = 0; i < strlen(str); i++){
            if(str[i]>='[' && str[i]<='~'){
                ch[c]=str[i];
                c++;
            } else if (str[i]>='A' && str[i]<='Z'){
                ch[c]=tolower(str[i]);
                c++;
            }
        }

        //if there is no word exit the loop
        if(strlen(ch)==0) break;

        //search for suggestions
        start = clock();
        if(!printSuggestions(trie, ch)){
            printf("No suggestions found\n");
        }
        end = clock();
        cpu_time_used = ((double)(end-start)) / CLOCKS_PER_SEC;
        printf("\nTime taken to print above suggestions: %f seconds\n", cpu_time_used);
    }

    return 0;
}