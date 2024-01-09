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


#include "Spawner.h"

// namespace j_net {

Define_Module(Spawner);


Spawner::Spawner()
    : timer_(nullptr)
{
}

Spawner::~Spawner()
{
    cancelAndDelete(timer_);
}


void Spawner::initialize()
{
    timer_ = new cMessage("generationTimer");
    //  maxGenTime_ = par("maxGenerationTime");

    simtime_t tn = par("tn");
    //  jobExitedSig_ = registerSignal("exited");


    scheduleAt(simTime() + tn, timer_); 
    // -> after some time, sends a message which will be managed by handleMessage
}


void Spawner::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage()){
        // here there could be a delay between spawning and entering queue <tqi>, 
        // but we keep it zero .
        handleNewSpawn();
    }
    else{
        // should not arrive here
        return;
    }
    
}


void Spawner::handleNewSpawn()
{
    // job begins as a jobWaitOutSpawner, 
    // but the RV is taken from tn so it is not needed
    Job* newJob = new Job("job", jobOutSpawnerToQueue);
    newJob->setTqi(par("tqi"));         //  0
    newJob->setTqosi(par("tqosi"));     //  0
    newJob->setTso(par("tso"));         // exp or uni
    //  newJob->setSchedulingPriority(0);

    send(newJob, "jobOut");

    simtime_t tn = par("tn");           // exp or uni
    scheduleAt(simTime() + tn, timer_);
}





// } /* j_namespace */

