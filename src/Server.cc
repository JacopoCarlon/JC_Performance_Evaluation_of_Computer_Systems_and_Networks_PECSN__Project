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

// namespace j_net {

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
    //  recvJobSignal_ = registerSignal("recvJobSignal");
    completedJobSignal_ = registerSignal("completedJobSignal");
}

void Server::handleMessage(cMessage *msg)
{
    Job* job = check_and_cast<Job*>(msg);
    // server receives a job, 
    // and after some time it completes it and asks for a new job
    if(msg->isSelfMessage()){
        if(job->getKind() == jobWaitOutServer){
            // it's the timer to finish the job
            //  handleCompleteJob( msg );
            //  EV << "SRV Completed job in server" << endl;

            // signal of <working time in server>
            simtime_t exit_time = simTime() - job->getTimestamp();
            emit(completedJobSignal_, exit_time);
            // send job to SINK to die
            job->setKind(jobOutServerToSink);
            send(job, "jobOut");

            // send a signal to the Queuer that we are now free !
            Job* ctrlj = new Job("jobCompleted", controlMessage);
            send(ctrlj, "msgDone");
            return;
        }
        return;
    } 
    else {
        // just received the job
        //  the queue is aware that the server is now occupied, 
        //  and will not send more jobs till we say we are free
        if(job->getKind() == jobOutQueueToServer){
            //  EV << "SRV Received job from the Queuer" << endl;
            //  emit(recvJobSignal_,1);

            // istante di ricezione
            job->setTimestamp();
            job->setKind(jobWaitOutServer);

            // prepare RV SERVICE TIME
            simtime_t time_service = job->getTso();

            int slowerness = par("slowness_multiplier");
            if(slowerness > 1){
                time_service = time_service*slowerness;
            }
            scheduleAt(simTime() + time_service, msg);
            //  delete msg; 
            //  scheduleAt(simTime() + serTim, servTimer_);
            return;
        }
        return;
    }

}


// } /* j_namespace */



