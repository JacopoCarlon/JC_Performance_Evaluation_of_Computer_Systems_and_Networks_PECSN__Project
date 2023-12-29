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

import ned.IdealChannel;


module Operators
{
    parameters:
        int tValue = default(50);
        
    gates:
        input jobIn;
        
    submodules:
        queuer: Queuer {
            hasServers = true;
            @display("p=256,128");
        }
        server_0: Server {
            @display("p=384,128");
        }
        server_1: Server {
            @display("p=384,256");
        }
    
    connections:
        jobIn --> queuer.jobIn;
        queuer.jobOut++ --> IdealChannel --> server_0.jobIn;
        queuer.jobOut++ --> IdealChannel --> server_1.jobIn;
}
