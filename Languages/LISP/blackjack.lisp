; Handle possible values of aces in the hand
; Ace is 11 if possible and 1 otherwise
(define (handle-aces accum ace-count)
  (if (and (> ace-count 0) (<= (+ accum 10) 21))
      (handle-aces (+ accum 10) (- ace-count 1))
      accum))
; Accumulate card values according based on number, face, or ace
(define (score-pass cards accum ace-count)
  (cond
  ; If there's no more cards we handle the aces and return the total
    ((null? cards) (handle-aces accum ace-count))
    ; If the card is an ace we treat is as 1 for now
    ((= (car cards) 1) (score-pass (cdr cards) (+ accum 1) (+ ace-count 1)))
    ; If it's a face card we treat it as 10
    ((> (car cards) 10) (score-pass (cdr cards) (+ accum 10) ace-count))
    ; Every other time we just count the value of the card
    (else (score-pass (cdr cards) (+ accum (car cards)) ace-count))))

(define (score-hand hand)
  (score-pass hand 0 0))

; Test cases
(println (score-hand '(1 11))) ; Should print 21
(println (score-hand '(2 3)))       ; Should print 5
(println (score-hand '(12 13)))    ; Should print 20
(println (score-hand '(10 8 5)))   ; Should print 23
(println (score-hand '(1 10 8 5))) ; Should print 24
(println (score-hand '(2 3 4 5 6 7 8))) ; Should print 35
(println (score-hand '(1 1 10 10 9))) ; Should print 31
(println (score-hand '(1 1)))  ; Should print 12
(println (score-hand '(1 1 1 13))) ; Should print 13
(println (score-hand '(1 7 1)))    ; Should print 19
(println (score-hand '(1 8 1)))    ; Should print 20
(println (score-hand '(1 1 1 1 1 1 1 1 1 1 1))) ; Should print 21
(println (score-hand '(1 10 10)))  ; Should print 21
(println (score-hand '(1 2 3 1 4 1 5))) ; Should print 21