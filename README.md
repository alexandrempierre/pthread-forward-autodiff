# pthread-forward-autodiff

Implementation of Forward Automatic Differentiation with multiple threads. I'm using C99 implementation of complex numbers to calculate the derivative of real functions with multiple inputs and multiple outputs.

### <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/1/17/Warning.svg/1200px-Warning.svg.png" width=20px> Warning: this program does not calculate derivatives of complex functions! Complex numbers here are only a means to the end of getting derivatives of real functions.

### <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/1/17/Warning.svg/1200px-Warning.svg.png" width=20px> Warning: the user of this program should never be dealing directly with Complex numbers, those are just a step in the path to the derivative. So give real numbers to the `scanf`s

## Technologies used

* C programming language (more specifically C99)
* POSIX threads
* GDB
* GNU Make

All the compiling and running this program have been done in Windows 10 with WSL. _I hope someday I get my Arch Linux back running..._

## Why can I do that?

I'll skip a lot of details, but if anyone ever bother to ask something in the issues I'll try to clarify.

We can define a new kind of number in the format `a + bε` with arithmetic rules that resemble the complex numbers in many ways:

* Addition: `(a + bε) + (c + dε) = (a+c) + (b+d)ε`
* Subtraction: `(a + bε) - (c + dε) = (a-c) + (b-d)ε`
* _Multiplication_: `(a + bε) * (c + dε) = (a*c) + (a*d + b*c)ε`
* _Division_: `(a + bε) / (c + dε) = (a/c) + (b/c - a*d/c^2)ε`

If you remember complex number arithmetic you may have noticed that multiplication and division are slightly different from those of the complex arithmetics. This is because `ε^2 = 0` unlike the imaginary unit `i` for which `i^2 = -1`.

### But the program uses complex numbers...

Yes, and floating point arithmetic too. I'm defining my numbers as `a + bεi`. And I make my `ε` small enough to have the cancellation of floating point addition make my equations hold. 

For an example, suppose that I want to represent `1 + ε` with complex numbers I need `abs(ε) < 1e-8` where `abs` is the absolute value function. But why this value? Because in floating point arithmetic `(1 + 1e-16) - 1 = 0`, so if I have `1 + εi` I don't need to bother with some negative numbers changing the correct value of the real part of my number. 

But most importantly of all and what made me use complex numbers: C doesn't have operator overloading and all the complex number arithmetic is already implemented in C99.

### Back to the ε and derivatives

Suppose I have an analytic function `f` and a dual number `x + ε` let's take a look at this function's Taylor Series around `x`:

`f(x + ε) = f(x)ε + f'(x)ε + f''(x)ε^2/2 + ...`

Remember that `ε^2 = 0`, it makes `ε^3 = 0` too and all that comes after that. This will make anything from `f''(x)ε^2/2` to the infinity equals zero, so the series now will look a lot more concise:

`f(x + ε) = f(x)ε + f'(x)ε`

That's why the `scanf`s need only real numbers, the program adds the `ε` where needed. And just evaluating the function at `x + ε` gives us `f(x)` and `f'(x)`. The complex numbers are very convenient here too: trigonometric, exponential and other good functions with which we usually work have very easy extension to the complex numbers. And most importantly those extensions are already implemented in C99. That's why `input_values` and `output_values` are complex variables even though I tell people that everything input have to be real.

### Greater dimensions

I won't make the explanation much longer (or explanatory): in greater dimensions you put `ε` in a direction and you get a directional derivative. 

## About performance

C99 complex numbers performance is not great. If you are reading this you should not expect great performance from this program just because it's C. But what is exemplified is one of the best use cases for forward automatic differentiation: I'm calculating the Jacobian matrix of a function `f: R^n --> R^m` for which `m >> n`.

The program may not take too long to run and may be even better if you have more processors to use but be careful of the things size, memory management is the simplest possible and if you try to allocate 10^8 contiguous memory positions your OS may just say "no" and the program dies.

## How to use

### Compiling

Just hitting `make all` on the keyboard will probably do the job.

### Running

This program needs two command line arguments, in order: the number of threads and the dimension of the funciton domain. And it gets the point in which the function and the derivative will be evaluated by `scanf`s so you should supply as many real numbers as the function domain deems necessary during the runtime of the program.

### Custom functions

It is possible to know how to it only by reading the comments in `function.c` file. But you just need to change the contents of function `f` in `function.c` without changing its prototype or return and adapt `calculate_codomain_dim` to your needs, but don't change its prototype.

By prototype I mean the name of the function, the type, order and name of its arguments and its return type;

## Future

I intend to write more functions (to derivate) and maybe a Python script to generate random entries but I don't have many plans other than that.

## References

[The place from where I got most of the idea.](https://github.com/MikeInnes/diff-zoo/blob/notebooks/forward.ipynb)

<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/1/17/Warning.svg/1200px-Warning.svg.png" width=20px> <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/7/7b/Canada_Stop_sign.svg/800px-Canada_Stop_sign.svg.png" width=20px> My thanks to [Wikimedia Commons](https://commons.wikimedia.org/wiki/File:Warning.svg) for the cool signs.