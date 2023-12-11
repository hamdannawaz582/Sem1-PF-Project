# Sem1-PF-Project
First semester Programming Fundamentals project, a copy of the Atari game Centipede

Project Brief and Evaluation Sheet are attached. We were given a list of allowed SFML data types and a skeleton code to familiarize us with SFML, both can be found in /Skeleton Code.

# Compiling and Running

## Requirements

### G++
### Arch Linux
```
sudo pacman -S gcc
```

### Debian
```
sudo apt install g++
```

### SFML
### Arch Linux
```
sudo pacman -S sfml
```

### Debian
```
sudo apt-get install libsfml-dev
```

## Compiling
```
g++ -c Centipede.cpp
g++ Centipede.o -o centipede -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
```

## Running
```
./centipede
```

# Playing the game
Menu navigation is through keyboard only, arrow keys and WASD are supported, gameplay is with WASD for movement and X and Y for shooting. Leaderboard mechanic is incomplete, the only thing that functions properly is the change music screen. To add more songs add them to the array in changeBGM(). Expect segmentation faults after level 11.
