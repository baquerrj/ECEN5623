#ifndef HOUGH_H
#define HOUGH_H

void HoughLines( int, void* );
void* executeHough( int dev );

void HoughElliptical( int, void* );
void* executeHoughElliptical( int dev );

#endif  // HOUGH_H