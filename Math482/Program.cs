using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SortingAlgorithms
{
    class Program
    {
        
        
        static void Main(string[] args)
        {
            SortAlgorithm sa = new SortAlgorithm();

            sa.RunProgram();
      
	    }
    }

    class SortAlgorithm
    {
        //Variables-----------------------------------------------------------------------------------------
        private int comparisonCount;
        private int swapCount;
        private List<int> ary;
        private int length;


        //Methods-------------------------------------------------------------------------------------------
        //--Constructor
        public SortAlgorithm() { }
        //--------------------------------------------------------------------------------------------------
        //--Run Program
        public void RunProgram(){

            //Local Variable
            int choice = 0;

            //Upon start up, we need to get an initial array of comparible items
            this.ary = GetInput();

            /*
            //Main loop of the program
            while (true)
            {
                switch (choice)
                {
                    case 1:
                        this.ary = GetInput();
                        break;
                    case 2:
                        QuickSort();
                        break;
                    case 3:
                        return;
                    default:
                        //Do nothing
                }
            }
             * */

            QuickSort();

            //Present a table of options to the user
            Console.WriteLine("\n\n");
            printTable();



        }
        //--------------------------------------------------------------------------------------------------
        //--Get User Input for array to be sorted
        private List<int> GetInput()
        {
            //Local Variables
            string aryString = null;
            string[] aryString2 = null;
            List<int> ary = new List<int>();

            //We need the user to input an array of ints.
            Console.Write("Please input an array of integers (each number separated by a space) : ");
            aryString = Console.ReadLine();

            //We need to write these numbers to an array.
            //--Delete any white space that might exist at the end or start of the string
            aryString.Trim();
            //--Split the input string into separate elements
            aryString2 = aryString.Split(' ');
            //--Attempt to fill the array :: Elements get added to beggining of list, so we will work backwards
            for (int index = 0; index < aryString2.Length; index++)
            {
                ary.Add(Convert.ToInt32(aryString2.GetValue(index)));
            }
            
            //Save the length
            this.length = aryString2.Length;

            return ary;
        }
        //--------------------------------------------------------------------------------------------------
        //--Prints the main option table
        private void printTable()
        {
            Console.WriteLine("1. Input Array");
            Console.WriteLine("2. Quick Sort");
            Console.WriteLine("3. Quit");
        }
        //--------------------------------------------------------------------------------------------------
        //--QuickSort Option
        private void QuickSort()
        {
            //We need to reset the comparison and swap counts
            this.comparisonCount = 0;
            this.swapCount = 0;

            //Start Quick-Sort
            QS(this.ary, 0, ary.Count - 1);

            //Results
            Console.WriteLine("\n\nTotal Comparisons: {0}\nTotal Exchanges: {1}", this.comparisonCount, this.swapCount);

        }
        //--------------------------------------------------------------------------------------------------
        //--Quick Sort Algorithm Part 1
        private void QS(List<int> ary, int start, int end)
        {
            Console.WriteLine("\nArray:\n");
            //Print out entire array
            for (int index = 0; index < ary.Count; index++)
            {
                Console.Write("{0}, ", ary[index]);
            }
            Console.Write("\n\n");
            Console.WriteLine("Making call: QS(ary, {0}, {1})", start, end);
            Console.WriteLine("\nSub-Array:\n");
            //Print sub array
            for (int index = start; index <= end; index++)
            {
                Console.Write("{0}, ", ary[index]);
            }
            Console.Write("\n\n");

            if (start < end)
            {
                int pivot = Partition(ary, start, end);
                QS(ary, start, pivot - 1);
                QS(ary, pivot + 1, end);
            }    
        }
        //--Quick Sort Algorithm Part 2
        private int Partition(List<int> ary, int start, int end)
        {
            int x = (int)ary[end];
            int i = start - 1;
            int tempVal;

            for (int j = start; j < end; j++)
            {
                //Print Comparison and update count
                Console.WriteLine("comparison: {0} to {1}", (int)ary[j], x);
                comparisonCount++;

                if (ary[j] <= x)
                {
                    ++i;

                    //Print Exchange and update count
                    Console.WriteLine("exchange: {0} <-> {1}", (int)ary[i], (int)ary[j]);
                    swapCount++;

                    //Exchange values pointed to by i and j
                    tempVal = (int)ary[i];
                    ary[i] = ary[j];
                    ary[j] = tempVal;
                }
            }

            //Make final swap to place pivot in correct place (pivot was the last number of the sub array)
            Console.WriteLine("Final Pivot Swap: {0} <-> {1}", ary[i + 1], x);
            swapCount++;
            ary[end] = ary[i + 1];
            ary[i + 1] = x;

            return i + 1;
        }

    }


    
}
