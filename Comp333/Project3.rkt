#lang racket

;Created By Jared Fowler
;Comp 333 Project 3


;Problem 1

(define fsm1
  (lambda (x state)
    (cond
      
      ((= state 0)
       (cond
         ((null? x) #f)  ;Ensure that we are not working with the empty string
         ((= (car x) 0) (fsm1 (cdr x) 0))
         ((= (car x) 1) (fsm1 (cdr x) 1))
         (else #f)
       ))
       
       ((= state 1)
        (cond
         ((null? x) #f)
         ((= (car x) 0) (fsm1 (cdr x) 1))
         ((= (car x) 1) (fsm1 (cdr x) 2))
         (else #f)
          
       ))
       
       ((= state 2)
        (cond
         ((null? x) #t) ;The null string is ok at this point
         ((= (car x) 0) (fsm1 (cdr x) 2))
         ;((= (car x) 1) (fsm1 (cdr x) 1)) This is no longer true
         (else #f)
       
        ))
       
       )
    )
  )


;Problem 2 part b

(define fsmFP
  (lambda (x state)
    (cond
      
      ((= state 0)
       (cond
         ((null? x) #f)
         ((char=? (car x) #\+) (fsmFP (cdr x) 1)) ; if '+' continue to state 1
         ((char=? (car x) #\-) (fsmFP (cdr x) 1)) ; if '-' continue to state 1
         ((char-numeric? (car x)) (fsmFP (cdr x) 2)) ; if number, continue to state 2
         (else #f)
       ))
      
      ((= state 1)
       (cond
         ((null? x) #f)
         ((char-numeric? (car x)) (fsmFP (cdr x) 2)) ; if number, continue to state 2
         (else #f)
       ))
      
      ((= state 2)
       (cond
         ((null? x) #f)
         ((char=? (car x) #\.) (fsmFP (cdr x) 3)) ; if '.' continue to state 3
         (else #f)
       ))
      
      ((= state 3)
       (cond
         ((null? x) #f)
         ((char-numeric? (car x)) (fsmFP (cdr x) 4)) ; if number, continue to state 4
         (else #f)
       ))
       
       ((= state 4)
       (cond
         ((null? x) #t) ;if we end here, we have a valid entry
         ((char-numeric? (car x)) (fsmFP (cdr x) 4)) ; if number, loop to state 4
         ((char=? (car x) #\e) (fsmFP (cdr x) 5)) ; if 'e' continue to state 5
         (else #f)
       ))
       
       ((= state 5)
       (cond
         ((null? x) #f)
         ((char=? (car x) #\+) (fsmFP (cdr x) 6)) ; if '+' continue to state 6
         ((char=? (car x) #\-) (fsmFP (cdr x) 6)) ; if '-' continue to state 6
         (else #f)
       ))
       
       ((= state 6)
       (cond
         ((null? x) #f)
         ((char-numeric? (car x)) (fsmFP (cdr x) 7)) ; if number, continue to state 7
         (else #f)
       ))
       
       ((= state 7)
       (cond
         ((null? x) #t)
         ((char-numeric? (car x)) (fsmFP (cdr x) 8)) ; if number, continue to state 8
         (else #f)
       ))
       
       ((= state 8)
       (cond
         ((null? x) #t)
         ((char-numeric? (car x)) (fsmFP (cdr x) 9)) ; if number, continue to state 9
         (else #f)
       ))
       
       ((= state 9)
       (cond
         ((null? x) #t)
         (else #f)
       ))
       
       )
    )
  )

;Problem 2 part c

(define floatingPt?
  (lambda (s)
    (fsmFP (string->list s) 0))) 