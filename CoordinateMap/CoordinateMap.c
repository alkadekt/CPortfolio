/* COP 3502C Programming Assignment 3: Coordinate Map
This program is written by: Andrew Keehan */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

//Coordinates structure
typedef struct coords{
    int x;
    int y;
}coords;

//Global variable for my location
coords MySpot;

//Function Prototypes
coords** ReadData(FILE *ifp, int NumInfected);
int compareTo(coords *ptrPt1, coords *ptrPt2);
int distance(coords *ptr, coords MySpot);
int BinarySearch(coords *Arr1[], coords *query, int len);
void Sort(coords *Arr1[], int len, int t); //NOT recursive
void mergeSort(coords *Arr1[], int FirstIndex, int LastIndex, int t); //Recursive
void merge(coords *arr[], int l, int m, int r);
void insertionSort(coords *Arr1[], int FirstIndex, int LastIndex);
void printArray(coords *Arr1[], int size, FILE *filePoint);

//MAIN FUNCTION
int main(){

    //Declarations
    int i;
    int NumInfected; //Number of infected people
    int NumPoints; //Number of points to search for
    int Threshold; //Used to decide whether to use Merge or Insertion Sort

    //Open the input and output files
    FILE *fp = fopen("in.txt", "r");
    FILE *outfp = fopen("out.txt", "w");

    //Read in my location
    fscanf(fp, "%d", &MySpot.x);
    fscanf(fp, "%d", &MySpot.y);

    //Read in the first points of data
    fscanf(fp, "%d", &NumInfected);
    fscanf(fp, "%d", &NumPoints);
    fscanf(fp, "%d", &Threshold);

    //Call the ReadData function
    coords **CArray = ReadData(fp, NumInfected);

    //Sort the coordinate data
    Sort(CArray, NumInfected, Threshold);
    printArray(CArray, NumInfected, outfp);

    //Search for the points we need to search for
    //First, read in the data of the points to search for
    coords **PointGroup = ReadData(fp, NumPoints);

    for(i=0; i<NumPoints; i++){
        //Get rank of the point if point exists
        int b = BinarySearch(CArray, PointGroup[i], NumInfected);
        if(b != -1){
            fprintf(outfp, "%d %d found at rank %d\n", PointGroup[i]->x, PointGroup[i]->y, b);
        }
        else{
            fprintf(outfp, "%d %d not found\n", PointGroup[i]->x, PointGroup[i]->y);
        }
    }

    //Close files
    fclose(fp);
    fclose(outfp);
    return 0;
}

//FUNCTIONS
coords** ReadData(FILE *fp, int NumInfected){
//This function reads in the data for the locations of infected people
//It uses DMA to store their data
    int i;
    int j = 0;
    coords **CoordArr = (coords**)malloc(NumInfected * sizeof(coords*));
    for(i=0; i<NumInfected; i++){
        CoordArr[i] = (coords*)malloc(sizeof(coords));
        fscanf(fp, "%d", &CoordArr[i]->x);
        fscanf(fp, "%d", &CoordArr[i]->y);
        j++;
    }
    return CoordArr;
}

int compareTo(coords *ptrPt1, coords *ptrPt2){
//This function finds which coordinate pair is closer
    int ONE = distance(ptrPt1, MySpot);
    int TWO = distance(ptrPt2, MySpot);
    if(ONE < TWO){
        return -1;
    }
    else if(TWO < ONE){
        return 1;
    }
    else if(ONE == TWO){
        if(ptrPt1->x == ptrPt2->x && ptrPt1->y == ptrPt2->y){
            return 0;
        }
        else if(ptrPt1->x < ptrPt2->x){
            return -1;
        }
        else if(ptrPt2->x < ptrPt1->x){
            return 1;
        }
        else if(ptrPt1->x == ptrPt2->x && ptrPt1->y < ptrPt2->y){
            return -1;
        }
        else if(ptrPt1->x == ptrPt2->x && ptrPt2->y < ptrPt1->y){
            return 1;
        }
    }
}

