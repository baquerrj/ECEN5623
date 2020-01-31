#!/home/baquerrj/anaconda3/bin/python
from matplotlib import pyplot as plt
import math
import numpy as np

m = np.arange(1, 200, 1)
u = np.zeros(len(m))

# u = m * (2**(1/m) -1)
for i in range(len(m)):
    u[i] = m[i] * (2 ** (1/m[i]) - 1)

plt.figure()
plt.xlabel('Number of Services')
plt.ylabel('CPU Utility')
plt.title('Rate-Monotonic Least Upper Bound')
plt.grid(True)
plt.plot(m, u, linewidth='0.8')
plt.show()
