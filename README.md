# fl_library
fuzzy logic library

A fuzzy logic library libfl in C language to be used in such domains as

    research
    prototyping
    education
    productions

By defining inputs, outputs variables, their own subsets which represents the domain where values operates, and by defining rules inferences to trigger outputs.

--

All files are commented through Doxygen format, run :

> doxygen doxyconf 

to generate documentation, 

see doxyconf to configure output of doxygen

and doc.dox file ( main page of explaination made by doxygen) 

--

Using Makefile for unix system , run :

> make

it does generate libfl.a for a static library
to use it, compile your own code with -lfl -lmath
