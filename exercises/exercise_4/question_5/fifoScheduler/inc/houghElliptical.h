#ifndef HOUGHELLIPTICAL_H
#define HOUGHELLIPTICAL_H

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

#endif  // HOUGHELLIPTICAL_H