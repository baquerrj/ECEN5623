#/bin/bash

if [ -z "$1" ]; then
   FPS=30
else
   FPS=$1
fi

TIMESTAMP=$(date +'%d%m%Y%H%M%S')
VIDEO_FILE="${FPS}Hz,${TIMESTAMP},timelapse.avi"
LOG_FILE="${FPS}Hz,${TIMESTAMP},timelapse.log"

echo "Image to Video shell script called" 2>&1 | tee -a $LOG_FILE

echo "value of FPS" $FPS >> $LOG_FILE
#use ffmpeg to convert to video
ffmpeg -r $FPS -pattern_type glob -i '*.ppm' -vcodec mpeg4 $VIDEO_FILE 2>&1 | tee -a $LOG_FILE

echo "INFO: Conversion complete using " $FPS " FPS " 2>&1 | tee -a $LOG_FILE
