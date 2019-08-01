# curl circuit-solver

This is a simple bruteforce algorithm for finding efficient boolean circuits evaluating `curl-s2` building block of IOTA Curl s-box.

At the moment the best solution found is:
`(Xor AH (And AL (Not BL)),Xor AL (And BH (Xor AH (And AL (Not BL)))))`.
Note that left ("low" bit) side is used in the right ("high" bit) side.
Note that `And x (Not y)` is usually one processor instruction `andn y x`.

# Build and Run

```
stack build
stack exec -- curl-exe
```

