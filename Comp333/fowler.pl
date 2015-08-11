/* Created By: Jared Fowler.  Comp 333, Spring 2014. April 22, 2014  Project 6 */




/* Problem 1:  Family Tree Database:

This is composed of a databas list of family members. It
will list an immediate family line going back to my great Grandparents
on both my mother's and father's side. I will add immediate aunts and
uncles, but will limit my listing of cousins as I have over 40.

The only associations that will be given will be male, female, and
parent. I will assign definitions for ancestor, grandparent,
grandfather, grandmother, mother, father, aunt (including all
generations of aunts), uncle (including all generation of uncles) ,
cousin, sister, brother and descendant.


*/

/* "male" listed from top of tree to bottom */
male(bennette).

male(jared).
male(markF).
male(christopher).
male(jasonF).
male(garrett).
male(alex).
male(jasonG).
male(daniel).
male(sheldon).
male(hapR).
male(paige).
male(morgan).
male(cameron).

male(guy).
male(rex).
male(max).
male(aaron).
male(steven).
male(markG).
male(michael).
male(hapE).

male(william).
male(robert).

male(williamL).
male(francis).
male(norman).
male(benjamin).

/* "female" listed from top of tree to bottom */
female(jocelyn).
female(samantha).
female(kaitlin).
female(carrieG).
female(monica).
female(beth).
female(nicole).
female(rose).
female(emma).
female(aubry).

female(rachael).
female(deborah).
female(deniz).
female(jennifer).
female(mariel).
female(carrieF).

female(arlene).
female(elaine).

female(ruby).
female(celia).
female(freeda).
female(anita).

/* "parent" listed from top of tree to bottom */
parent(jocelyn,bennette).
parent(markF,bennette).

parent(guy,jared).
parent(guy,markF).
parent(rachael,jared).
parent(rachael,markF).
parent(max,jasonF).
parent(max,christopher).
parent(rex,samantha).
parent(rex,kaitlin).
parent(rex,garrett).
parent(carrieF,samantha).
parent(carrieF,kaitlin).
parent(carrieF,garrett).
parent(dapE,sheldon).
parent(deborah,sheldon).
parent(hapE,hapR).
parent(deborah,hapR).
parent(hapE,paige).
parent(deborah,paige).
parent(hapE,morgan).
parent(deborah,morgan).
parent(hapE,aubry).
parent(hapE,cameron).
parent(deborah,aubry).
parent(deborah,cameron).
parent(markG,beth).
parent(markG,nicole).
parent(markG,rose).
parent(markG,emma).
parent(jennifer,beth).
parent(jennifer,nicole).
parent(jennifer,rose).
parent(jennifer,emma).
parent(michael,jasonG).
parent(michael,monica).
parent(michael,alex).
parent(michael,carrieG).
parent(mariel,jasonG).
parent(mariel,monica).
parent(mariel,alex).
parent(mariel,carrieG).
parent(steven,daniel).
parent(steven,christina).
parent(deniz,daniel).
parent(deniz,christina).

parent(robert,guy).
parent(robert,rex).
parent(robert,max).
parent(elaine,guy).
parent(elaine,max).
parent(elaine,rex).
parent(william,aaron).
parent(william,steven).
parent(william,markG).
parent(william,michael).
parent(william,rachael).
parent(william,deborah).
parent(arlene,aaron).
parent(arlene,steven).
parent(arlene,markG).
parent(arlene,michael).
parent(arlene,rachael).
parent(arlene,deborah).

parent(francis,robert).
parent(celia,robert).
parent(williamL,elaine).
parent(ruby,elaine).
parent(anita,william).
parent(norman,william).
parent(freeda,arlene).
parent(benjamin,arlene).


/* Other Definitions */
mother(A,B) :- female(A), parent(A,B).
father(A,B) :- male(A), parent(A,B).

grandparent(A,B) :- parent(A,C),parent(C,B).
grandmother(A,B) :- female(A), grandparent(A,B).
grandfather(A,B) :- male(A), grandparent(A,B).

ancestor(A,B)   :- parent(A,B).
ancestor(A,B)   :- parent(A,C),ancestor(C,B).
descendant(A,B) :- ancestor(B,A).

brother(A,B) :- male(A), mother(C,A), mother(C,B), not(self(A,B)).
sister(A,B)  :- female(A), mother(C,A), mother(C,B), not(self(A,B)).

cousin(A,B) :- grandparent(C,A), grandparent(C,B), not(self(A,B)), not(sibling(A,B)).

