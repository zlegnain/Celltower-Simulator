# Celltower-Simulator
A program that will display moving vehicles and cell towers. Designed and 
programmed using TCP, as vehicles (clients) will send data to servers (towers) 
using multithreading programming.

# Prerequisites
- GNU compiler and Linux (I have not tested if it works on Windows)

# Instructions
1. clone/download repository
2. compile by navigating to the directory with the source files and run `make all`
3. To run simulator type `./simulator&`
4. To generate vehicles type `./generator&`, this will generator vehicles with random x and y coordinates
5. To stop generating vehicles type `fg` and then Ctrl-C
6. To stop the program run `./stop`
7. To remove all object files in directory, simple clean the makefile by doing `make clean`





