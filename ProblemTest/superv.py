#/****************************************************************************
# Copyright (c) 2021, CEA
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#****************************************************************************/

# Example of program loading Cathare and running a computation while

import ProblemTest 


def runUntil(P, tmax):
    stop = False  # // P wants to stop
    ok = False  # // P solveTimeStep was OK

    while (not stop):  # {                     // Loop on timesteps

        ok = False

        # {                // Loop on timestep size
        while ((not ok) and (not stop)):

            present = P.presentTime()
            # dt=P.computeTimeStep(stop)
            [dt, stop] = P.computeTimeStep()

#     // modify dt to stop at exactly tmax without restraining too much the time step.
#     // - modified dt will always be less than dt
#     // - and always more than min(dt,previous dt)/2
#      // where previous dt is the value returned by computeTimeStep for the last validated time step.
            if (present*(1+1e-5)>=tmax): # // dt/1000 is an epsilon to handle machine precision errors
                stop = True
            elif (present + dt > tmax):
                dt = tmax - present
            elif (present + 2 * dt > tmax):
                dt = (tmax - present) / 2

            if (stop):
                break

            P.initTimeStep(dt)

            ok = P.solveTimeStep()
            if (not ok):
                P.abortTimeStep()
            else:
                P.validateTimeStep()
        #}                                     // End loop on timestep size

         # }                                   // End loop on timesteps

if 1:
    C = ProblemTest.ProblemTest()
    C.setDataFile("toto")
    runUntil(C, 1)
    C.setInputDoubleValue("a",0.);
    runUntil(C,2);
    C.setInputDoubleValue("b",4.);
    runUntil(C,4.);
    C.terminate()
