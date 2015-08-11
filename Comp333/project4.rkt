#lang racket

;Created By: Jared Fowler
;comp 333 Spring 2014
;Project 4


;$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
;------------------------Problem 1---------------------------------
;$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

;(1)---------------------------------------------------------------
;Function "(contains-digit! s)" returns true if a string contains a digit
;else, it returns false

;Psudocode
;Function has a string argument
;string->list null? return false
;first element of string->list number? return true
;else contains-digit? (string - first elememt)

(define contains-digit?
  (lambda (str)
    (cond
      ((null? (string->list str)) #f)
      ((char-numeric? (car (string->list str))) #t)
      (else (contains-digit? (list->string (cdr (string->list str)))))
    )   
  )
)

;(2)---------------------------------------------------------------
;Function "(getStringsWithDigits lst)" returns list of strings in lst that contain
;at least one digit

;Psudocode
;Function has a list of strings argument
;filter each string with function contains-digit?

(define getStringsWithDigits
  (lambda (lst)
    (filter contains-digit? lst)
   )
)


;$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
;------------------------Problem 2---------------------------------
;$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

;(1)---------------------------------------------------------------
;Function "(prime? n)" returns true if integer n is prime, else, returns false

;We determine if the number is 1, 2, or divisible by 2
;If none of these is the case, we call our prime-helper function

(define prime?
  (lambda (n)
    (cond
      ((null? n) #f)
      ((equal? n 1) #f)
      ((equal? n 2) #t)
      ((equal? 0 (modulo n 2)) #f)
      (else (prime-helper 3 n))
    )
  )
)

;Function "(prime-helper x n)" return false if x divides n, else
;recursively calls itself until we reach the square root of n

(define prime-helper
  (lambda (x n)
    (cond
      ((equal? x n) #t)
      ((equal? 0 (modulo n x)) #f)
      ((< x (sqrt n)) (prime-helper (+ x 2) n))
      (else #t)
    )
  )
)

;(2)---------------------------------------------------------------
;Function "(range n m)" returns a list of numbers (n n+1 n+2 ... m) if n<=m, else
;returns the empty list

;Function takes in two integers
;We test if n > m, if so we return the empty list
;else, we add n to the list of range((n+1) m)

(define range
  (lambda (n m)
    (cond
      ((> n m) '())
      (else (cons n (range (+ n 1) m)))
    )
  )
)

;(3)---------------------------------------------------------------
;A single expression that could solve the problem of the sort "how many primes exist between 2 and 1000?"
;Plan:
;     1. We need to get a list of numbers between 2 and 1000. For this we can use our range function
;     2. Take this list and use a filter with the argument condition prime? This will return a list of 
;        the prime numbers that exist between 2 and 1000.
;     3. We can now use the function, length to determine how long the list is.

; (length (filter prime? (range 2 1000)))

;(4)---------------------------------------------------------------
;A single expression that could solve the problem of the sort "sum of the squares of all primes between 10 and 1000"
;Plan:
;     1. We need to get a list of numbers between 10 and 1000. For this we can use our range function
;     2. Take this list and use a filter with the argument condition prime? This will return a list of 
;        the prime numbers that exist between 2 and 1000.
;     3. We need to square each element in the list. For this we can use the map function with the fucntion sqr
;     4. We finally need to add up all the elements, which we can do using the foldl function with the initial value of 0

; (foldl + 0 (map sqr (filter prime? (range 10 1000))))


;$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
;------------------------Problem 3---------------------------------
;$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

;(1)---------------------------------------------------------------
;Function "(rowsum2D k x)" finds the sum of the k'th row of matrix x
;Returns an error if k is out of range

;Plan:
;     1. First, check to make sure that k is in range
;     2. We need to get the k'th row of the matrix. Use the nth-element function to do this
;     3. We can now use the sum by using the foldl function with the + operator and initial state of 0

(define rowsum2D
  (lambda (k x)
    (cond
      ((> k (length x)) (error "Out of Range!"))
      ((< k 1) (error "Out of Range!"))
      (else (foldl + 0 (nth-element k x)))
    )
  )
)

;Function (nth-element n lst) returns a list from a list of lists at the n'th element
(define nth-element
  (lambda ( n lst)
    (cond
      ( ( = n 1) (car lst))
      ( else ( nth-element ( - n 1) (cdr lst)))
    )
  )
)

;(2)---------------------------------------------------------------
;Function "(colsum2D k x)" finds the sum of the k'th column in matrix x
;Returns an error message if out of range

;Plan:
;     1. First make sure that k is in range. We can do this by using the length
;        function on the car of the list
;     2. We need to step through every individual list, pull out the kth element, and add it to another list.
;        To do this we can use the map function with the nth-element function and anomynous lambda function
;     3. Now that we have a list of all numbers in the k'th column, we use foldl to 
;        add them together.

(define colsum2D
  (lambda (k x)
    (cond
      ((< k 1) (error "Out of Range!"))
      ((> k (length (car x))) (error "Out of Range!"))
      (else (foldl + 0 (map (lambda (y) (nth-element k y)) x)))
    )
  )
)

;(3)---------------------------------------------------------------
;Function "(diagsum k x)" finds the diagnol of a square matrix
;Returns an error message if matrix x is not square

;Plan:
;     1. Determine if x is a square matrix. Compare the length of the list
;        along with the length of the car of the list of lists.
;     2. Use diagsum-helper to return list of the diagnol
;     3. Use foldl function to obtain the sum

(define diagsum
  (lambda (k x)
    (cond
      ((not (equal? (length x) (length (car x)))) (error "Matrix Not Square!"))
      (else (foldl + 0 (diagsum-helper 1 x)))
    )
  )
)

;Function diagsum-helper takes in a list of lists and returns the diagnol of that list in a list

(define diagsum-helper
  (lambda (k x)
    (cond
      ((equal? 0 (length x)) '())
      (else (cons (nth-element k (car x)) (diagsum-helper (+ k 1) (cdr x))))
    )
  )
)