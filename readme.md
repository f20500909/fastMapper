
## fastMapper
    a mapper gengator

## How to use
 in cpp

 in python

 pip install fastmapper
 "
 "

 in lua

## What to do next ?
 it not only support biamap . it will support vector map like svg  i am doing this!


## How it work ?


## reference




Algorithm
Read the input bitmap and count NxN patterns.
(optional) Augment pattern data with rotations and reflections.
Create an array with the dimensions of the output (called "wave" in the source). Each element of this array represents a state of an NxN region in the output. A state of an NxN region is a superposition of NxN patterns of the input with boolean coefficients (so a state of a pixel in the output is a superposition of input colors with real coefficients). False coefficient means that the corresponding pattern is forbidden, true coefficient means that the corresponding pattern is not yet forbidden.
Initialize the wave in the completely unobserved state, i.e. with all the boolean coefficients being true.
Repeat the following steps:
Observation:
Find a wave element with the minimal nonzero entropy. If there is no such elements (if all elements have zero or undefined entropy) then break the cycle (4) and go to step (5).
Collapse this element into a definite state according to its coefficients and the distribution of NxN patterns in the input.
Propagation: propagate information gained on the previous observation step.
By now all the wave elements are either in a completely observed state (all the coefficients except one being zero) or in the contradictory state (all the coefficients being zero). In the first case return the output. In the second case finish the work without returning anything.