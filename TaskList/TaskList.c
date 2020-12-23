/* COP 3502C Programming Assignment 5: TaskList
This program is written by: Andrew Keehan */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#define MAXTASKS 1000000

//Task structure
typedef struct task{
    int ID;
    int timeGiven; //Time the task was assigned
    int NumPhases; //Number of phases in the task
    int *Lengths; //Array of the lengths of the phases
    int TimeLeft; //Time left to complete the task
    int whatNext; //What phase to complete next (0-based)
}task;

//Heap structure starts with size of 10
typedef struct heap{
    task **heaparray;
    int capacity;
    int size;
}heap;

//Function Prototypes
void readData(FILE* fp, task** TaskArr, int NumTasks);
heap* initHeap();
void swap(heap *h, int index1, int index2);
void percolateUp(heap* h, int index);
void percolateDown(heap* h, int index);
int compareTo(task* task1, task* task2);
int insert(heap* h, task* NewTask);
void removeLast(heap* h);
task* removeMax(heap* h);
void freeHeap(heap *h);
int TimeSum(int* Lengths, int NumPhases, int index);

//MAIN FUNCTION
int main(){

    //Declarations
    int i;
    int NumTasks, time, NumCompleted;

    //Open the input and output files
    FILE *fp = fopen("in.txt", "r");
    FILE *outfp = fopen("out.txt", "w");

    //Read in the number of tasks given
    fscanf(fp, "%d", &NumTasks);

    //Read in all the tasks into an array to start
    task **TaskArr = (task**)malloc(sizeof(task*) * NumTasks);
    for(i=0;i<NumTasks;i++){
        TaskArr[i] = (task*)malloc(sizeof(task));
    }
    readData(fp, TaskArr, NumTasks);

    //Initialize the heap
    heap *TheHeap = initHeap();

    //Receive each task and complete tasks as the time ticks by
    time = 0;
    NumCompleted = 0;
    int index = 0; //Index of task array for next task to come in
    task* WorkingTask = NULL; //Task pointer for the task we pull out of heap

    while(time < 2000000){

        //See if there is a new task to add, assuming there is one coming up
        if(time <= TaskArr[index]->timeGiven){

            if(time == TaskArr[index]->timeGiven){
                //Insert the task in the heap at the proper level
                insert(TheHeap, TaskArr[index]);

                //Increase the index of tasks we have received
                if(NumCompleted < NumTasks-1){
                    index++;
                }
            }
        }
        //Check to see if we are idle
        if(WorkingTask == NULL){

            //Start a new task, if there are any
            WorkingTask = removeMax(TheHeap);

            if(WorkingTask != NULL){
                //Set timeGiven of Working Task equal to the current clock
                //This is so we know when we are done with its current phase
                WorkingTask->timeGiven = time;
            }
            else{
            }
        }
        //If not, check if we finished the task phase we were working on
        else if(time - WorkingTask->timeGiven == WorkingTask->Lengths[WorkingTask->whatNext]){
            //Print the assignment output
            fprintf(outfp,"%d %d %d\n", time, WorkingTask->ID, WorkingTask->whatNext+1);
            //Adjust the WorkingTask traits before we decide to reinsert
            WorkingTask->whatNext++;
            WorkingTask->TimeLeft = TimeSum(WorkingTask->Lengths,WorkingTask->NumPhases,WorkingTask->whatNext);

            //If the whole task isn't done yet, we need to reinsert the task
            if(WorkingTask->whatNext < WorkingTask->NumPhases){
                insert(TheHeap, WorkingTask);

                //Set WorkingTask to NULL
                WorkingTask = NULL;
            }
            else{
                NumCompleted++;
            }
            //Choose the next task to remove from heap
            WorkingTask = removeMax(TheHeap);
            if(WorkingTask != NULL){
                //Set timeGiven of Working Task equal to the current clock
                //This is so we know when we are done with its current phase
                WorkingTask->timeGiven = time;
            }
        }
        //Increase clock time
        time++;
    }

    //Free the array and the heap
    for(i=0;i<NumTasks;i++){
        free(TaskArr[i]);
    }
    free(TaskArr);
    free(TheHeap);
    //Close files
    fclose(fp);
    fclose(outfp);
    return 0;
}

//FUNCTIONS
void readData(FILE* fp, task** TaskArr, int NumTasks){
//Reads the input file data into an array
    int i,j;
    for(i=0;i<NumTasks;i++){
        TaskArr[i]->ID = i + 1;
        fscanf(fp,"%d",&TaskArr[i]->timeGiven);
        fscanf(fp,"%d",&TaskArr[i]->NumPhases);
        //Allocate space for the lengths of the phases
        TaskArr[i]->Lengths = (int*)malloc(sizeof(int) * TaskArr[i]->NumPhases);
        //Read in the length of each phase
        for(j=0;j<TaskArr[i]->NumPhases;j++){
            fscanf(fp,"%d",&TaskArr[i]->Lengths[j]);
        }
        //Setup TimeLeft and whatNext
        TaskArr[i]->TimeLeft = TimeSum(TaskArr[i]->Lengths,TaskArr[i]->NumPhases,0);
        TaskArr[i]->whatNext = 0;
    }
    return;
}

