/*Created By: Jared Fowler
yrolg4tseuq@sbcglobal.net

Math 482, Combinational Algorithms
Professor John Dye
CSUN

A common test question is to find the average height of 
a binary search tree after a randomly selected node is
deleted.  To do this, I can only think of actually writing 
out the trees, and then visualizing what would happen for
every single deletion.  The purpose of this program is to
spit out some results which can then be analyzed to determine
if there exists some general formula to solve this problem.*/

#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
using namespace std;

#include "BasicBST.h"


//A basic function to find the factorial of a number
int Factorial(int n){
    //Stopping condition
    if (n == 1)
        return 1;
    //Else
    return (Factorial(n - 1) * n);
}


int main(){

    //Changeable Variables
    int n = 8; //Number of nodes.. will be numbered 1 through n

    //Do not change anything below this line
    //We want to keep track of the resultant height and how many we have of each
    vector<int> vecHeight(n);
    //Fill this array with intial height of zero
    for (size_t index = 0; index < vecHeight.size(); index++){
        vecHeight[index] = 0;
    }

    //Fill vector with values to be inserted
    vector<int> vec(n);
    for (size_t index = 0; index < vec.size(); index++){
        vec[index] = index + 1;	//Numbers 1-n
    }

    //Main loop
    do{

        //For loop will control the random value deletions
        for (int i = 8; i < 9; i++){

            //Create a bst with the first node
            BinarySearchTree bst(new Node(vec[0]));

            //Add the remaining values to the tree
            for (size_t index = 1; index < vec.size(); index++){
                bst.Add(vec[index]);
            }

            //Delete a random value.. in this case 'i'
            bst.remove(i);

            //Get the new height
            vecHeight[bst.TreeHeight()] += 1;

            //Destroy the tree
            bst.CleanUp();

            //Restart
        }

    } while (next_permutation(vec.begin(), vec.end()));


    int totalHeight = 0;

    //Finally, print out the height array and answer
    for (size_t index = 1; index < vecHeight.size(); index++){
        cout << "Height " << index << ": " << vecHeight[index] << endl;
        totalHeight += (vecHeight[index] * index);
    }

    cout << endl << endl;
    cout << "Avg. Resulting Height: " << (static_cast<double>(totalHeight) / (Factorial(n) * 1)) << endl << endl;


    return 0;
}