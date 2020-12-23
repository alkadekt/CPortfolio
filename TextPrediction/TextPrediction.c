/* COP 3502C Programming Assignment 4: Text Prediction
This program is written by: Andrew Keehan */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#define MAXCHAR 2000000
#define MAXFREQ 1000000000

//Coordinates structure
typedef struct trie{
    int isWord;
    int NodeFreq;
    int PrefixFreq;
    int MaxFreq;
    struct trie* next[26];
}trie;

//Function Prototypes
trie* init();
void insert(trie* tree, char word[], int Freq, int key);
void QueryTest(FILE* outfp, trie* tree, char word[], int key);
void printTree(trie* tree, char word[]);
int isEmpty(trie* root);
int HasOneKid(trie* tree);
trie* MaxChild(trie* tree);
int FreqDiff(trie* parent, trie* child);
void freeDictionary(trie* tree);

//MAIN FUNCTION
int main(){

    //Some declarations
    int i;
    int NumCommands, Command, NumEntries;

    //Open the input and output files
    FILE *fp = fopen("in.txt", "r");
    FILE *outfp = fopen("out.txt", "w");

    //Create a dictionary
    trie* myDictionary = init();

    //Get the number of commands from input file
    fscanf(fp, "%d", &NumCommands);

    char Entry[(MAXCHAR/NumCommands)/2];
    char Query[(MAXCHAR/NumCommands)/2];

    for(i=0;i<NumCommands;i++){
        fscanf(fp, "%d", &Command);
        //Insert a word
        if(Command == 1){
            fscanf(fp, "%s", Entry);
            fscanf(fp, "%d", &NumEntries);
            //Insert word into dictionary
            insert(myDictionary,Entry,NumEntries,0);
        }
        //Search for a word
        if(Command == 2){
            fscanf(fp, "%s", Query);
            //Test the Query
            QueryTest(outfp,myDictionary,Query,0);
        }
    }

    /*
    //Print all the words
    printf("\nPrinting the Tree:\n");
    char word[1];
    word[0] = '\0';
    printTree(myDictionary, word);
    */

    //Close files
    fclose(fp);
    fclose(outfp);
    //Free memory
    freeDictionary(myDictionary);
    return 0;
}

//FUNCTIONS
trie* init(){
//This function initializes the tree
    //Create the structure, not a word
    trie* myTree = malloc(sizeof(trie));
    myTree->isWord = 0;
    myTree->NodeFreq = 0;
    myTree->PrefixFreq = 0;
    myTree->MaxFreq = 0;
    //Set each pointer to NULL
    int i;
    for(i=0; i<26; i++){
        myTree->next[i] = NULL;
    }
    //Return a pointer to the new root
    return myTree;
}

void insert(trie* tree, char word[], int Freq, int key){
//Inserts a word into the tree recursively

    //Adjust the PrefixFreq
    tree->PrefixFreq = tree->PrefixFreq + Freq;

    //Down to the end, insert the word
    if(key == strlen(word)){
        tree->isWord = 1;
        tree->NodeFreq = tree->NodeFreq + Freq;
        if(isEmpty(tree)){
            tree->MaxFreq = 0;
        }
        return;
    }

    //See if the next place to go exists, if not, create it
    int nextIndex = word[key] - 'a';
    if(tree->next[nextIndex] == NULL){
        //Initializing a new child
        if(Freq > tree->MaxFreq - Freq){
            tree->MaxFreq = Freq;
        }
        else{
            tree->MaxFreq = tree->MaxFreq - Freq;
        }
        tree->next[nextIndex] = init();
    }
    //Recursive call to insert for the children
    insert(tree->next[nextIndex],word,Freq,key+1);

    //Adjust the MaxFreq
    if(HasOneKid(tree) == 1){
        tree->MaxFreq = tree->PrefixFreq;
    }
    else{
        tree->MaxFreq = MaxChild(tree)->PrefixFreq;
    }
}

void QueryTest(FILE* outfp, trie* tree, char word[], int key){
//This function searches the tree to print the suggestion

    //If we've reached the end of the query word
    if(key == strlen(word)){
        //If the node we're on is a word, the query is not a proper prefix
        if(tree->MaxFreq == 0){
            fprintf(outfp,"unknown word\n");
            return;
        }
        char output[27];
        int tempDiff = MAXFREQ;
        //Go through each child and print the ones that have the same PrefixFreq as the current MaxFreq
        for(int i=0;i<26;i++){
            if(tree->next[i] != NULL){
                //if the difference is the same, add char to end
                if(FreqDiff(tree,tree->next[i]) == tempDiff){
                    int j = 0;
                    while(output[j] != '\0'){
                        j++;
                    }
                    output[j] = (char)('a' + i);
                    output[j+1] = '\0';
                }
                //if the difference is smaller, replace with new char
                if(FreqDiff(tree,tree->next[i])<tempDiff){
                    output[0] = (char)('a' + i);
                    output[1] = '\0';
                    tempDiff = FreqDiff(tree,tree->next[i]);
                }
            }
        }
        //Print the output
        fprintf(outfp,"%s\n", output);
        return;
    }

    //If the next place doesn't exist, query does not exist in dictionary
    int nextIndex = word[key] - 'a';
    if(tree->next[nextIndex] == NULL){
        fprintf(outfp,"unknown word\n");
        return;
    }
    //Recursive call of function
    QueryTest(outfp, tree->next[nextIndex], word, key+1);
}

void printTree(trie* tree, char word[]){
//Prints the entire tree, all nodes
    if(tree == NULL)
        return;

    //Print this node
    if(strlen(word) != 0)
        printf("%s\tNodeFreq: %d\tPrefixFreq: %d\t  MaxFreq: %d\n", word, tree->NodeFreq, tree->PrefixFreq, tree->MaxFreq);
    else
        printf("ROOT\tNodeFreq: %d\tPrefixFreq: %d\t  MaxFreq: %d\n", tree->NodeFreq, tree->PrefixFreq, tree->MaxFreq);
    //Safer if we store this.
    int len = strlen(word);
    //Recursively print all words in each subtree in alpha order.
    int i;
    for(i=0; i<26; i++){
        //Add a letter to our word
        word[len] = (char)('a' + i);
        word[len+1] = '\0';
        //Keep printing out word segments for each child
        printTree(tree->next[i], word);
    }
}

int isEmpty(trie* root){
//Checks to see if tree is empty
    for(int i=0;i<26;i++){
        if(root->next[i])
            return 0;
    }
    return 1;
}

int HasOneKid(trie* tree){
//Returns 1 if the node has only one kid
    int i, NumKids;
    for(i=0;i<26;i++){
        if(tree->next[i] != NULL){
            NumKids++;
        }
    }
    if(NumKids == 1)
        return 1;

    return 0;
}

trie* MaxChild(trie* tree){
//Returns the child with the biggest PrefixFreq
    int i;
    trie* temp = init();
    for(i=0;i<26;i++){
        if(tree->next[i] != NULL && tree->next[i]->PrefixFreq > temp->PrefixFreq){
            temp = tree->next[i];
        }
    }
    return temp;
}

int FreqDiff(trie* parent, trie* child){
//Finds the int difference between MaxFreq of parent and PrefixFreq of child
    int difference;
    difference = parent->MaxFreq - child->PrefixFreq;
    return difference;
}

void freeDictionary(trie* tree){
//Frees memory of the tree
    int i;
    for(i=0; i<26; i++){
        if(tree->next[i] != NULL)
            freeDictionary(tree->next[i]);
    }
    free(tree);
}
