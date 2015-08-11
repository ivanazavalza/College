/*
 * Created By: Jared Fowler
 * Comp 333  Spring 2014
 * Project #5, Problem 3

	
	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	$ Write a generic class called MyList that $
	$ behaves similarly to Scheme Lists.       $
	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
 */



package comp333Proj5Problem3;

import java.util.*;

public class  MyList<E>
{
   private ArrayList<E> theList;

   public MyList( )
   { //create an empty list
	   theList = new ArrayList<E>();
   }
	  
   public MyList( ArrayList<E> data)
   { 
	   theList = data;
   }
   
   public int length()
   { //number of items on list
	   return theList.size();
   }

   public boolean empty( )
   {  
	   return theList.isEmpty();
   }
   
   public E car ()
   {
	   //Note: This is poorly done by java.  Because
	   //we are working with generic, we have a 
	   //harder time cloning.  There might arise an
	   //logic error if we later modify the returned
	   //object!
	   return theList.get(0);
   }
   
   public MyList<E> cdr()
   { //leaves this unchanged
	   //create a new list that contains the cdr
	   ArrayList<E> list = new ArrayList<E>();
	   for(int index = 1; index < theList.size(); index++){
		   list.add(theList.get(index));
	   }
	   //return the cdr in a MyList object
	   return new MyList<E>(list);
	   
   }
   
   public MyList<E> cons( E x)
   {  //leaves this unchanged  
	   //create a new list that contains the cons
	   ArrayList<E> list = new ArrayList<E>();
	   //Add the new first element
	   list.add(x);
	   //Add the rest
	   for(int index = 0; index < theList.size(); index++){
		   list.add(theList.get(index));
	   }
	   //return the cons in a MyList object
	   return new MyList<E>(list);
   }
   
   public String toString()
   { //prints contents of list 
	   String s = new String();
	   s = s.concat("'( ");
	   for(int index = 0; index < theList.size(); index++){
		   s = s.concat(theList.get(index).toString() + " ");
	   }
	   s = s.concat(")");
	   return s;
   }
   
   public boolean member(E x)
  {
	 //must be recursive method
       //use equals to test for equality
       //Use only car, cdr and cons to write this method
	   if(this.theList.size() == 0)
		   return false;
	   
	   else if(x.equals(this.car()))
		   return true;
	   
	   /*
	    * The following is intersting.  We in
	    * fact create another object in order to
	    * evaluate the expression! Object Stacking!
	    */
	   else
		   return this.cdr().member(x);
	   
   }
   
   
   public static void main( String[] args)
   {
      //Put test program here. Systematically test Strings, Integers, Objects  
	   
	   
	   //STRINGS---------------------------
	   System.out.println("STRINGS:");
       MyList<String> aList = new MyList<String>();
       MyList<String> bList = new MyList<String>();
       
       System.out.println(aList);
       aList = aList.cons("string1"); 
       aList = aList.cons("string2").cons("string3").cons("string4");
       bList = aList.cdr();
       System.out.println(aList + " "  + bList);
       String item = "string1";
       System.out.println( item + " is in list? "  + (aList.member(item)));  
       item = "string7";
       System.out.println( item + " is in list? "  + (aList.member(item)));  
       while( !aList.empty())
       {
          System.out.print(aList.car()+ " / " );
          aList = aList.cdr();
          System.out.println(aList);
 
       }
       System.out.println();
       System.out.println();
       System.out.println();
       
       
       //INTEGERS---------------------------
       System.out.println("INTEGERS");
       MyList<Integer> cList = new MyList<Integer>();
       MyList<Integer> dList = new MyList<Integer>();
       
       System.out.println(cList);
       cList = cList.cons(25); 
       cList = cList.cons(36).cons(66).cons(11);
       dList = cList.cdr();
       System.out.println(cList + " "  + dList);
       Integer item2 = 25;
       System.out.println( item2 + " is in list? "  + (cList.member(item2)));  
       item2 = 65;
       System.out.println( item2 + " is in list? "  + (cList.member(item2)));  
       while( !cList.empty())
       {
          System.out.print(cList.car()+ " / " );
          cList = cList.cdr();
          System.out.println(cList);
 
       }
       System.out.println();
       System.out.println();
       System.out.println();
       
       
     //OBJECTS---------------------------
       System.out.println("OBJECTS");
       MyList<Object> eList = new MyList<Object>();
       MyList<Object> fList = new MyList<Object>();
       
       System.out.println(eList);
       eList = eList.cons("HELLO"); 
       eList = eList.cons(36).cons(66.00).cons(11);
       fList = fList.cdr();
       System.out.println(eList + " "  + fList);
       Object item3 = 36;
       System.out.println( item3 + " is in list? "  + (eList.member(item3)));  
       item3 = 66;
       System.out.println( item3 + " is in list? "  + (eList.member(item3)));  
       while( !eList.empty())
       {
          System.out.print(eList.car()+ " / " );
          eList = eList.cdr();
          System.out.println(eList);
 
       }
       System.out.println();
       
       
       
       
       
   }
   
}  
