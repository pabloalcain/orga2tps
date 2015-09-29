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

args = { 'diff': 'img2.bmp',
         'blur': '5 2' }

n = { 'diff': { 'c': 25, 'asm': 125 }, 
      'blur': { 'c': 25, 'asm': 125 } }
# lo que quiero que haga de la de 1kx1k
filtros = ('diff', 'blur')
implementaciones = ('c', 'asm')


rad = map(int, np.linspace(20, 1, 20))
x = 200
y = 200
filt = 'blur'
image_rand(x, y, fname='img1.bmp')

for r in rad:
    print "Procesando imagen para radio {0}".format(r)
    for imp in implementaciones:
        niter = int(n[filt][imp])
        t = time_me(filt, 'img1.bmp', imp, '2 {0}'.format(r), n=niter, path=path)
        tot[filt][imp].append(t/niter)
    # Aquí comparamos que las dos imágenes sean iguales
    diff = compara_imagenes('img1.bmp.{0}.C.bmp'.format(filt), 
                            'img1.bmp.{0}.ASM.bmp'.format(filt), threshold=0)
    if diff:
        msg = 'Atención! Para el radio {0} las implementaciones no concuerdan'
        print msg.format(r)

rad = np.array(rad)
asm = np.array(tot[filt]['asm'])
c = np.array(tot[filt]['c'])
fig, ax = pl.subplots()
ax.plot(rad, 1e9*c/x/y, label='Tiempo en C')
ax.plot(rad, 1e9*asm/x/y, label='Tiempo en ASM')
ax.set_ylabel(r'Tiempo por pixel [ns]')
ax.set_xlabel('Radio del filtro [px]')
ax.legend(loc = "center right")
fig.tight_layout()
fig.savefig('tiempo_radio_{0}.pdf'.format(filt))
fig.savefig('tiempo_radio_{0}.png'.format(filt))
fig, ax = pl.subplots()
ax.plot(rad, c/asm)
ax.set_xlabel('Radio del filtro [px]')
ax.set_ylabel(r'Speedup de ASM vs C')
fig.tight_layout()
fig.savefig('speedup_radio_{0}.pdf'.format(filt))
fig.savefig('speedup_radio_{0}.png'.format(filt))
np.savetxt('tiempos_radio_{0}.dat'.format(filt), np.transpose([asm, c]), header='asm, c')
