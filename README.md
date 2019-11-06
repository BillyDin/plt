
# WARNING

> When you try to compile, there should be normally 2 errors. To correct those 2 errors, you have to write the line below in Engine.h and Command.h

```sh 
#include "../state/State.h"
```

# Projet Logiciel Transversal

> This project is to make a video game in C++ for our final year in ENSEA "Informatique et systèmes" specialization

## Livraison 2.2

## Get Started

Clone the repository
```sh 
$ git clone https://github.com/BillyDin/plt.git
```

## Build

```sh
$ cd plt
$ mkdir build
$ cd build
$ cmake .. && make
```

## Print

```sh
$ ./bin/client hello
```

## Test

```sh
$ cd ./plt/build
$ make code-coverage
```

## Print a rendering

```sh
$ ./bin/client render
```

## Run the engine

```sh
$ ./bin/client engine
```


## Release History

* 2.2
    * Report 2.2
    * Engine diagram
    * Implemented code for the game engine
    * Print different renderings of state by calling the game engine 

* 2.1
    * Report 2.1
    * Render diagram
    * Implemented code to print a rendering of state

* 1.final
    * State diagram 
    * State implementation
    * Test code coverage
    * Report 1.final

* 1.1
    * Implement the code to print a message
    * Report 1.1
