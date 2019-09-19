/*  ASLAM M.M.M
    E/15/021
    CO322 Lab4  */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ALPHABET_SIZE 26
#define MAX_WORD_SIZE 20

typedef struct trienode{
    struct trienode *children[ALPHABET_SIZE]; 
    char str[MAX_WORD_SIZE];
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
    pNode->str;
    for (int i = 0; i < ALPHABET_SIZE; i++) 
        pNode->children[i] = NULL;

    return pNode;
}

TrieNode* insertWord(char str [], TrieNode *head){
    
    TrieNode *currentNode = head;
    
    //there is no word start with the first letter of incoming string
    if(currentNode->children[str[0]-'a']==NULL){
        currentNode->isLeaf = false; //set current node as not leaf

        currentNode->children[str[0]-'a'] = createNode();
        currentNode = currentNode->children[str[0]-'a']; //go to new node
        
        //set values for new node
        currentNode->isLeaf = true;
        currentNode->isWordEnd = true;
        memset(currentNode->str, '\0', sizeof(currentNode->str));
        strcpy(currentNode->str, str);

    } else {
        currentNode = currentNode->children[str[0]-'a']; //go to next node
        char temp [MAX_WORD_SIZE]; //copy the string in the node
        memset(temp, '\0', sizeof(temp));
        strcpy(temp, currentNode->str);

        int i=0;
        while (temp[i]==str[i]) i++;

        //existing string is part of the incoming string 
        if (i==strlen(currentNode->str)){
            //pass the stripped incoming string to create a new node
            insertWord(&str[i], currentNode);
        }

        /* existing string is not a part of the incoming string. Therefore,
            we have to insert a new node inside headNode and the currentNode*/
        else {
            //modify the string in current node
            memset(currentNode->str, '\0', sizeof(currentNode->str));
            strcpy(currentNode->str, &temp[i]); //copy non matching characters to the current node

            head->children[temp[0]-'a'] = createNode(); //create a new node for replace the existing node and connect that to the parent
            memset(head->children[temp[0]-'a']->str, '\0', sizeof(head->children[temp[0]-'a']->str));
            strncpy(head->children[temp[0]-'a']->str, temp, i); //copy matching characters to the new node
            head->children[temp[0]-'a']->isLeaf = false; //new node is not leaf

            head->children[temp[0]-'a']->children[temp[i]-'a'] = currentNode; //connect newly created node with it's child

            //incoming string is part of the existing string
            if(i==strlen(str)){
                head->children[temp[0]-'a']->isWordEnd = true;
            }
            //incoming string has more characters to add
            else {
                head->children[temp[0]-'a']->isWordEnd = false;
                insertWord(&str[i], head->children[temp[0]-'a']);    
            }
        }
    }
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
            strcat(temp, currentNode->str);
            
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

    TrieNode *radixHead =  createNode(); //create root node

    //insert all words into the trie data structure
    char str[MAX_WORD_SIZE];
    int c;
    char ch[MAX_WORD_SIZE]; //for the purpose of filtering
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
        insertWord(ch, radixHead);
    }
    
    fclose(file); //close the file after all words updated into the trie
    
    //ask user to enter a word for give suggestions
    printf("Enter first n characters a word : ");
    while (1){
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
        if(!printSuggestions(radixHead, ch)){
            printf("No suggestions found\n");
        }

        //get more input
        printf("\nEnter first n characters a word : ");
    }

    return 0;
}