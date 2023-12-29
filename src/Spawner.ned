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


simple Spawner
{
    parameters:
        volatile double tn @unit(s) = default(1min);
        volatile double tqi @unit(s) = default(1min);
        volatile double tqosi @unit(s) = default(1min);
        volatile double tso @unit(s) = default(1min);

        //  double maxGenerationTime @unit(s) = default(3s);
        //  double vMax = default(100);
        //  generate random value for assignment to Server_0 or Server_1, in range [0, vMax]

        @signal[exited];
        @statistic[planeExited](title="Job exiting at given time"; source="exited"; record=vector);
    
    gates:
        output jobOut;
}
