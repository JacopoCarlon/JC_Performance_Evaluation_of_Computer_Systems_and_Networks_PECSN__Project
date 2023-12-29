//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __J_P11_1Q2S_QUEUER_H_
#define __J_P11_1Q2S_QUEUER_H_

#include <omnetpp.h>
#include "Job_m.h"

using namespace omnetpp;

class Queuer : public cSimpleModule
{
    cQueue jobsQueue_;
    
    simsignal_t genTimeSignal_;
    simtime_t lastSeen_;

    simsignal_t jobsQueueTimeSig_ ;
    simsignal_t jobsQueueLenSig_  ; 
    simsignal_t numJobsParkedSig_ ;

    int pRange_;  // probability max value
    int pVal_;    // probability limes value, in range [0, pRange_]

    // default all free : 0 == [00]
    // if S_0 occupied  : 1 == [01]
    // if S_1 occupied  : 2 == [10]
    // if both occupied : 3 == [11]
    int occupied_servers_;  
 
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);
    void handleArrivedJob(Job* job);
    bool trySendJobFromQueue();

};

#endif
