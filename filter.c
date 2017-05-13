#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>


FILE *input_img;
FILE *output_img;
int col, row;
unsigned char r[2000][2000];
unsigned char g[2000][2000];
unsigned char b[2000][2000];
unsigned char img[2000][2000];
pthread_t threads[10];
int threads_n;

void error(int c)
{
	printf("error %d \n", c);
	exit(1);
}

void *filter(void *arg)
{
	int *x = (int*)arg;
	int start_y = row / threads_n * (*x);
	int end_y = start_y + row / threads_n;
	for (int i = start_y; i < end_y; i++) {
		for (int j = 1; j < col-1; j++) {
			int rx = r[i+1][j-1] + 2*r[i+1][j] + r[i+1][j+1] - r[i-1][j-1] - 2*r[i-1][j] - r[i-1][j+1];
			int gx = g[i+1][j-1] + 2*g[i+1][j] + g[i+1][j+1] - g[i-1][j-1] - 2*g[i-1][j] - g[i-1][j+1];
			int bx = b[i+1][j-1] + 2*b[i+1][j] + b[i+1][j+1] - b[i-1][j-1] - 2*b[i-1][j] - b[i-1][j+1];
			int ry = r[i-1][j+1] + 2*r[i][j+1] + r[i+1][j+1] - r[i-1][j-1] - 2*r[i][j-1] - r[i+1][j-1];
			int gy = g[i-1][j+1] + 2*g[i][j+1] + g[i+1][j+1] - g[i-1][j-1] - 2*g[i][j-1] - g[i+1][j-1];
			int by = b[i-1][j+1] + 2*b[i][j+1] + b[i+1][j+1] - b[i-1][j-1] - 2*b[i][j-1] - b[i+1][j-1];
			double gr = sqrt(rx*rx + ry*ry);
			double gg = sqrt(gx*gx + gy*gy);
			double gb = sqrt(bx*bx + by*by);
			double color = (gr + gg + gb) / 3;
			img[i][j] = color > 255 ? 255 : color;
		}
	}
}

void main(int argc, char **argv)
{	
	struct timespec t1, t2;
	int thr_numbers[10];
	
	if (argc != 4) 
		error(1);
		
	input_img = fopen(argv[1], "r");
	output_img = fopen(argv[2], "w");
	threads_n = argv[3][0] - '0';
	
	char basket[100];
	fgets(basket, 100, input_img);
	if (fgetc(input_img) == '#')
		fgets(basket, 100, input_img);
	else 
		fseek(input_img, -1, SEEK_CUR);
	fscanf(input_img, "%d %d\n", &col, &row);
	fprintf(output_img, "P5\n%d %d\n255\n", col, row);
	
	
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			r[i][j] = fgetc(input_img);
			g[i][j] = fgetc(input_img);
			b[i][j] = fgetc(input_img);
		}
	}
	
	clock_gettime(CLOCK_REALTIME, &t1);
	printf("start at %ld\n", t1.tv_nsec);
	for (int i = 0; i < threads_n; i++) {
		thr_numbers[i] = i;
		pthread_create(&threads[i], NULL, filter, (void*)&thr_numbers[i]);
	}
	
	for (int i = 0; i < threads_n; i++) {
		int res = pthread_join(threads[i], NULL);
		printf("thread #%d finished\n", i);
	}
		
	clock_gettime(CLOCK_REALTIME, &t2);
	printf("end at   %ld\n", t2.tv_nsec);
	printf("diff = %ld\n", (t2.tv_nsec - t1.tv_nsec) / 10000);
	
	
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			fputc(img[i][j], output_img);
		}
	}
}










