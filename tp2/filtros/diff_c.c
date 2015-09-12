
#include <stdlib.h>
#include <math.h>
#include "../tp2.h"

#define MAX(a,b) \
   __extension__({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

void diff_c (
  unsigned char *src,
  unsigned char *src_2,
  unsigned char *dst,
  int m,
  int n,
  int src_row_size,
  int src_2_row_size,
  int dst_row_size
) {
  int idx;
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < m; j += 4) {
      idx = i*src_row_size+j;
      unsigned char b_resta = abs( src[idx] - src_2[idx] );
      unsigned char g_resta = abs( src[idx + 1] - src_2[idx + 1] );
      unsigned char r_resta = abs( src[idx + 2] - src_2[idx + 2] );
      unsigned char norma_inf = MAX(b_resta, g_resta);
      norma_inf = MAX(norma_inf, r_resta);

      dst[idx] = norma_inf; // blue
      dst[idx + 1] = norma_inf; // green
      dst[idx + 2] = norma_inf; // red
      dst[idx + 3] = 255; // alpha
    }


}
