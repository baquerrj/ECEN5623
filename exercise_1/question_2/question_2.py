#!/home/baquerrj/anaconda3/bin/python
from matplotlib import pyplot as plt
import math
import numpy as np


def plot():
    m = np.arange(1, 200, 1)
    u = np.zeros(len(m))

    # u = m * (2**(1/m) -1)
    for i in range(len(m)):
        u[i] = m[i] * (2 ** (1/m[i]) - 1)

    u_min = np.min(u) * 100
    fig, ax = plt.subplots()
    textstr = '\n'.join((
            r'$U = m\times(2^{\frac{1}{m}}-1)$',
            r'$U_{asymptotic} = %.3f \%%$' % (u_min))) 
    props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)
    ax.text(0.50, 0.95, textstr, transform=ax.transAxes, fontsize=12, verticalAlignment='top', bbox=props)
    plt.xlabel('Number of Services')
    plt.ylabel('CPU Utility')
    plt.title('Rate-Monotonic Least Upper Bound')
    plt.grid(True)
    plt.plot(m, u, linewidth='0.8')

    return fig

if __name__ == '__main__':
    fig = plot()
    plt.show()
