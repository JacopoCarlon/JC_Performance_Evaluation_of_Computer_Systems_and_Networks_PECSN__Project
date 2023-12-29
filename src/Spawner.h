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

#ifndef __J_P11_1Q2S_SPAWNER_H_
#define __J_P11_1Q2S_SPAWNER_H_

#include <omnetpp.h>

using namespace omnetpp;

class Spawner : public cSimpleModule
{
    cMessage* timer_;
    simtime_t planeExitedSig_;

    //  void scheduleNext(cMessage* timer);

  public:
    Spawner();
    ~Spawner();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage* msg);

    void handleNewSpawn();
};

// namespace

#endif
