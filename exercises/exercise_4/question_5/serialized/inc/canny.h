#ifndef CANNY_H
#define CANNY_H

/**
 * @brief Performs the actual canny transform on the
 *        cvCatpure capture
 *
 * @param int     exists to satifsy cv::createTrackbar API
 * @param void*   exists to satifsy cv::createTrackbar API
 * @returns void
 */
void CannyThreshold( int, void* );

/**
 * @brief Entry point for canny worker thread
 *
 * @param args
 * @return void*
 */
void* executeCanny( void* args );

#endif  // CANNY_H