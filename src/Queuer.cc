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

// namespace j_net {

Define_Module(Queuer);

int max_servers = 2;
int servers_bits = 0;

void Queuer::initialize()
{
    jobsQueueTimeSig_   = registerSignal("lqtime");
    jobsQueueLenSig_    = registerSignal("lqlen");
    numJobsParkedSig_   = registerSignal("npark");

    genTimeSignal_ = registerSignal("genTimeSignal");
    lastSeen_= 0;

    for(int i = 0; i< max_servers; i++){
        servers_bits |= (1<<i);
    }
    // check if tValue parameter is defined for the parent module 
    occupied_servers_ = 0;
    EV << "num occupied_servers_ :"<< occupied_servers_ << endl;
    EV << "num max_servers :"<< max_servers << endl;        // should be 2
    EV << "num servers_bits :"<< servers_bits << endl;      // should be 3
    
    pVal_ = par("probVal").intValue();
    pRange_ = par("probMax").intValue();
    
}

void Queuer::handleMessage(cMessage *msg)
{
    if(!par("hasServers").boolValue()) {
        delete msg;
        return;
    } else  {
        Job* job = check_and_cast<Job*>(msg);     
        if(msg->isSelfMessage()) 
        {
            if(job->getKind() == jobWaitOutQueue){
                handleSendJobToServer(job);
            }
            return;
        } 
        else // message to gate
        {
            cGate *arrivalGate = msg->getArrivalGate();
            if(arrivalGate == gate("jobIn")){
                // type is jobOutSpawnerToQueue : a new job
                if(job->getKind() == jobOutSpawnerToQueue){
                    // PUSH it in queue
                    // (it will also try pop)
                    handleArrivedJob(job);
                } 
                return;
            } else {
                    // type is controlMessage
                if(job->getKind() == controlMessage){
                    // CONTROL message from server
                    // we must kill it
                    int src_gate = arrivalGate->getIndex();
                    // if src_gate == 0, comes from server_0, means cg was |= 1
                    // if src_gate == 1, comes from server_1, means cg was |= 2
                    int value_saved = src_gate+1;
                    // if i AND occupied_servers and value_saved and it is false, 
                    // it means that i had not saved it as occupied, but i SHOULD HAVE DONE THAT EARLIER
                    // i want the AND to be true

                    // KILL JOB
                    delete job;

                    EV << "CTRL num occupied_servers_ :"<< occupied_servers_ << endl;
                    EV << "CTRL server_who_sent_IMFREE :"<< src_gate << endl;
                    //  EV << "num bit_gate :"<< value_saved << endl;
                    // if server was already free, something bad happened
                    if ( !(occupied_servers_ & value_saved) ){
                        throw cRuntimeError("Q.handleMessage - message from server: %i", src_gate);
                    } 

                    // update free servers
                    // just do XOR with 1 or 2
                    occupied_servers_ ^= value_saved;

                    EV << "CTRL num occupied_servers_ :"<< occupied_servers_ << endl;
                    // try pop (should succeed)
                    bool try_send_something = trySendJobFromQueue();
                    if( !try_send_something && jobsQueue_.getLength()){
                        throw cRuntimeError("Q.handleMessage - did not pop after freed_msg from server: %i", src_gate);
                    }
                }
                return;
            }
        }
    }
}

// there is a new job from the Spawner, we push it in queue and try to pop 
void Queuer::handleArrivedJob(Job* job) {
    // DO PUSH
    job->setTimestamp();
    jobsQueue_.insert(job);

    emit(jobsQueueLenSig_, jobsQueue_.getLength());
    EV << "QQQQQQLEN after job added is :"<< jobsQueue_.getLength() << endl;

    // CALL POP
    if(occupied_servers_ == servers_bits){
        return;
    }else{
        // there is room to pop, we must pop
        bool try_send_something = trySendJobFromQueue();
        if (!try_send_something){
            throw cRuntimeError("Q.handleMessage - did not pop after freed_msg from SPAWNER");
        }
    }
}


