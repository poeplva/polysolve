You can compile the program with `g++ polysolve.cpp -o polysolve`. You can then
run it with `./polysolve`. Running the program with no arguments will output a
usage message on how to use the program.

If you run the program with the example found in the project description
`./polysolve 2 2 -7 1 -7 1.5 1.8 0.001`, we get the output
```
The root found by using the bisection algorithm is 1.66941. It took 12 iterations for the algorithm to conclude.
The root found by using the secant algorithm is 1.66941. It took 4 iterations for the algorithm to conclude.
The root found by using the hybrid algorithm is 1.66941. It took 5 iterations for the algorithm to conclude.
```

The program will output an error message when the first two guesses provided in
the arguments do not have opposite signs when evaluated at the polynomial. For
example if you run `./polysolve 2 2 -7 1 -7 10 11 0.001`, you get the output
```
terminate called after throwing an instance of 'std::invalid_argument'
  what():  Bisection method only applicable when the polynomial changes sign between the two guesses!
Aborted (core dumped)
```

One can try many different polynomials and see that the bisection algorithm
almost always takes more iterations than the secant algorithm. This is
consistent with the fact that the bisecton algorithm only converges linearly,
while the secant algorithm converges with exponent 1.68... > 1. It is, however,
possible that the secant algorithm won't even converge while the bisection
algorithm converges surely. To see an example to this, run `./polysolve 3 4 4 43 2 2 -7 1 -7 -1 1 0.001`,
you will get the output
```
The root found by using the bisection algorithm is 0.706177. It took 14 iterations for the algorithm to conclude.
The algorithm did not converge and hit the maximum number of iterations (10000000).
The root found by using the hybrid algorithm is 0.706162. It took 9 iterations for the algorithm to conclude.
```
You see that the secant algorithm did not even converge. We see that
0.074523 was in between the first two guesses, and it was a point where the
derivative of the polynomial vanished. We knew that if the derivative vanishes
between the first two guesses, it is possible that the secant algorithm won't
converge.
