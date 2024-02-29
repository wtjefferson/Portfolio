; Check if the current roll is a strike
(define (is-strike roll)
  (= roll 10))
; Check if the current and next roll combined make a spare
(define (is-spare roll1 roll2)
  (= (+ roll1 roll2) 10))
; Recursively caculate total score based on rolls, frame, current total, and index
(define (calc-score rolls frame total-score roll-index)
  (cond
  ; If all frames are done, return the total score
    ((= frame 10) total-score)
    ; If the current roll is a strike, add the bonus of the next two rolls
    ((is-strike (car rolls)) ; strike
     (calc-score (cdr rolls)
                 (+ frame 1)
                 (+ total-score 10 (cadr rolls) (caddr rolls))
                 (+ roll-index 1)))
    ; If the current and next roll is a spare, add the bonus of the next roll
    ((is-spare (car rolls) (cadr rolls)) ; spare
     (calc-score (cddr rolls)
                 (+ frame 1)
                 (+ total-score 10 (caddr rolls))
                 (+ roll-index 2)))
    (else
    ; If it's neither a strike nor spare, just add the value of the rolls to total
     (calc-score (cddr rolls)
                 (+ frame 1)
                 (+ total-score (car rolls) (cadr rolls))
                 (+ roll-index 2)))))

(define (bowling rolls)
  (calc-score rolls 0 0 0))


;; Test cases
;; I used racket to compile and test, so that's why the println statements are still here
;; Racket was the closest compiler I could get to predictably interpret my code
(println (bowling '(0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0)))  ;; Should print 0
(println (bowling '(10 10 10 10 10 10 10 10 10 10 10 10)))  ;; Should print 300
(println (bowling '(5 3 5 3 3 5 5 3 3 5 3 5 6 2 2 6 7 1 1 7))) ;; Should print 80
(println (bowling '(5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5)))  ;; Should print 150
(println (bowling '(4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4)))  ;; Should print 80
(println (bowling '(10 9 1 5 5 7 2 10 10 10 9 0 8 2 9 1 10)))  ;; Should print 187
(displayln "")
;; Test cases that do not work yet
(println (bowling '(1 2 3 4 5 4 6 3 2 7 8 1 9 0 6 3 5 2 9 1 10)))  ;; Should print 93 (91)
(println (bowling '(1 2 3 4 5 4 6 3 2 7 8 1 9 0 6 3 5 2 10 10 10)))  ;; Should print 112 (101)
(println (bowling '(1 2 3 4 5 4 6 3 2 7 8 1 9 0 6 3 5 2 10 5 5)))  ;; Should print 104 (91)
