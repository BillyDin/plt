# Projet Logiciel Transversal 

> This project is to make a video game in C++ for our final year in ENSEA "Informatique et systèmes" specialization

[![forthebadge](https://forthebadge.com/images/badges/made-with-c-plus-plus.svg)](https://forthebadge.com)
[![forthebadge](https://forthebadge.com/images/badges/built-with-love.svg)](https://forthebadge.com)

## Release 4.final 

## Get Started

Clone the repository
```sh 
$ git clone https://github.com/BillyDin/plt.git
```
## Install dependencies

```sh
$ apt-get update
$ apt-get install python python3 libmicrohttpd-dev lcov libsfml-dev gcc cmake make libxml2-dev dia lcov libboost-all-dev gcovr python-pip \
   && pip install gcovr
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

## Make a roll back

```sh
$ ./bin/client rollback
```

## Play vs Random AI

```sh
$ ./bin/client random_ai
```
## Play vs Heuristic AI

```sh
$ ./bin/client heuristic_ai
```
## Play vs Deep AI

```sh
$ ./bin/client deep_ai
```

## Random AI vs Random AI

```sh
$ ./bin/client raivsrai
```
## Heuristic AI vs Heuristic AI

```sh
$ ./bin/client haivshai
```
## Deep AI vs Deep AI

```sh
$ ./bin/client daivsdai
```

## Deep AI vs Deep AI with the engine in a different thread
```sh
$ ./bin/client thread
```

## Make IAs play the game for 12 turns and save all commands in "replay.txt"
```sh
$ ./bin/client record
```

## Play the game through the record stored in "replay.txt"
```sh
$ ./bin/client play
```

## Start the server and connect 2 players to it

### In a first bash
```sh
$ ./bin/server listen
```

### In a second bash
```sh
$ ./bin/client network_final
```

### In a third bash
```sh
$ ./bin/client network_final
```

## Start the server, connect 2 players to it, launch the game with DeepAI vs Deep AI

### In a first bash
```sh
$ ./bin/server listen
```

### In a second bash
```sh
$ ./bin/client network_final
```

### In a third bash
```sh
$ ./bin/client network_final
```


## Release History

* 4.final
    * Report 4.final
    * Implemented the final feature to make our game online


* 4.2
    * Report 4.2
    * Implemented WEB API


* 4.1
    * Report 4.1
    * Implemented the engine of the game in a different thread than the main thread
    * Implemented command serialization in JSON

* 3.final
    * Report 3.final
    * AI diagram with DeepAI
    * Implemented code for the Deep AI
    * Player vs Deep AI
    * Deep AI vs Deep AI

* 3.1
    * Report 3.1
    * AI diagram with Heuristic
    * Implemented code for the Heuristic AI
    * Player vs Heuristic AI
    * Heuristic AI vs Heuristic AI

* 2.final
    * Report 2.final
    * AI diagram
    * Implemented code for the Random AI
    * Player vs Random AI
    * Random AI vs Random AI

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
