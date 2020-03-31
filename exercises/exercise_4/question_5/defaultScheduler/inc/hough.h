#ifndef HOUGH_H
#define HOUGH_H

/**
 * @brief Performs the actual hough-lines transform on the
 *        cvCatpure capture
 *
 * @param int
 * @param void
 * @returns void
 */
void HoughLines( int, void* );

/**
 * @brief Entry point for hough-lines transform
 *
 * @param args
 * @return void*
 */
void* executeHough( void* args );

/**
 * @brief Performs the actual hough-elliptical transform on the
 *        cvCatpure capture
 *
 * @param int
 * @param void
 * @returns void
 */
void HoughElliptical( int, void* );

/**
 * @brief Entry point for hough-elliptical transform
 *
 * @param args
 * @return void*
 */
void* executeHoughElliptical( void* args );

#endif  // HOUGH_H