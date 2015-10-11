# calculator
simple expression calculator in c

A simple calculator for single digit numbers with +*-/ operations.

It has a read-eval-print-loop which parses infix notation to postfix notation and then evaluates the postfix using a stack machine.

In addition to the interactive repl mode, it can also be used in pipe mode, suppressing printing of banner and prompt.

For example
```
echo "(7+9*2)*(8-1)/(1+3-2)" | ./kalc
```
or
```
  $ ./kalc 
  Kim's calculator program. Copyright 2015 Kimberley Skelton
  kalc>(7+9*2)*(8-1)/(1+3-2)
  Postfix 792*+81-*13+2-/
  87.500000
```

