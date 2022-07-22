import math
A = 1
n=5
d=10
d0=1
x_sigma=0.7734

RSSI = (A - (10*n*math.log(d/d0)-x_sigma))
