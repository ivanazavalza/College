/*Created By: Jared Fowler
  June 12, 2014
  Math 482 Combinational Algorithms
  Professor John Dye
  
  
  A common Homework question involvs finding how many binary trees
  have height k, based off the permutations of n numbers, where each
  number is distinct.  We will call these numbers 1-n, as what their 
  values really are doesn't matter. (It only matters that one is bigger
  than the other.*/

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

//We will need a basic node
class node{

public:
    node* rightChild;
    node* leftChild;
    int value;

    node(){}
    node(int i){
        value = i;
        rightChild = NULL;
        leftChild = NULL;
    }

};

//A basic function to find the factorial of a number
int Factorial(int n){
    //Stopping condition
    if (n == 1)
        return 1;
    //Else
    return (Factorial(n - 1) * n);
}

//Main program
int main(){

    //Variables (Can be changed)
    int n = 12;	//# of elements 
    int k = 6;	//desired height

    //Variable. Do not change anything below this line
    int totalCount = 0;	//Number of trees with height k

    /*For future clean up, we create an array of pointers to nodes.
    This will bypass the need to traverse a tree, deleting each node.
    (This means that there will be 2 pointers to same object)*/
    vector<node*> aryOfPtrs(n);

    //Step 1: We need to determine how many permutations exist.  
    int permutations = Factorial(n);

    //Progress Bar for large projects
    bool PB = false;	
    if (n >= 8)
        PB = true;

    int interval = permutations / 100;


    //Step 2: We need to set up the first permutation. (Array of ints)
    vector<int> vec(n);
    for (size_t index = 0; index < vec.size(); index++){
        vec[index] = index + 1;	//Numbers 1-n
    }

    //Step 3: Run through every permutation, building every tree and compare
    //Start the loop that will take us through each of these permutations
    for (int index = 0; index < permutations; index++){

        //Step A: Fill an array with the next permutation.
        if (index != 0){
            next_permutation(vec.begin(), vec.end());
        }

        #if(0)
        //Print permutation
        for (size_t i = 0; i < vec.size(); i++){
            cout << vec[i];
        }
        cout << endl;
        #endif

        /*Step B: We now need to fill a binary tree with this info.
        While doing this, we also need to keep track of the trees height.
        We will first place the root of the tree, and then we will count
        how many levels we go down to insert a node.  If this count is greater
        than the current height we have recorded for the tree, we will update
        the tree's height.  This is repeated until we have inserted all of the
        nodes.*/
        int treeHeight = 0;	//Height of the tree

        //Create root node and put it into a new binary tree
        node* rootNode = new node(vec[0]);
        //Have our array of pointers keep track of this dynamic memory
        aryOfPtrs[0] = rootNode;

        node* currentNode = NULL;
        

        //Now, while elements remain in the vector, continue to insert elements
        for (size_t index2 = 1; index2 < vec.size(); index2++){

            //How far down the tree have we gone?
            int count = 0;

            //We begin at the root
            currentNode = rootNode;

            //Compare with nodes until place is found
            while (true){
                if (vec[index2] < currentNode->value){	//Left Child?
                    //Is that spot empty?
                    if (currentNode->leftChild == NULL){
                        //Create new node and have current node point to it
                        currentNode->leftChild = new node(vec[index2]);
                        //Also have my array of pointers point to it
                        aryOfPtrs[index2] = currentNode->leftChild;
                        //Increase the count;
                        count++;
                        //Get out of this while loop
                        break;
                    }
                    else //Spot is not empty
                    {
                        currentNode = currentNode->leftChild;
                        //Increase count
                        count++;
                    }

                }
                else{ //Right child

                    //Is that spot empty?
                    if (currentNode->rightChild == NULL){
                        //Create new node and have current node point to it
                        currentNode->rightChild = new node(vec[index2]);
                        //Also have my array of pointers point to it
                        aryOfPtrs[index2] = currentNode->rightChild;
                        //Increase the count;
                        count++;
                        //Get out of this while loop
                        break;
                    }
                    else //Spot is not empty
                    {
                        currentNode = currentNode->rightChild;
                        //Increase count
                        count++;
                    }

                }

            }

            //Do we have a new height winner? If so, update the treeHeight
            if (count > treeHeight)
                treeHeight = count;

            //Are we past the height we are trying to find?
            if (treeHeight > k)
                break;

        }

        /*Step C: At this point we have created a binary search tree and have recorded its height.
        We finally compare this height to our desired height k.  Afterwards, we clean up our dynamic
        memory using the array of pointers.  The loop then starts again.*/
        if (treeHeight == k)
            totalCount++; 

        #if(0)
        cout << "  Height = " << treeHeight << endl;
        #endif

        #if(1)
        for (size_t i = 0; i < aryOfPtrs.size(); i++){
            delete (aryOfPtrs[i]);
        }
        #endif

        //Print out progress
        if (PB){
            if ((index % interval) == 0){
                cout << "*";
            }		
        }

    }

    //Print out final results
    cout << "Binary Trees with height " << k << ":  " << totalCount << endl;

    return 0;
}