heap* initHeap(){
//Initializes the heap

    int i;
    heap* h;
    //Default size is 10
    int size = 10;

    //Allocate space for the heap
    h = (heap*)(malloc(sizeof(heap)));
    //Capacity should be the number of tasks
    h->capacity = size;
    //Allocate space for the heap array
    h->heaparray = (task**)malloc(sizeof(task*)*(size+1));
    for(i=0;i<size+1;i++){
        h->heaparray[i] = (task*)malloc(sizeof(task));
    }
    h->size = 0;
    return h;
}

void swap(heap *h, int index1, int index2){
//Swaps tasks from index1 and index 2
    task* temp = h->heaparray[index1];
    h->heaparray[index1] = h->heaparray[index2];
    h->heaparray[index2] = temp;
}

void percolateUp(heap* h, int index){

    //Can only percolate up if the node isn't the root
    if(index > 1){
        //See if our current node has more time left than its parent
        if(compareTo(h->heaparray[index],h->heaparray[index/2]) == -1){
            //Move our node up one level
            swap(h, index, index/2);
            //See if it needs to be done again
            percolateUp(h, index/2);
        }
    }
}

void percolateDown(heap* h, int index){

    int maxIndex;

    //Only try to percolate down internal nodes
    if((2*index+1) <= h->size){
        //Find the max child of this node
        //If left child has more time left
        if(compareTo(h->heaparray[2*index],h->heaparray[2*index+1]) == -1){
            maxIndex = 2 * index;
        }
        //If right child has more time left
        else if(compareTo(h->heaparray[2*index],h->heaparray[2*index+1]) == 1){
            maxIndex = 2 * index + 1;
        }
        //If left and right children are identical somehow
        else if(compareTo(h->heaparray[2*index],h->heaparray[2*index+1]) == 0){
            maxIndex = 2 * index + 1;
        }

        //If timeLeft of maxIndex > timeLeft of parent, we need to move down
        if(compareTo(h->heaparray[maxIndex],h->heaparray[index]) == -1){
            swap(h, index, maxIndex);
            //Recursive call
            percolateDown(h, maxIndex);
        }
    }
    //If there is only a left child
    else if(h->size == 2*index){

        //No recursive call required
        if(compareTo(h->heaparray[2*index],h->heaparray[index]) == -1)
            swap(h, index, 2*index);
    }
}

int compareTo(task* task1, task* task2){
//Returns negative if first task comes first and 0 if identical
    if(task1->TimeLeft == task2->TimeLeft){
        if(task1->ID == task2->ID){
            return 0;
        }
        else if(task1->ID < task2->ID){
            return -1;
        }
        else{
            return 1;
        }
    }
    else if(task1->TimeLeft > task2->TimeLeft){
        return -1;
    }
    else{
        return 1;
    }
}

int insert(heap* h, task* NewTask){
//Inserts the task into the heap by copying

    int* temp;
    int* throwaway;
    int i;

    //Add space if array is full
    if (h->size == h->capacity) {

        //Allocate new space for an array
        h->heaparray = (task**)realloc(h->heaparray, sizeof(task*)*(2*h->capacity+1));
        //Realloc failed so we quit.
        if (h->heaparray == NULL)
            return 0;
        //Double the capacity.
        h->capacity *= 2;
    }

    //Adjust all the necessary components of h, and then move the inserted item into its appropriate location.
    h->size++;
    h->heaparray[h->size] = NewTask;
    percolateUp(h, h->size);
}

void removeLast(heap* h){
//Removes last node so it can be moved to the root during removeMax

}

task* removeMax(heap* h){
//Removes the root (the max) off the heap, then percolates down with replacement

    task* MaxTask;
    MaxTask = NULL;
    if(h->size > 0){
        //Allocate space for MaxTask
        MaxTask = (task*)malloc(sizeof(task));
        //Copy root into MaxTask
        MaxTask = h->heaparray[1];
        //Copy last task into root
        h->heaparray[1] = h->heaparray[h->size];
        //Remove last task
        removeLast(h);
        h->size--;
        //Percolate the new root down if needed
        percolateDown(h,1);
        //Return the MaxTask
        return MaxTask;
    }
    else{
        //Return the pointer as NULL
        return MaxTask;
    }
}

void freeHeap(heap *h){

}

int TimeSum(int* Lengths, int NumPhases, int index){
//Finds the time left starting at index

    int i;
    int total;
    total = 0;

    for(i=index; i<NumPhases; i++){
        total = total + Lengths[i];
    }
    return total;
}
