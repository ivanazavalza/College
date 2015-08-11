/*Created By: Jared Fowler

This file contains a basic Node and bst class.
Used for simple deletion and height tests.*/

#ifndef BASICBST_H
#define BASICBST_H

class Node{

public:
    Node* rightChild;
    Node* leftChild;
    int value;

    Node(){}
    Node(int i){
        value = i;
        rightChild = nullptr;
        leftChild = nullptr;
    }

};


class BinarySearchTree{

public:
    Node* rootPtr;
    int maxCount; //Used for TreeHeight

    BinarySearchTree(Node* root){
        rootPtr = root;
    }

    BinarySearchTree(){}

    //Insert
    void Add(int value){
        //We will use reccurrence to find the right spot
        AddReccurrence(value, rootPtr);
    }


    bool remove(int data)
    {

        //Note:  The book would have me search to see if the value exists... why do that if 
        //i have to go through the tree anyways?  A:  function independence and reuseability?

        bool removed = false;

        rootPtr = removeValue(rootPtr, data, removed);

        return removed;
    }

    Node* removeValue(Node* subTreePtr, int target, bool& success)
    {
        success = false;

        if (subTreePtr == nullptr)  //If node is nullptr, return nullptr
            return subTreePtr;

        //if node isn't nullptr, test current node 
        if (subTreePtr->value == target)
        {
            subTreePtr = removeNode(subTreePtr);
            success = true;
            return subTreePtr;
        }

        if (success == false)
        {
            subTreePtr->leftChild = (removeValue(subTreePtr->leftChild, target, success));
        }

        if (success == false)
        {
            subTreePtr->rightChild = (removeValue(subTreePtr->rightChild, target, success));
        }

        return subTreePtr;

    }

    Node* removeNode(Node* nodePtr)
    {
        //if it's a leaf
        if (nodePtr->leftChild == nullptr && nodePtr->rightChild == nullptr)
        {
            delete nodePtr;
            nodePtr = nullptr;
            return nullptr;
        }

        //if has one child
        else if (nodePtr->leftChild == nullptr || nodePtr->rightChild == nullptr)
        {
            Node* temp;

            if (nodePtr->leftChild == nullptr)
                temp = nodePtr->rightChild;

            else
                temp = nodePtr->leftChild;

            delete nodePtr;
            nodePtr = nullptr;
            return temp;
        }

        //has two children
        else
        {
            int entry;
            Node* temp = removeLeftmostNode(nodePtr->rightChild, entry);
            nodePtr->rightChild = temp;
            nodePtr->value = entry;
            return nodePtr;
        }
    }

    Node* removeLeftmostNode(Node* subTreePtr, int& inorderSuccessor)
    {
        if (subTreePtr->leftChild == nullptr)
        {
            inorderSuccessor = subTreePtr->value;
            return removeNode(subTreePtr);
        }
        else
        {
            subTreePtr->leftChild = (removeLeftmostNode(subTreePtr->leftChild, inorderSuccessor));
            return subTreePtr;
        }

    }

    //TreeHeight
    int TreeHeight(){ //Uses a bfs and counts the longes branch we can travel
        //Alogrithm will basically determine which of its children is longer,
        //and add its own height to it.
        int numNodes = TreeHeightReccurrence(rootPtr);
        //Because the height is number of edges, we take away 1
        return numNodes - 1;
    }

    //CleanUP
    void CleanUp(){
        //Deletes all dynamic memory used by the tree
        CleanUpReccurrence(rootPtr);
    }


private:

    void AddReccurrence(int value, Node* currentNode){
        //Compare the current values
        if (currentNode->value > value){
            //Do we have a left child already?
            if (currentNode->leftChild == nullptr)
            {
                //If not, put the value there
                currentNode->leftChild = new Node(value);
            }
            //Else, we traverse until we find the right spot
            else{
                AddReccurrence(value, currentNode->leftChild);
            }
        }
        else{ //value goes to the right
            //Do we have a right child already?
            if (currentNode->rightChild == nullptr)
            {
                //If not, put the value there
                currentNode->rightChild = new Node(value);
            }
            //Else, we traverse until we find the right spot
            else{
                AddReccurrence(value, currentNode->rightChild);
            }
        }
    }

    int TreeHeightReccurrence(Node* currentNode){
        int heightLeft = 0;
        int heightRight = 0;

        //Get the height of the left subtree
        if (currentNode->leftChild != nullptr){
            heightLeft = TreeHeightReccurrence(currentNode->leftChild);
        }
        //Get the height of the right subtree
        if (currentNode->rightChild != nullptr){
            heightRight = TreeHeightReccurrence(currentNode->rightChild);
        }

        //Which of the two is greater?
        if (heightLeft > heightRight){
            return heightLeft + 1;
        }
        else{
            return heightRight + 1;
        }
    }

    void CleanUpReccurrence(Node* currentNode){
        //Is there anything in the left subtree?
        if (currentNode->leftChild != nullptr){
            //If yes, we need to go down it
            CleanUpReccurrence(currentNode->leftChild);
        }
        //Is there anything down the right subtree?
        if (currentNode->rightChild != nullptr){
            //If yes, we need to go down it
            CleanUpReccurrence(currentNode->rightChild);
        }

        //At this point, there is nothing below the currentNode, so we delete it
        delete currentNode;
    }

    Node* getNodePtr(int value, Node* currentNode){

        Node* temp;

        //Is the current Node the one we are looking for?
        if (currentNode->value == value){
            return currentNode;
        }
        //Go down the left subtree if it exists
        if (currentNode->leftChild != nullptr){
            //If yes, we need to go down it
            temp = getNodePtr(value, currentNode->leftChild);
        }
        //If we havn't found it yet, go down right subtree
        if (temp == nullptr && currentNode->rightChild != nullptr){
            temp = getNodePtr(value, currentNode->rightChild);
        }

        //Regardless of what we have found, return temp
        return temp;
    }




};

#endif