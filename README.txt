# 2DShooter
2-D top down shooter matches playable on the user terminal - C++

*IMPORTANT* Program requires ncurses library *IMPORTANT*
How to compile with ncurses: g++ Simulator.cpp -lncurses -std=c++11

Terminal input takes the map file you decide to use as its only argument.
Example : ./a.out COD_Shipment.txt

Controls:
Movement - WASD / Arrow Keys
Shooting - Spacebar

Rules:
Terrorist can plant bombs by moving onto the "P" labeled areas.
Counter-Terrorist can defuse bombs by moving onto the "B" labeled area (when planted).

Win Conditions: Terrorist Side (T)
Kill opposing team
Plant bomb and have the timer run out

Win Conditions: Counter-Terrorist (CT)
Kill opposing team
Defuse bomb
Round timer runs out

ASCII Map Legend:
# : Bridge
X : Wall
O : Obstacle
B : Bomb
C : Counter-Terrorist
T : Terrorist
P : Bombsite
n : Map outline
