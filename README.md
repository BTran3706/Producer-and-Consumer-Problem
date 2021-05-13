# Producer-and-Consumer-Problem

Simulation of a producer and consumer problem using multithreading and semaphores.  

- Lucy and Ethel (consumers) work for the candy factory Mizzo that produces two types of candy: crunchy frog bites and everlasting escargot suckers (producers).  
- No more than 10 candies can be on the conveyer belt at a time.  
- No more than 3 frog bites can be on the conveyer belt at a time.  
- Candies are taken off the conveyer belt in the order they are produced.
- Optional command line arguments: L (Lucy), E (Ethel), f (frog bites), and/or e (escargot) followed by a number is the time in milliseconds it takes the specified consumer to put
a candy in a box and the time it takes to produce the specified candy. The default time is 0 if no arguments.
