#ifndef HOUGH_H
#define HOUGH_H

void HoughLines( int, void* );
void* executeHough( void* args );

void HoughElliptical( int, void* );
void* executeHoughElliptical( void* args );

#endif  // HOUGH_H