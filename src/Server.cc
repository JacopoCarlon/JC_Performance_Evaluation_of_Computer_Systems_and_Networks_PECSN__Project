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

#include "Server.h"
#include "Job_m.h"

Define_Module(Server);
// this is a single-job single-thread server.

//  Server::Server()
//      : signalDone_(nullptr)
//  {}
//  
//  Server::~Server()
//  {
//      cancelAndDelete(signalDone_);
//  }



void Server::initialize()
{
    //  signalDone_ = new cMessage("jobDone");

    //  servTimer_ = new cMessage("serviceDoneTimer");
    recvJobSignal_ = registerSignal("recvJobSignal");
    completedJobSignal_ = registerSignal("completedJobSignal");
}

void Server::handleMessage(cMessage *msg)
{
    Job* job = check_and_cast<Job*>(msg);
    // server receives a job, 
    // and after some time it completes it and asks for a new job
    if(msg->isSelfMessage()){
        // it's the timer to finish the job
        //  handleCompleteJob( msg );
        EV << "Completed job in server" << endl;
        emit(completedJobSignal_,1);
        // send job to sink to die
        send(job, "jobOut");

        // send a signal to the Queuer that we are now free !
        Job* ctrlj = new Job("jobCompleted");
        send(ctrlj, "msgDone");
    } 
    else {
        // just received the job
        //  the queue is aware that the server is now occupied, 
        //  and will not send more jobs till we say we are free
        EV << "Received job from the Queuer" << endl;
        emit(recvJobSignal_,1);

        // prepare RV SERVICE TIME
        simtime_t time_service = job->getTso();
        scheduleAt(simTime() + time_service, msg);
        //  delete msg; 
        //  scheduleAt(simTime() + serTim, servTimer_);

    }

}














