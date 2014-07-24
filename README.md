EricLedCube
===========

Design notes

Le 164 a une limite de 50mA en Icc ce qui limite les LEDs à 50mA / 8 = 6.25mA

SIMULATION 4 COL/4 PLAN

Avec résistances de 33ohms±5%

Statistiques pour code dans loop() sans aucun port manipulation

LED CUBE STATISTICS**

time on delay=1us
loop=1.37ms
toff/loop=1.33ms
ton/loop=0.04ms
duty cycle=2.78%
refresh rate=730.99Hz
Icol = 0.5mA

LED CUBE STATISTICS**

time on delay=450us
loop=3.19ms
toff/loop=1.34ms
ton/loop=1.84ms
duty cycle=57.90%
refresh rate=313.97Hz
Icol = 8.0mA

LED CUBE STATISTICS**

time on delay=1000us
loop=5.42ms
toff/loop=1.34ms
ton/loop=4.08ms
duty cycle=75.29%
refresh rate=184.43Hz
Icol = 10.4mA
(Icol avec interrupt à 4*184Hz = 10.4mA)

LED CUBE STATISTICS**

time on delay=2500us
loop=11.51ms
toff/loop=1.34ms
ton/loop=10.16ms
duty cycle=88.33%
refresh rate=86.90Hz
Icol = 12.2mA

Statistiques pour code dans loop() avec port manipulation

LED CUBE STATISTICS

time on delay=250us
loop=1.27ms
toff/loop=0.24ms
ton/loop=1.03ms
duty cycle=81.00%
refresh rate=788.27Hz
Icol = 10.9mA
(Icol avec interrupt à 4*788Hz = 10.9mA)

LED CUBE STATISTICS

time on delay=450us
loop=2.08ms
toff/loop=0.24ms
ton/loop=1.84ms
duty cycle=88.44%
refresh rate=480.77Hz
Icol = 11.8mA

LED CUBE STATISTICS

time on delay=1000us
loop=4.31ms
toff/loop=0.24ms
ton/loop=4.07ms
duty cycle=94.39%
refresh rate=231.91Hz
Icol = 12.5mA

LED CUBE STATISTICS

time on delay=2500us
loop=10.40ms
toff/loop=0.24ms
ton/loop=10.16ms
duty cycle=97.68%
refresh rate=96.17Hz
Icol = 13mA

Sans 74595 pour driver les Darlington (4 IO directs)

LED CUBE STATISTICS

time on delay=250us
loop=1.13ms
toff/loop=0.10ms
ton/loop=1.03ms
duty cycle=91.38%
refresh rate=888.57Hz
Icol = 12.3mA

LED CUBE STATISTICS

time on delay=76us
loop=0.42ms
toff/loop=0.10ms
ton/loop=0.32ms
duty cycle=77.17%
refresh rate=2398.08Hz
Icol = 11mA

Sans 74595, en simulant une latch à place d'un SR 74164

LED CUBE STATISTICS

time on delay=16us
loop=101.90us
toff/loop=19.07us
ton/loop=82.82us
duty cycle=81.28%
refresh rate=9813.81Hz