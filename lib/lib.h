//
// Created by astra on 6/2/2024.
//

#ifndef LIB_H
#define LIB_H

extern double luminance;
extern double clipped;
extern unsigned int histogram[256];

int read_jpeg_file(char* filename);

#endif //LIB_H
