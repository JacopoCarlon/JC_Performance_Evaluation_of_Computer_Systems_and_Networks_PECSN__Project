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

#include "Queuer.h"
#include "Job_m.h"

Define_Module(Queuer);

int max_servers = 2;

void Queuer::initialize()
{
    jobsQueueTimeSig_   = registerSignal("lqtime");
    jobsQueueLenSig_    = registerSignal("lqlen");
    numJobsParkedSig_   = registerSignal("npark");

    genTimeSignal_ = registerSignal("genTimeSignal");
    lastSeen_= 0;

    // check if tValue parameter is defined for the parent module 
    occupied_servers_ = 0;
    
    pVal_ = par("probVal").intValue();
    pRange_ = par("probMax").intValue();
    
}

void Queuer::handleMessage(cMessage *msg)
{
    if(!par("hasServers").boolValue())
    {
        delete msg;
        return;
    }else{
        if(lastSeen_ != 0)
        {
            simtime_t diff = simTime() - lastSeen_;
            emit(genTimeSignal_,diff.dbl());
            EV << diff << endl;
        }
        lastSeen_ = simTime();

        cGate *arrivalGate = msg->getArrivalGate();

        Job* job = check_and_cast<Job*>(msg);
        
        if(arrivalGate == gate("jobIn")){
            // message from spawner
            // PUSH it in queue
            // (it will also try pop)
            handleArrivedJob(job);
        }else{
            //  //  if (gateSize("ctrlIn")>0){
            // message from server
            int src_gate = arrivalGate->getIndex();
            // if src_gate == 0, comes from server_0, means cg was |= 1
            // if src_gate == 1, comes from server_1, means cg was |= 2
            int value_saved = src_gate+1;
            // if i AND occupied_servers and value_saved and it is false, 
            // it means that i had not saved it as occupied, but i SHOULD HAVE DONE THAT EARLIER
            // i want the AND to be true

            // KILL JOB
            delete msg;

            EV << "num occupied_servers_ :"<< occupied_servers_ << endl;
            EV << "server_who_sent_IMFREE :"<< src_gate << endl;
            EV << "num bit_gate :"<< value_saved << endl;
            // if server was already free, something bad happened
            if ( !(occupied_servers_ & value_saved) ){
                throw cRuntimeError("Q.handleMessage - message from server: %i", src_gate);
            } 

            // update free servers
            // just do XOR with 1 or 2
            occupied_servers_ ^= value_saved;

            EV << "num occupied_servers_ :"<< occupied_servers_ << endl;
            // try pop (should succeed)
            bool sent_something = trySendJobFromQueue();
            if( !sent_something && jobsQueue_.getLength()){
                throw cRuntimeError("Q.handleMessage - did not pop after freed_msg from server: %i", src_gate);
            }
            //  //  }
        }
    }
    return;
}

// there is a new job from the Spawner, we push it in queue and try to pop 
void Queuer::handleArrivedJob(Job* job) {
    // DO PUSH
    job->setTimestamp();
    jobsQueue_.insert(job);

    // crasha qua : 
    emit(jobsQueueLenSig_, jobsQueue_.getLength());

    // CALL POP
    if (occupied_servers_ == 3){
        return;
    }else{
        // there is room to pop, we must pop
        bool sent_something = trySendJobFromQueue();
        if (!sent_something){
            throw cRuntimeError("Q.handleMessage - did not pop after freed_msg from SPAWNER");
        }
    }
}


// POP :
bool Queuer::trySendJobFromQueue(){
    if(!jobsQueue_.getLength()){
        return false;
    }

    if(occupied_servers_ == 3){
        return false;
    }      
    /*   
    //  https://stackoverflow.com/questions/61962790/is-it-possible-to-know-which-input-gate-triggered-the-handlemessage-method-of
    */
    EV << "num occupied_servers_ before send:"<< occupied_servers_ << endl;
    if(occupied_servers_ == 0){
        // entrambi gate liberi
        int this_rnd_val = uniform(0, pRange_);
        
        Job* job = check_and_cast<Job*>(jobsQueue_.pop());
        emit(jobsQueueLenSig_, jobsQueue_.getLength());

        simtime_t queue_time = simTime() - job->getTimestamp();
        emit(jobsQueueTimeSig_, queue_time);

        if( this_rnd_val < pVal_ ){
            send(job, "jobOut", 0);
            // set server_0 occupied
            occupied_servers_ |= 0x01;
            EV << "rng send to server_gate :"<< 0 << endl;
        } 
        else{
            send(job, "jobOut", 1);
            // set server_1 occupied
            occupied_servers_ |= 0x02;
            EV << "rng send to server_gate :"<< 1 << endl;
        }
        EV << "num occupied_servers_ after send:"<< occupied_servers_ << endl;
        return true;
    } else {
        // occupied_servers_ == 1 or 2
        // server_0 has the bit 0, and is sole occupied on 1
        // server_1 has the bit 1, and is sole occupied on 2
        int currently_occupied_gate = occupied_servers_ -1 ;

        // 1^3=2 ; 2^3=1
        int bit_to_send_to = occupied_servers_ ^ 0x03;

        // send to 0 or 1
        int gate_to_send_to = bit_to_send_to -1;

        Job* job = check_and_cast<Job*>(jobsQueue_.pop());
        emit(jobsQueueLenSig_, jobsQueue_.getLength());

        simtime_t queue_time = simTime() - job->getTimestamp();
        emit(jobsQueueTimeSig_, queue_time);

        send(job, "jobOut", gate_to_send_to);

        // !!!!!!   UPDATE OCCUPIED GATE !!!!!!!!!!!!
        // we are in 1 or 2, so now all are occupied !: 3
        occupied_servers_ = 0x03;
        EV << "send to server_gate :"<< gate_to_send_to << endl;
        EV << "num occupied_servers_ after send:"<< occupied_servers_ << endl;

        return true;
    }

}







