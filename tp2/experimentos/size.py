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

n = { 'diff': { 'c': 400, 'asm': 4000 }, 
      'blur': { 'c': 400, 'asm': 4000 } }
# lo que quiero que haga de la de 1kx1k
filtros = ('diff', 'blur')
implementaciones = ('c', 'asm')


sz = map(int, np.linspace(2000, 100, 200))
sz = [i - i%4 for i in sz] #Redondeo a múltiplo de 4

for x in sz:
    print "Procesando imagen de {0} x {1}".format(x, x)
    image_rand(x, x, fname='img1.bmp')
    image_rand(x, x, fname='img2.bmp')
    for filt in filtros:
        for imp in implementaciones:
            niter = int(n[filt][imp]*(1000.0/x)**2)
            t = time_me(filt, 'img1.bmp', imp, args[filt], n=niter, path=path)
            tot[filt][imp].append(t/niter)
        # Aquí comparamos que las dos imágenes sean iguales
        diff = compara_imagenes('img1.bmp.{0}.C.bmp'.format(filt), 
                               'img1.bmp.{0}.ASM.bmp'.format(filt), threshold=0)
        if diff:
            msg = 'Atención! Las imágenes de {0}x{1} en filtro {2} no concuerda'
            print msg.format(x, x, filt)
            save = 'img{0}_{1}x{2}.bmp'
            save_img1 = save.format(1, x, x)
            save_img2 = save.format(2, x, x)
            system('cp img{0}.bmp img{0}_{1}x{2}.bmp'.format(1, x, x))
            system('cp img{0}.bmp img{0}_{1}x{2}.bmp'.format(2, x, x))
            print 'Imágenes guardadas en {0} y {1}'.format(save_img1, save_img2)

sz = np.array(sz)
for filt in filtros:
    asm = np.array(tot[filt]['asm'])
    c = np.array(tot[filt]['c'])
    fig, ax = pl.subplots()
    ax.plot(sz, 1e9*c/sz**2, label='Tiempo en C')
    ax.plot(sz, 1e9*asm/sz**2, label='Tiempo en ASM')
    ax.set_ylabel(r'Tiempo por pixel [ns]')
    ax.set_xlabel('Ancho/Alto de la imagen [px]')
    ax.legend(loc = "center right")
    fig.tight_layout()
    fig.savefig('tiempo_{0}.pdf'.format(filt))
    fig.savefig('tiempo_{0}.png'.format(filt))
    fig, ax = pl.subplots()
    ax.plot(sz, c/asm)
    ax.set_xlabel('Ancho/Alto de la imagen [px]')
    ax.set_ylabel(r'Speedup de ASM vs C')
    fig.tight_layout()
    fig.savefig('speedup_{0}.pdf'.format(filt))
    fig.savefig('speedup_{0}.png'.format(filt))
    np.savetxt('tiempos_{0}.dat'.format(filt), np.transpose([asm, c]), header='asm, c')
