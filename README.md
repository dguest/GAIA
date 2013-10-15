GAIA -- Generalized Algorithm for Identification Architectures
===========

A multi-class, generic Neural Network Package for Jet ID's at the ATLAS detector.

###Thin-Client installation

A thin-client library with a header-only implementation can be found in `include/JetTagger.h`. If you wish to simply use this header, copy it into any directory which would use it. To place it in a standard `#include` search path, type `sudo make header`.

###General Idea

GAIA is a next-generation neural network library designed for use with large datasets. A la Hinton's work on greedy training of neural networks, we implement a series of stacked autoencoders of arbitrary complexity. Each layer has a non-linear extraction of a new, lower dimensional basis of the features trained upon. 





