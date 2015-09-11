
#include <stdlib.h>
#include <math.h>
#include "../tp2.h"

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
	unsigned char (*src_matrix)[src_row_size] = (unsigned char (*)[src_row_size]) src;
	unsigned char (*src_2_matrix)[src_2_row_size] = (unsigned char (*)[src_2_row_size]) src_2;
	unsigned char (*dst_matrix)[dst_row_size] = (unsigned char (*)[dst_row_size]) dst;

	for (int i = 0; i < n; ++i)
		for (int j = 0; j < src_row_size; j += 4) {
			unsigned char b_resta = abs( src_matrix[i][j] - src_2_matrix[i][j] );
			unsigned char g_resta = abs( src_matrix[i][j + 1] - src_2_matrix[i][j + 1] );
			unsigned char r_resta = abs( src_matrix[i][j + 2] - src_2_matrix[i][j + 2] );
			unsigned char norma_inf = max(max(b_resta, g_resta), r_resta);

			dst_matrix[i][j] = norma_inf; // blue
			dst_matrix[i][j + 1] = norma_inf; // green
			dst_matrix[i][j + 2] = norma_inf; // red
			dst_matrix[i][j + 3] = 255; // alpha			
		}


}
