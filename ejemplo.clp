(defglobal
?*num* = 45)

(defrule adivinar
=>
(assert (lee)))

(defrule lee
?h <- (lee)
=>
(retract ?h)
(printout t "Ingresa un numero: ")
(assert (numero_in (read))))

(defrule num_bajo
?h <- (numero_in ?n&:(< ?n ?*num*))
=>
(retract ?h)
(printout t ?n " es bajo" crlf)
(assert (lee)))

(defrule num_alto
?h <- (numero_in ?n&:(> ?n ?*num*))
=>
(retract ?h)
(printout t ?n " es alto" crlf)
(assert (lee)))

(defrule num_correcto
?h <- (numero_in ?n&:(= ?n ?*num*))
=>
(retract ?h)
(printout t ?n " es el numero correcto" crlf))