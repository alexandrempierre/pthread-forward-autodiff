# pthread-forward-autodiff

Implementation of Forward Automatic Differentiation with multiple threads. I'm using C99 implementation of complex numbers to calculate the derivative of real functions with multiple inputs and multiple outputs.

### <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/1/17/Warning.svg/1200px-Warning.svg.png" width=20px> Warning: this program does not calculate derivatives of complex functions! Complex numbers here are only a means to the end of getting derivatives of real functions.

## Technologies used

* C programming language (more specifically C99)
* POSIX threads
* GDB
* GNU Make

All the compiling and running this program have been done in Windows 10 with WSL. _I hope someday I get my Arch Linux back running..._

## How to use

### Compiling

Just hitting `make all` on the keyboard will probably do the job.

### Running

This program needs two command line arguments, in order: the number of threads and the dimension of the funciton domain. And it gets the point in which the function and the derivative will be evaluated by `scanf`s so you should supply as many numbers as the function domain deems necessary during the runtime of the program.

### Custom functions

It is possible to know how to it only by reading the comments in `function.c` file. But you just need to change the contents of function `f` in `function.c` without changing its prototype or return and adapt `calculate_codomain_dim` to your needs, but don't change its prototype.

By prototype I mean the name of the function, the type, order and name of its arguments and its return type;

## Future

I intend to write more functions (to derivate) and maybe a Python script to generate random entries but I don't have many plans other than that.