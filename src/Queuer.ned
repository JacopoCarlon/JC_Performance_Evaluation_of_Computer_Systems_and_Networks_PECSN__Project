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

package j_p11_1q2s;

simple Queuer
{
    parameters:
        bool hasServers = default(false);

        int probVal = default(50);
        int probMax = default(100);

        // used
        @signal[jqtime];
        @statistic[inQueueTime](title="Time of the entering jobs queue"; source="jqtime"; record=stats,vector);

        // used
		@signal[jqlen];
        @statistic[jobsQueueLen](title="Length of the jobs queue"; source="jqlen"; record=stats,timeavg,vector);

        // used
        @signal[genTimeSignal];
        @statistic[genTime](source="genTimeSignal"; record=mean,vector);
		
        //  //  //  @signal[npark];
        //  //  //  @statistic[numJobsParked](title="Number of planes in the parking area"; source="npark"; record=stats,timeavg,vector);


    gates: 
        input jobIn;
        output jobOut[];
        input ctrlIn[];
}
