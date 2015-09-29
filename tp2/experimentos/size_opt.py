# -*- coding: utf-8 -*-
"""
Benchmark de filtros vs tamaño global de la imagen
"""

import itertools as it
import numpy as np
import pylab as pl
from os import system

from benchmark import *

path = '../build'
tot = { 'diff': { 'c': [], 'asm': [] }, 
        'blur': { 'c': [], 'asm': [] } }

tot = { 'o0': [], 'o3': []}
args = { 'diff': 'img2.bmp',
         'blur': '5 2' }

n = { 'diff': { 'c': 400, 'asm': 4000 }, 
      'blur': { 'c': 400, 'asm': 4000 } }
# lo que quiero que haga de la de 1kx1k
filtros = ('diff', 'blur')
implementaciones = ('c', 'asm')


sz = map(int, np.linspace(2000, 100, 200))
sz = [i - i%4 for i in sz] #Redondeo a múltiplo de 4
filt = 'diff'

for x in sz:
    print "Procesando imagen de {0} x {1}".format(x, x)
    image_rand(x, x, fname='img1.bmp')
    image_rand(x, x, fname='img2.bmp')
    for coda in ['o0', 'o3']:
        niter = int(400*(1000.0/x)**2)
        t = time_me('diff', 'img1.bmp', 'c', args[filt], n=niter, path=path, coda=coda)
        tot[coda].append(t/niter)

sz = np.array(sz)
o3 = np.array(tot['o3'])
o0 = np.array(tot['o0'])
fig, ax = pl.subplots()
ax.plot(sz, 1e9*o0/sz**2, label='Tiempo con -O0')
ax.plot(sz, 1e9*o3/sz**2, label='Tiempo con -O3')
ax.set_ylabel(r'Tiempo por pixel [ns]')
ax.set_xlabel('Ancho/Alto de la imagen [px]')
ax.legend(loc = "center right")
fig.tight_layout()
fig.savefig('tiempo_optimiz_{0}.pdf'.format(filt))
fig.savefig('tiempo_optimiz_{0}.png'.format(filt))
fig, ax = pl.subplots()
ax.plot(sz, o0/o3)
ax.set_xlabel('Ancho/Alto de la imagen [px]')
ax.set_ylabel(r'Speedup de -O3 vs -O0')
fig.tight_layout()
fig.savefig('speedup_optimiz_{0}.pdf'.format(filt))
fig.savefig('speedup_optimiz_{0}.png'.format(filt))
np.savetxt('tiempos_optimiz_{0}.dat'.format(filt), np.transpose([o3, o0]), header='o3, o0')
