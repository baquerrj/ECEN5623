#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <thread_utils.h>

//! @file Defines thread and process configuration stuff

// Thread CPU affinities. (negative value = no affinity specified)
const int NUM_CPUS = 4;  // number of CPU's on the target machine

const int CPU_MAIN      = 0;
const int CPU_SEQUENCER = CPU_MAIN;
const int CPU_COLLECTOR = 0;
const int CPU_PROCESSOR = 1;
const int CPU_RECEIVER  = 2;
const int CPU_SENDER    = 2;
const int CPU_LOGGER    = 3;

static const ProcessParams sequencerParams = {
    cpuSequencer,
    SCHED_FIFO,
    99,
    0};

static const ThreadConfigData sequencerThreadConfig = {
    true,
    "SEQUENCER",
    sequencerParams};

static const ProcessParams collectorParams = {
    cpuCollector,
    SCHED_FIFO,
    98,
    0};

static const ThreadConfigData collectorThreadConfig = {
    true,
    "COLLECTOR",
    collectorParams};

static const ProcessParams senderParams = {
    cpuSender,  // CPU1
    SCHED_FIFO,
    98,  // highest priority
    0};

static const ThreadConfigData senderThreadConfig = {
    true,
    "SENDER",
    senderParams};

static const ProcessParams processorParams = {
    cpuProcessor,
    SCHED_FIFO,
    98,
    0};

static const ThreadConfigData processorThreadConfig = {
    true,
    "PROCESSOR",
    processorParams};

static const ProcessParams cleanupParams = {
    cpuLogger,
    SCHED_FIFO,
    1,  // low priority thread
    0};

static const ThreadConfigData cleanupThreadConfig = {
    true,
    "CLEANUP",
    cleanupParams};

extern bool abortS1;
extern bool abortS2;
extern bool abortS3;
extern bool abortS4;
extern sem_t* semS1;
extern sem_t* semS2;
extern sem_t* semS3;
extern sem_t* semS4;

#endif // __CONFIGURATION_H__