# -*- coding: utf-8 -*-
"""
Funciones para poder correr un benchmarking
"""

import numpy, Image
from os import system
import time

def image_rand(w, h, fname=None):
    """Crea una imagen al azar
    
    Parámetros:
    -----------

    w: integer
        Ancho de la imagen
    h: integer
        Alto de la imagen
    fname: string
        Nombre del archivo de salida
        Por defecto es None [se genera un archivo 'random<w>x<h>.bmp']
    """
    if fname == None:
        fname = 'random{0}x{1}.bmp'.format(w, h)
    imarray = numpy.random.rand(w, h, 3)*255
    im = Image.fromarray(imarray.astype('uint8'))
    im.save(fname)

def time_me(filtro, img, imp, params, n=1, path=None):
    """Toma una imagen y le aplica el filtro con la implementación
    determinados por un número de iteraciones. Devuelve el tiempo que
    tardó.
    
    Parámetros:
    -----------

    filtro: string
        Filtro a correr: 'blur' o 'diff'
    img: string
        Imagen para el filtro.
    imp: string
        Implementación a usar: 'c' o 'asm'
    params: string
        Parámetros: nombre de la otra imagen para diff, sigma y radio para blur
    n: int
        Número de iteraciones (parámetro -t del programa original)
        Por defecto es 1
    path: string
        Directorio donde está el ejecutable.
        Por defecto es None [se considera instalado]
    """
    if not path:
        exe = 'tp2'
    else:
        exe = '{0}/tp2'.format(path)

    cmd = [exe, filtro, '-i {0}'.format(imp), '-t {0}'.format(n), img, params]
    t0 = time.time()
    system(' '.join(cmd))
    return time.time() - t0

def compara_imagenes(file1, file2, threshold=5):
    """
    Compara dos imágenes y devuelve si son diferentes o no (a menos de
    un threshold)
    """
    
    image1 = Image.open(file1)
    image2 = Image.open(file2)
    h1 = numpy.array(image1)
    h2 = numpy.array(image2)
    diff = (abs(h1 - h2) > threshold).any()
    
    print abs(h1-h2)
    return diff
