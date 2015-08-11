/*Created By: Jared Fowler
June 12, 2014
Math 482 Combinational Algorithms
Professor John Dye


A common Homework question involvs finding how many patterns of trees
exist, based off the permutations of n numbers, where each
number is distinct.  We will call these numbers 1-n, as what their
values really are doesn't matter. (It only matters that one is bigger
than the other.*/

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;

//We will need a basic node
class node{

public:
    node* rightChild;
    node* leftChild;
    int value;
    int locationValue; //Where in the tree does this node exist?

    node(){}
    node(int i, int l){
        value = i;
        locationValue = l;
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

//Recursive Function that will create a pattern for us
string FindPattern(node* rootNode){

    //Add current nodes information to the pattern
    string pattern = "*" + to_string(rootNode->locationValue);

    //Add left child node information
    if (rootNode->leftChild != NULL){
        pattern.append(FindPattern(rootNode->leftChild));
    }

    //Add right child node information
    if (rootNode->rightChild != NULL){
        pattern.append(FindPattern(rootNode->rightChild));
    }

    //Return pattern
    return pattern;
    
}

//Main program
int main(){

    //Variables (Can be changed)
    int n = 12;	//# of elements 
    int k = 5;	//desired height

    //Do not change anything below this line
    /*Patterns will be stored as strings consisting of 1's and 0's. We
    will use a vector to hold a list of strings with different patterns.
    The final count (what we want to find) will be determined by the vector size*/
    vector<string> stringAry;
    
    /*For future clean up, we create an array of pointers to nodes.
    This will bypass the need to traverse a tree, deleting each node.
    (This means that there will be 2 pointers to same object)*/
    vector<node*> aryOfPtrs(n);

    #if(0) //Manual
    //Step 1: We need to determine how many permutations exist.  
    int permutations = Factorial(n-1);
    #endif


    #if(1) //Auto
    //Step 1: We need to determine how many permutations exist.  
    int permutations = Factorial(n);
    #endif


    //Progress Bar for large projects
    bool PB = false;
    if (n >= 8)
        PB = true;

    int interval = permutations / 100;

    
    //Step 2: We need to set up the first permutation. (Array of ints)
    vector<int> vec(n);
    #if(0) //Manual
    vec[0] = 6;
    vec[1] = 1;
    vec[2] = 2;
    vec[3] = 3;
    vec[4] = 4;
    vec[5] = 5;
    vec[6] = 7;
    vec[7] = 8;
    vec[8] = 9;
    vec[9] = 10;
    vec[10] = 11;
    vec[11] = 12;
    #endif
    #if(1)
    for (size_t index = 0; index < vec.size(); index++){
        vec[index] = index + 1;	//Numbers 1-n
    }
    #endif


    //Step 3: Run through every permutation, building every tree and compare
    //Start the loop that will take us through each of these permutations
    for (int index = 0; index < permutations; index++){

    #if(0) //manual
        //Step A: Fill an array with the next permutation.
        if (index != 0){
            next_permutation(vec.begin()+1, vec.end());
        }
    #endif

    #if(1) //Auto
        //Step A: Fill an array with the next permutation.
        if (index != 0){
            next_permutation(vec.begin(), vec.end());
        }
    #endif

        #if(0)
        //Print permutation
        for (size_t i = 0; i < vec.size(); i++){
            cout << vec[i];
        }
        //cout << endl;
        #endif

        /*Step B: We now need to fill a binary tree with this info.
        While doing this, we also need to keep track of the trees height.
        We will first place the root of the tree, and then we will count
        how many levels we go down to insert a node.  If this count is greater
        than the current height we have recorded for the tree, we will update
        the tree's height.  This is repeated until we have inserted all of the
        nodes.*/
        int treeHeight = 0;	//Height of the tree

        //Keep track of node insertion count
        int nodeCount = 0;

        //Create root node and put it into a new binary tree
        node* rootNode = new node(vec[0], 1);
        //Have our array of pointers keep track of this dynamic memory
        aryOfPtrs[0] = rootNode;
        nodeCount++;

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
                        currentNode->leftChild = new node(vec[index2], ((currentNode->locationValue) * 2));
                        //Also have my array of pointers point to it
                        aryOfPtrs[index2] = currentNode->leftChild;
                        nodeCount++;
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
                        currentNode->rightChild = new node(vec[index2], ((currentNode->locationValue) * 2) + 1);
                        //Also have my array of pointers point to it
                        aryOfPtrs[index2] = currentNode->rightChild;
                        nodeCount++;
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


        /*Setp B2: Generate unique pattern.  I've decided the best way to do this
        would be to traverse the tree in a bfs alorithm.  The nodeLocations will
        be written to a string, which will serve as the binary tree's unique pattern*/

        //We need a string where we can set up our pattern
        string pattern;

        pattern = FindPattern(rootNode);


        /*Step C: At this point we have created a binary search tree and have recorded its height.
        At this point due to a previous check, there should only be trees of height k or less
        We finally compare this height to our desired height k. If they are the same, we compare
        the string to the other strings.  If unique, we add it to the vector.
        Afterwards, we clean up our dynamic memory using the array of pointers.  
        The loop then starts again.*/
        #if(1)
        if (treeHeight == k){
            bool unique = true;
            for (size_t i = 0; i < stringAry.size(); i++){
                //Is our pattern unique?
                if (pattern.compare(stringAry[i]) == 0){ //Returns 0 if equal (Not Unique)
                    unique = false;
                    break;
                }
            }
            if (unique){
                stringAry.push_back(pattern);
            }
        }
        #endif
            

        #if(0)
        cout << " Pattern = " << pattern << "  Height = " << treeHeight << endl;
        #endif

        #if(1)
        for (int i = 0; i < nodeCount; i++){
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
    cout << "Root 1: Patterns of Binary Trees with " << n << " nodes and height " << k << " :  " << stringAry.size() << endl;

    return 0;
}