aunt(A,B)  :- female(A), sister(A,C), parent(C,B).
aunt(A,B)  :- female(A), couple(A,C), uncle(C,B).
uncle(A,B) :- male(A), brother(A,C), parent(C,B).
uncle(A,B) :- male(A), couple(A,C), aunt(C,B).

couple(A,B)  :- parent(A,C), parent(B,C).
self(A,B)    :- A = B.
sibling(A,B) :- brother(A,B).
sibling(A,B) :- sister(A,B).



/* Problem 2: isUnion and isIntersection

•	Define a Prolog predicate, isUnion, so that isUnion (X,Y,Z) is true if Z
is the union of X and Y.  Assume that X and Y are bound lists , neither of which
has any duplicates. The values in Z may be in any order; there should be no
duplicates in Z. Do not use list predicates except member. This includes append.
Call isUnion recursively.

Usage:  isUnion( [a,c], [q,w,e], [a,c,q,w,e] )    is true
	isUnion( [ 1,7,9], [ 4,7,2], Z)  returns Z = [1,7,9,4,2]
	or same list with different order
	isUnion( [ a,y,r,t], [a, b] , [a,y,r,t,a,b]) is false.


•	Define a Prolog predicate, isIntersection, so that isIntersection (X,Y,Z)
is true if Z is the intersection of X and Y.  Assume that X and Y are bound lists,
neither of which has any duplicates. The values in Z may be in any order; there
should be no duplicates in Z. Do not use list predicates except member. This
includes append. Call isIntersection recursively.

Usage:  isIntersection( [a,c], [q,w,e], [] ) is true
	isIntersection( [1,5,3,9], [],[])  is true
	isIntersection( [ 1,7,9, 3], [ 3, 4,7,2], Z)  returns Z = [3,7]   or [7,3]
	isIntersection( [ a,y,r,t], [y, b] , [y]) is true

*/

/*Base case. The union of the empty list and B is B*/
isUnion([],B,B).
/*If first element in list A is in B, then skip it and call isUnion.  This will
eliminate duplicates and will auto-fix when we reach the base case.*/
isUnion([H1|T1],B,C) :- member(H1,B), isUnion(T1,B,C).
/*Given the above case is false, that is, B does not contain the first element in A, we
must declare that element (H1) to be in list C. We then recursive call.*/
isUnion([H1|T1],B,[H1|T2]) :- isUnion(T1,B,T2).

/*Base case.  The intersection of empty list and B(_) is empty list*/
isIntersection([],_,[]).
/*If first element of list A is a member of list B, it better be a memeber of list C*/
isIntersection([H1|T1],B,[H1|T2]) :- member(H1,B), isIntersection(T1,B,T2).
/*Given the above case is false, that is, B does not contain the first element in A, we
must declare that list C does not have that element!*/
isIntersection([_|T1],B,C) :- isIntersection(T1,B,C).





/*Problem 3:  Little Sudoku Problem*/

sudoku(X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11,X12,X13,X14,X15,X16,X17,X18,X19,X20,X21,X22,X23,X24,X25,X26) :-

/*We start by listing the permutations (we list these according to how many variables need to be found.  This will greatly reduce the number of permutations that need to be done! */

	permutation([2,3,4],[X18,X22,X23]),
	permutation([1,4,5],[X4,X5,X8]),
	permutation([1,3,4,6],[X14,X15,X16,X17]),
    permutation([1,2,3,6],[X2,X6,X15,X23]),
    permutation([3,4,5],[X6,X7,X8]),
    permutation([1,2,3,4],[X18,X19,X20,X21]),
	permutation([2,3,4,5],[X1,X9,X14,X22]),
	permutation([1,2,4,6],[X8,X11,X19,X24]),
	permutation([1,3,4,5],[X4,X12,X20,X25]),
	permutation([1,3,4,6],[X11,X12,X13,X17]),

       %the rest


	permutation([1,2,4,5,6],[X1,X2,X3,X4,X5]),
	permutation([1,2,3,5,6],[X9,X10,X11,X12,X13]),
    permutation([2,3,4,5,6],[X22,X23,X24,X25,X26]),
    permutation([1,3,4,5,6],[X5,X13,X17,X21,X26]),
	permutation([2,3,4,5,6],[X3,X7,X10,X16,X18]),
	permutation([2,3,4,5,6],[X1,X2,X3,X6,X7]),
	permutation([1,2,3,5,6],[X9,X10,X14,X15,X16]),
	permutation([1,2,3,4,5,6],[X19,X20,X21,X24,X25,X26])
.















