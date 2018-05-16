# CRDT Examples
Examples of CRDTs implementations in C++.
These are for educational purpose only and are not designed to work in real world.
(Ex: use fixed number of process, fake timestamp).
They may be however use as a starting point or reference.


# Features

## CvRDT (State-based)
- GCounterInt: Grow-only Integer Counter
- PNCounterInt: Increment / Decrement Integer Counter
- GVectorInt: Grow-only Integer Vector
- PNVectorInt: Increment / Decrement Integer Vector
- GSetInt: Grow-only Integer set
- USetInt: Grow-only Integet 2P-Set
- LWWRegister: Last-Write-Wins Register

## CmRDT (Operation-based)
- GCounterInt: Grow-only Integer Counter


# Operations

## State-based object (CvRDT)
    Convergent Replicated Data Types (CvRDT)
    State CvRDT is defined by (S s0 q u m)
        S   -> Global State
        s0  -> State at beginning
        q   -> Query method
        u   -> Update method
        m   -> Merge method

## Operation-based object (CmRDT)
    Commutative Replicated Data Types (CmRDT)
    Operation CmRDT is defined by (S s0 q t u P)
        S   -> Global State
        s0  -> State at beginning
        q   -> Query method
        t   -> Side-effect-free prepare update method
        u   -> Effect update method (downstream)
        P   -> Delivery relation P for communication protocol


# Papers and resources
- Conflict-free Replicated Data Types (https://pages.lip6.fr/Marc.Shapiro/papers/CRDTs_SSS-2011.pdf)
- A comprehensive study of Convergent and Commutative Replicated Data Types (https://pages.lip6.fr/Marc.Shapiro/papers/Comprehensive-CRDTs-RR7506-2011-01.pdf)
