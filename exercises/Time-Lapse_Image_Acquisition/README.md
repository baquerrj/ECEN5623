# ECEN5623 Real-Time Embedded Systems
## Final Project: Time-Lapse Image Acquisition
### Roberto Baquerizo

#### Overview
For the final project of ECEN 5623, I plan to complete the Time-Lapse Image Acquisition from the Extended Lab. I intend to follow Extended Lab System Requirements. The primary goal of the project is to implement a time-lapse image acquisition service to run on the Jetson Nano development kit. The Logitech C270 camera will be used to acquire individual frames at a rate of 1Hz. The frames will be encoded into the PPM format, and a time-stamp will be included in the PPM header of every capture frame. The time-stamp will include at the very least the time associated with the capture and the platform on which the capture took place.
The image acquisition shall be implemented as a soft real-time system. The system shall be capable of processing frames at 1Hz and it shall command the camera to take an image at 1Hz. The frame acquisition and time-stamping will be accurate so that there is no observable error in 2000 frames, or 2000 seconds of continuous operation. The application will transfer processed frames to the host the Jetson Nano is connected to via Ethernet.
In addition, I have chosen the following “target” and “stretch” goals to implement:
* Continuous download of frames over Ethernet so that you can run indefinitely and never run out of space on your flash file system which should maintain only the last 2000 frame (target goal #2).
* Run at a higher frame rate of 10 Hz with continuous download, this time for 9 minutes, which will produce up to 6000 frames (about 6GB uncompressed for 640x480) and repeat the jitter and accumulated latency verification at this higher rate (stretch goal #1).

#### Requirements
1. The name of the platform the (server) application is running on shall be embedded in the PPM header of every frame as a comment, e.g. the output of `uname -a`.
2. A timestamp shall be embedded in the PPM header of every frame as a comment.
* SEQUENCER:
1.	Service execution shall be handled by a Cyclic Executive which will be the sequencer thread.
2.	The sequencer thread shall always run at 20Hz and perform semaphore posts to release other services for execution.
* FRAME COLLECTOR:
1.	The frame collector shall execute at 1Hz or 10Hz, depending on user input.
2.	The frame collector thread shall acquire single frames at its configured operating frequency (1Hz or 10Hz).
3.	The frame collector thread shall emplace the raw captured frames in a ring buffer, or similar memory structure.
4.	The captured frames shall be timestamped in the memory structure.
5.	The resolution of the acquired frames shall be 640x480.
* FRAME PROCESSOR:
1.	The frame processor thread shall execute at 1Hz or 10Hz, depending on user input.
2.	The frame processor thread shall perform YUV to RGB conversion , and then dump compressed data to a PPM file.
3.	Upon release by the sequencer, the frame processor shall wait on a semaphore posted by the frame collector. This will notify the processor that a new frame is ready to be processed.
* FRAME SENDER:
1.	The frame sender thread shall execute at 1Hz or 10Hz, depending on user input.
2.	The frame sender thread shall set up a TCP server.
3.	The frame sender thread shall transfer files over TCP to the currently connected client.
* CLEANUP SERVICE:
1.	The Cleaner thread shall execute at 0.1Hz or 1Hz, depending on the current capture frequency.
2.	The Cleaner thread shall run at the lowest priority.
3.	The Cleaner thread shall start deleting PPM files to free up memory after the 2000 frames have been transferred by the Frame Sender.
a.	This is so that frames that have not been sent to the remote client are not deleted and lost forever.
* FRAME RECEIVER:
1.	The frame receiver thread shall set up a TCP client.
2.	The frame receiver thread shall connect to the TCP server running on the Jetson Nano.
3.	The frame receiver thread shall perform blocking reads on its socket to receive the data from the server.
4.	The frame receiver thread shall write the received packets to disk as a PPM file.