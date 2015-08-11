//Created By: Jared Fowler

#include <iostream>

//Some Global Variables
int total_compare;
int member_compare;
int member;


//Stooge Sort
void Sort(int A[], int i, int j){

    //We need to update the number of comparisons.
    total_compare++;
    //We need to check if the specific member is being compared
    if ((A[i] == member) || (A[j] == member))
        member_compare++;
        
    if (A[i] > A[j])
    {
        //Exchange
        int temp = A[i];
        A[i] = A[j];
        A[j] = temp;
    }

    if (i + 1 >= j)
        return;

    int k = (j - i + 1) / 3;

    //Recursive Calls
    Sort(A, i, j - k);
    Sort(A, i + k, j);
    Sort(A, i, j - k);
}


int main(){

    //We need an array and a member
    //UPDATE INFO HERE!---------------------------------
    int A[] = { -1, 7, 6, 5, 4, 3, 2, 1 };
    //int A[] = {-1, 1, 2, 3, 4, 5, 6 };
    member = 4;
    //END UPDATE!---------------------------------------


    //We need to reset the counters
    total_compare = 0;
    member_compare = 0;

    //Do the algorithm
    Sort(A, 1, 7);

    //Print out the results
    std::cout << "Total Comparisons: " << total_compare << std::endl;
    std::cout << "Member " << member << " Comparisons: " << member_compare << std::endl;

    return 0;
}