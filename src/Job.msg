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

// tqo : time_leave_queue 
// tsi : time_enter_server
// tqo : == tsi


// tn  : time to create job     // RV creation time

// tqi : time_enter_queue		// time-out time before entering queue : 0

// time_queue is occupied to send out one before sending out others 
// tqosi : elaboration time in queue before sending : 0

// time required to service the job in the server
// tso : time_leave_server		// RV servizio in server
//
//

//  namespace network;

message Job {
	//	double v;

	simtime_t tqi;   	
	simtime_t tqosi;
	simtime_t tso;
};