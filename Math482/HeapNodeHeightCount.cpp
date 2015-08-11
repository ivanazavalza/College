/*Created By: Jared Fowler
  June 2014
  Math 482 Combinational Algorithms
  Professor John Dye
  
  
  A common test question is to find the number of 
  nodes with height 'n'.  This program takes care 
  of this task.*/

#include <iostream>
#include <cmath>
using namespace std;

int main(){

    int nodes = 27;
    int height = 2;

    //Step 1: We find the height of the root
    int treeHeight = log(nodes) / log(2);
    //Test if the height is valid
    if (treeHeight < height){
        cout << "A binary heap of " << nodes << " nodes has 0 nodes of height " << height << endl << endl;
        return 0;
    }
    //Step 2: We find out how many nodes are being used on the bottom row of the entire tree
    int usedNodes = nodes - (pow(2, treeHeight) - 1);
    //Step 3: We find out how many nodes are not being used on the bottom row of entire tree
    int freeNodes = pow(2, treeHeight) - usedNodes;

    /*The next few steps are more tricky.  First, imagine an incomplete binary tree. We want to
    find the nodes with a height of k, which have height k due to having nodes on the very bottom
    row of the tree.  At height k, there will be 2^k nodes under a node with that height.  We can 
    therefore take the number of nodes at that row "usedNodes" and divide it by 2^k.  The cieling 
    of this is the number of nodes with a height of k*/

    //Step 4: Get nodes with hieght k that reach into very bottom row
    int total1 = ((usedNodes / (pow(2, height))) + .99999); //Hopefully works 99% of time for cieling
    
    /*The remaining nodes with height k must be 1 level higher than the ones we just found.  We don't
    want to double count, however, so we need to find how many nodes make part of the ones we just counted*/

    //Step 5: Get nodes that have already counted nodes as subtrees
    int parentOfCounted = ((total1 / 2.0) + .99999);

    /*If this number is equal to the number of nodes in that row of the tree, then we are done. Also confirm
    that we are not going into a depth < 0*/

    //Step 6: Are we done?
    if (parentOfCounted == (pow(2, treeHeight - height - 1))){
        cout << "A binary heap of " << nodes << " nodes has " << total1 << " nodes of height " << height << endl << endl;
    }
    else if ((treeHeight - height - 1) < 0){
        cout << "A binary heap of " << nodes << " nodes has " << total1 << " nodes of height " << height << endl << endl;
    }

    /*How many more nodes are on that row? Find it, and subtract it from the 'parentOfCounted'*/

    //Step 7: Find the remaining nodes
    else{
        int total2 = pow(2, treeHeight - height - 1) - parentOfCounted;
        cout << "A binary heap of " << nodes << " nodes has " << total1 + total2 << " nodes of height " << height << endl << endl;
    }


    return 0;
}