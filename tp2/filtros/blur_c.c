#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../tp2.h"

void crea_filtro(int radius, float sigma, float *filtro) {
  float sum, dist;
  int x, y, len;
  len = 2*radius + 1;

  sum = 0.0;
  /* se puede hacer más corto, pero no creo que esto consuma la mayor
     parte del tiempo */
  for (int i = 0; i < len; ++i) {
    for (int j = 0; j < len; ++j) {
      x = (i - radius)/sigma;
      y = (j - radius)/sigma;
      dist = (x * x + y * y)/2;
      filtro[i * len + j] = exp(-dist);
      sum += filtro[i * len + j];
    }
  }

  /* elijo hacer esto para asegurarme que esté normalizado a 1 el filtro */
  float invsum = 1/sum;
  for (int j = 0; j < len * len; j++) {
    filtro[j] *= invsum;
  }
}


void blur_c (unsigned char *src,
             unsigned char *dst,
             int cols,
             int filas,
             float sigma,
             int radius)
{
  int idx, neigh;
  float rgb[3];
  float *filtro;
  int len = (2*radius + 1);
  
  filtro = (float*) malloc(len * len * sizeof(float));
  crea_filtro(radius, sigma, filtro);

  for (int i = radius; i < filas - radius; ++i) { /* evitamos los bordes */
    for (int j = radius; j < cols - radius; ++j ) {
      idx = 4*(i*cols + j); /* idx apunta a la estructura BGRA */
      rgb[0] = 0;
      rgb[1] = 0;
      rgb[2] = 0;
      for (int k = 0; k < len; ++k) {
        for (int l = 0; l < len; ++l) {
          int di = i + k - radius;
          int dj = j + l - radius;
          double factor = filtro[k*len + l];
          neigh = 4 * ( di * cols + dj);
          
          rgb[0] += factor * src[neigh];
          rgb[1] += factor * src[neigh + 1];
          rgb[2] += factor * src[neigh + 2];
        }
      }
      for (int l = 0; l < 3; ++l) {
        dst[idx + l] = (unsigned char) rgb[l];
      }
      dst[idx + 3] = 0xFF;
    }
  }
  free(filtro);

}
