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

package j_p11_1q2s.simulations;

import j_p11_1q2s.Queuer;
import j_p11_1q2s.Server;
import j_p11_1q2s.Sink;
import j_p11_1q2s.Spawner;
import ned.IdealChannel;


network J_Network
{
    @display("bgb=640,384");
    submodules:
        spawner: Spawner {
            @display("p=128,128");
        }
        queuer: Queuer {
            @display("p=256,128");
        }
        server_0: Server {
            @display("p=384,128");
        }
        server_1: Server {
            @display("p=384,256");
        }
        sink_0: Sink {
            @display("p=512,128");
        }
        sink_1: Sink {
            @display("p=512,256");
        }
    connections:
        spawner.jobOut --> IdealChannel --> queuer.jobIn;

        queuer.jobOut++ --> IdealChannel --> server_0.jobIn;
        queuer.jobOut++ --> IdealChannel --> server_1.jobIn;

        server_0.msgDone --> IdealChannel --> queuer.ctrlIn++;
        server_1.msgDone --> IdealChannel --> queuer.ctrlIn++;
        
        server_1.jobOut --> IdealChannel --> sink_1.jobIn;
        server_0.jobOut --> IdealChannel --> sink_0.jobIn;
}