// try POP :
bool Queuer::trySendJobFromQueue(){
    if(!jobsQueue_.getLength()){
        return false;
    }

    if(occupied_servers_ == servers_bits ){
        return false;
    }      
    /*   
    //  https://stackoverflow.com/questions/61962790/is-it-possible-to-know-which-input-gate-triggered-the-handlemessage-method-of
    */
    EV << "QLEN : QueueLen before POP is:" << jobsQueue_.getLength() << endl;
    Job* job = check_and_cast<Job*>(jobsQueue_.pop());
    emit(jobsQueueLenSig_, jobsQueue_.getLength());

    EV << "QLEN : QueueLen after POP is:" << jobsQueue_.getLength() << endl;

    simtime_t queue_time = simTime() - job->getTimestamp();
    emit(jobsQueueTimeSig_, queue_time);
    
    job->setKind(jobWaitOutQueue);

    // time required to leave queue and reach server
    // default is 0
    scheduleAt(simTime() + job->getTqosi(), job);

    return true;
}


void Queuer::handleSendJobToServer(Job*job){
    
    job->setKind(jobOutQueueToServer);

    if(occupied_servers_ == 0)
    {
        EV << "RRRNG num occupied_servers_ before send:"<< occupied_servers_ << endl;

        // entrambi gate liberi
        int this_rnd_val = uniform(0, pRange_);
        
        // this part is hardcoded for 2 servers for now
        if( this_rnd_val < pVal_ ){
            EV << "RRRNG send to server_gate :"<< 0 << endl;
            send(job, "jobOut", 0);
            // set server_0 occupied
            occupied_servers_ |= 0x01;
        } 
        else{
            EV << "RRRNG send to server_gate :"<< 1 << endl;
            send(job, "jobOut", 1);
            // set server_1 occupied
            occupied_servers_ |= 0x02;
        }
        EV << "RRRNG num occupied_servers_ after send:"<< occupied_servers_<<" QLEN now is:"<< jobsQueue_.getLength() << endl;

        // now there is at-the-least another server still free !!!
        // try sending another job !!
        if( (occupied_servers_ != servers_bits) && (jobsQueue_.getLength() > 0) ){
            EV << "RRRNG double down ; occupied_servers_ is :"<< occupied_servers_ << endl;
            trySendJobFromQueue();
        }
        return;
    } 
    else 
    {
        EV << "NORMAL num occupied_servers_ before send:"<< occupied_servers_ << endl;
        // occupied_servers_ == 1 or 2
        // server_0 has the bit 0, and is sole occupied on 1
        // server_1 has the bit 1, and is sole occupied on 2
        //  int currently_occupied_gate = occupied_servers_ -1 ;
        // 1^3=2 ; 2^3=1
        int bit_to_send_to = occupied_servers_ ^ servers_bits;
        // send to 0 or 1
        int gate_to_send_to = bit_to_send_to -1;

        //  EV << "NORMAL value of server_bits : :"<< servers_bits << endl;
        EV << "NORMAL send to server_gate :"<< gate_to_send_to << endl;

        if(gate_to_send_to < 0){
            EV << "BAD : about to send to negative gate" << endl;
            EV << "BAD : current QueueLen is:" << jobsQueue_.getLength() << endl;
            EV << "BAD : occupied_servers_ :"<< occupied_servers_ << endl;
            EV << "BAD : servers_bits :"<< servers_bits << endl;
            EV << "BAD : bit_to_send_to :"<< bit_to_send_to << endl;
            EV << "BAD : gate_to_send_to :"<< gate_to_send_to << endl;
            // i want it to crash so i can read
            //send(job, "jobOut", gate_to_send_to);
            return;
        }
        send(job, "jobOut", gate_to_send_to);

        // !!!!!!   UPDATE OCCUPIED GATE !!!!!!!!!!!!
        // we are in 1 or 2, so now all are occupied !: 3
        occupied_servers_ |= bit_to_send_to;
        EV << "NORMAL num occupied_servers_ after send:"<< occupied_servers_ << endl;

        if( (occupied_servers_ != servers_bits) && (jobsQueue_.getLength() > 0) ){
            // there is at-the-least another server still free!
            EV << "SHOULD_NOT_BE_HERE num occupied_servers_ after send:"<< occupied_servers_ << endl;
            EV << "SHOULD_NOT_BE_HERE : current QueueLen is:" << jobsQueue_.getLength() << endl;
            EV << "SHOULD_NOT_BE_HERE : occupied_servers_ :"<< occupied_servers_ << endl;
            EV << "SHOULD_NOT_BE_HERE : servers_bits :"<< servers_bits << endl;
            trySendJobFromQueue();
        }
        return;
    }
    return;
}



//  /*
//  if(lastSeen_ != 0)
//          {
//              simtime_t diff = simTime() - lastSeen_;
//              emit(genTimeSignal_,diff.dbl());
//              EV << diff << endl;
//          }
//          lastSeen_ = simTime();
//  */


// } /* j_namespace */
