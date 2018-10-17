# Roguelike

This is an early stage implementation of a classic roguelike.

## Contents

1. [Install](#install)
2. [Instructions](#instruction)
3. [License](#license)

## Install

#### Requires

* C compiler ;
* A curse implementation such as ncurses.

### Build

    $ ./configure
    $ make

There is no install target for now as it is not interesting enough yet.

## Instructions

Directions:

    y k u
     \|/
    h-@-l
     /|\
    b j n

Others:

* `.`: rest ;
* `>`: climb to the next level ;
* `<`: climb to the previous level ;
* `?`: open help menu ;
* `O`: open options menu ;
* `CTRL-C`: quit.

## License

All the code is licensed under the ISC License.
It's free, not GPLed !
