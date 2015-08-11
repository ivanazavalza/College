//Created By: Jared Fowler
//Math 482 - Josephus Permutation (BINARY ONLY)
//Professor: John Dye
//June 2014
//yrolg4tseuq@sbcglobal.net

/*
A common problem seen on tests and homework assignments.
This program tests an algorithm that I made in order to 
find the last, second to last, and third to last numbers
eliminated in a Josephus-Binary.  The algorithm can be 
seen below.

Credit Also Goes To Natalia Alonso Who Helped Seed The Idea.

Tested and 100% Working on Ranges 4-15,000
*/

#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

//Input K - Binary Constant
const int k = 2;

class node  //A very basic node
{
public:
    //Variables
    int value;
    bool used;

    node(int val){
        value = val;
        used = false;
    }

    node(){};
};

int main(){

    size_t index;
    int logV;
    int last;
    int second;
    int third;
    int pass = 0;
    int fail = 0;
    vector<int> failVec;

    /*This loop controls each individual test.  We find the entire
    elimination order by brute force, and then compare it to the
    algorithm. The int "n", declared in the for loop below, will
    determine our original set size*/
    for (int n = 4; n <= 20; n++){

        //Reset the index (where we are in the original list of numbers)
        size_t index = 0;

        //Vector to hold the original list of numbers
        vector<node> vec(n);
        //Vector to hold values in their "knocked-off" order (last element is winner)
        vector<int> ary(n);

        //Fill our initial vector
        for (size_t i = 0; i < vec.size(); i++){
            vec[i] = node(i + 1);
        }

        /* ***********BRUTE FORCE RUN THROUGH *********
        We step through one element at a time.  If it has not yet been "used",
        we count it, otherwise, we don't.  Once we have reached k, we transfer 
        the indexed value over to the array and mark it as checked.*/
        for (int i = 0; i < n; i++){

            int count = 0;
            while (true){

                if (!vec[index].used){
                    count++;
                    //Are we done?
                    if (count >= k){
                        break;
                    }
                }
                //Increment index
                index++;
                //Make sure index is in range
                if (index >= vec.size()){
                    index = 0;
                }

            }

            //At this point, the index should be on the number we wish to eliminate...
            ary[i] = vec[index].value;
            vec[index].used = true;
        }

        
        
        /*  *******Proposed Algorithm. Author: Jared Fowler ******** */
        if (true){
            //Get the log_2
            logV = log(n) / log(2);

            //Find the last element, the winner.
            last = 2 * (n - pow(2, logV)) + 1;  //Correct For ALL!!!

            //SECOND TO LAST
            second = last + pow(2, logV - 1);
            //If this value is over n, we do a different technique
            if (second > n)
                second = last - pow(2, logV);
            
            //THIRD TO LAST
            third = second + pow(2, logV - 1);
            //If this value is over n, we do a different technique
            if (third > n)
                third = second - pow(2, logV);
            //If the above value goes below or equal to zero, we do a different technique
            if (third <= 0)
                third = second + pow(2, logV - 2);
            
            
        }


        //Compare the brute force results to my algorithm
        if ((last == ary[n - 1]) && (second == ary[n - 2]) && (third == ary[n - 3]))
            pass++;
        else{//Keep a log of what values failed.
            failVec.push_back(n);
            fail++;
        }

    }


    //Print Results
    cout << "Pass: " << pass << endl;
    cout << "Fail: " << fail << endl;
    cout << endl << "FAILED: " << endl << endl;

    for (size_t index = 0; index < failVec.size(); index++)
    {
        cout << failVec[index] << " ";
    }

    cout << endl << endl;

    return 0;
}