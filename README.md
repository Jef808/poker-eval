# Texas Hold'em Poker Evaluator

A Monte Carlo-based Texas Hold'em poker hand evaluator that calculates
winning probabilities between two players. Available as both GUI and
command-line applications with identical functionality.

## Features

- Calculate win probabilities for two poker hands
- Support for pre-flop, flop, turn, and river scenarios
- Uses Monte Carlo sampling with 100,000 iterations for preflop scenarios
- GUI and CLI interfaces

## Building

### Requirements

- CMake 4.0.1 or higher
- C++20 compatible compiler
- Internet connection (for downloading dependencies)

### Build Instructions

``` bash
mkdir build
cd build
cmake ..
make
```

Dependencies (SFML and Catch2) are automatically downloaded via CMake's
FetchContent.

## Usage

### CLI Version

``` bash
./cli "hand1" "hand2" [board]
```

**Arguments:** 
- `hand1`: First player's 2 cards (e.g., "As Kh") 
- `hand2`: Second player's 2 cards (e.g., "Qd Jc")
- `board`: Optional board cards (e.g., "Ts 9h 8d")

**Card Format:** `[2-9TJQKA][shdc]` (rank + suit)

**Examples:**

``` bash
# Pre-flop
$ ./cli "As Ah" "Kd Kc"

Player 1 wins: 81.08%
Player 2 wins: 18.56%
Ties:           0.36%

# With flop
$ ./cli "As Kh" "Qd Jc" "Ts 9h 8d"

Player 1 wins:  0.91%
Player 2 wins: 99.09%
Ties:           0.00%

# With turn
$ ./cli "7h 7d" "Ac Kh" "2s 3c 4d 5h"

Player 1 wins:  9.09%
Player 2 wins: 84.09%
Ties:           6.82%
```

### GUI Version

``` bash
./holdem_evaluator
```

The GUI provides the same functionality as the CLI through an 
intuitive interactive interface.

![Screenshot](/resources/gui.png)

## Implementation

For the 5-card evaluation, we closely follow [Cactus Kev](http://suffe.cool/poker/evaluator.html)'s blog post. Instead of the binary search approach proposed at the end of his article, we use a straightforward perfect hash technique to obtain a very efficient 5 card evaluator.

## Running Tests

By default, tests are not built. To build and run tests, replace the build command above by

``` bash
mkdir build
cd build
cmake .. -DBUILD_TESTS=ON
make test
```