int distance(coords *ptr, coords MySpot){
//This returns the distance between a coordinate and my spot
//It doesn't do the full square root at the end because it's too complicated
    int range;
    range = (MySpot.x - ptr->x)*(MySpot.x - ptr->x) + (MySpot.y - ptr->y) * (MySpot.y - ptr->y);
    return range;
}

int BinarySearch(coords *Arr1[], coords *query, int len){
//This function searches through a sorted list for a coordinate
//If it finds the coordinate, it returns its rank
//If it does not find the coordinate, it returns -1
    int l = 0, h = len - 1;
    int mid;
    while(l <= h){
        mid = (l + h) / 2;
        //Check if item is present at mid
        if(compareTo(Arr1[mid], query) == 0)
            return mid + 1;
        //If item greater, ignore left half
        if(compareTo(Arr1[mid], query) == -1)
            l = mid + 1;
        //If item is smaller, ignore right half
        else
            h = mid - 1;
    }
    //if we reach here, then element was not present
    return -1;
}

void Sort(coords *Arr1[], int len, int t){
//Wrapper sort function decides which type of sort to use
//If the length of the array is less than or equal to the threshold, use insertion
    if(len <= t){
        insertionSort(Arr1, 0, len-1);
    }
    else{
        mergeSort(Arr1, 0, len-1, t);
    }
}

//RECURSIVE FUNCTION FOR MERGESORT
void mergeSort(coords *Arr1[], int FirstIndex, int LastIndex, int t){
//This sorts a list of coordinates in order of distance from My Spot


    if(LastIndex - FirstIndex <= t){
        insertionSort(Arr1, FirstIndex, LastIndex);
    }
    else{
        int m = (FirstIndex + LastIndex)/2;
        //Sort the first and second halves
        mergeSort(Arr1, FirstIndex, m, t);
        mergeSort(Arr1, m+1, LastIndex, t);

        merge(Arr1, FirstIndex, m, LastIndex);
    }
}

void merge(coords *arr[], int l, int m, int r){
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;
    //create temp arrays
    coords **L = (coords**)malloc(n1 * sizeof(coords*));
    coords **R = (coords**)malloc(n2 * sizeof(coords*));
    //Copy data to temp arrays L[] and R[]
    for(i = 0; i < n1; i++){
        L[i] = (coords*)malloc(sizeof(coords));
        L[i] = arr[l + i];
    }
    for(j = 0; j < n2; j++){
        R[j] = (coords*)malloc(sizeof(coords));
        R[j] = arr[m + 1+ j];
    }
    //Merge the temp arrays back into arr[l..r]
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while(i < n1 && j < n2){
        if(compareTo(L[i], R[j]) == -1){
            arr[k] = L[i];
            i++;
        }
        else{
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    //Copy the remaining elements of L[], if there are any
    while (i < n1){
        arr[k] = L[i];
        i++;
        k++;
    }
    //Copy the remaining elements of R[], if there are any
    while (j < n2){
        arr[k] = R[j];
        j++;
        k++;
    }
    free(L);
    free(R);
}

void insertionSort(coords *Arr1[], int FirstIndex, int LastIndex){
//This sorts a list of coordinates in order of distance from My Spot
    int i, j;
    coords *item; //temporary coordinate holder

    for(i=FirstIndex + 1; i<=LastIndex; i++){
        item = Arr1[i];

        for(j=i-1; j>=FirstIndex; j--){
            if(compareTo(Arr1[j], item) == 1 || compareTo(Arr1[j], item) == 0){
                Arr1[j+1] = Arr1[j];
            }
            else{
                break;
            }
        }
        Arr1[j+1] = item;
    }
    return;
}

void printArray(coords *Arr1[], int size, FILE *filePoint){
//This function prints out the array
    int i;
    for(i=0; i<size; i++){
        fprintf(filePoint, "%d %d\n", Arr1[i]->x, Arr1[i]->y);
    }
    return;
}
