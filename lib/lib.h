//
// Created by astra on 6/2/2024.
//

#ifndef LIB_H
#define LIB_H

extern unsigned int size;
extern unsigned int histogram[256];
extern unsigned int hist_max;
extern unsigned int width;
extern unsigned int height;
extern double luminance;
extern double clipped;
extern double mean;

int read_jpeg_file(const char* filename);
double lum(double x);

#endif //LIB_H
