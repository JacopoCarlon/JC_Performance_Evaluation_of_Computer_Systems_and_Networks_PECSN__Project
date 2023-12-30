#ifndef __COMMON_H_
#define __COMMON_H_

// namespace j_net {

enum : short {
    jobOutSpawnerToQueue,
    jobWaitOutQueue,
    jobOutQueueToServer,
    jobWaitOutServer, 
    jobOutServerToSink,
    controlMessage
};

// } /* j_namespace */

#endif /*__COMMON_H_*/