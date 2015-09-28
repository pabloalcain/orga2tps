# -*- coding: utf-8 -*-
"""
Benchmark de filtros vs aspect ratio de la imagen
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

n = { 'diff': { 'c': 200, 'asm': 400 }, 
      'blur': { 'c': 100, 'asm': 400 } }
# lo que quiero que haga de la de 1kx1k
filtros = ('diff', 'blur')
implementaciones = ('c', 'asm')


width = 2000 # el ancho de la imagen si fuera cuadrada
low = np.linspace(0.1,1,100)
ar = []
area = []
for ratio in low:
    for _ in (np.sqrt(ratio), np.sqrt(1/ratio)):
        x = int(_ * width)
        x -= x%4
        y = int((1.0/_) * width)
        y -= y%4
        ar.append(float(x)/float(y))
        area.append(float(x*y))

        print "Procesando imagen de {0} x {1}".format(x, y)
        image_rand(x, y, fname='img1.bmp')
        image_rand(x, y, fname='img2.bmp')
        for filt in filtros:
            for imp in implementaciones:
                niter = int(n[filt][imp]) * (1000.0**2/(x*y))
                t = time_me(filt, 'img1.bmp', imp, args[filt], n=niter, path=path)
                tot[filt][imp].append(t/niter)
                # Aquí comparamos que las dos imágenes sean iguales
            diff = compara_imagenes('img1.bmp.{0}.C.bmp'.format(filt), 
                                    'img1.bmp.{0}.ASM.bmp'.format(filt), threshold=0)
            if diff:
                msg = 'Atención! Las imágenes de {0}x{1} en filtro {2} no concuerda'
                print msg.format(x, y, filt)
                save = 'img{0}_{1}x{2}.bmp'
                save_img1 = save.format(1, x, y)
                save_img2 = save.format(2, x, y)
                system('cp img{0}.bmp img{0}_{1}x{2}.bmp'.format(1, x, y))
                system('cp img{0}.bmp img{0}_{1}x{2}.bmp'.format(2, x, y))
                print 'Imágenes guardadas en {0} y {1}'.format(save_img1, save_img2)
        system('rm img1.bmp img2.bmp img1.bmp.* img2.bmp.*')

ar = np.array(ar)
area = np.array(area)

for filt in filtros:
    asm = np.array(tot[filt]['asm'])
    c = np.array(tot[filt]['c'])
    fig, ax = pl.subplots()
    ax.plot(ar, 1e9*c/area, label='Tiempo en C')
    ax.plot(ar, 1e9*asm/area, label='Tiempo en ASM')
    ax.set_ylabel(r'Tiempo por pixel [ns]')
    ax.set_xlabel('Aspect ratio')
    ax.legend(loc = "center right")
    fig.tight_layout()
    fig.savefig('tiempo_ratio_{0}.pdf'.format(filt))
    fig.savefig('tiempo_ratio_{0}.png'.format(filt))
    fig, ax = pl.subplots()
    ax.plot(ar, c/asm)
    ax.set_xlabel('Aspect ratio')
    ax.set_ylabel(r'Speedup de ASM vs C')
    fig.tight_layout()
    fig.savefig('speedup_ratio_{0}.pdf'.format(filt))
    fig.savefig('speedup_ratio_{0}.png'.format(filt))
    np.savetxt('tiempos_ratio_{0}.dat'.format(filt), np.transpose([area, ar, asm, c]), header='area, aspect ratio, asm, c')
