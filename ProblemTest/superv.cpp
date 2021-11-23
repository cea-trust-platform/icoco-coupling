/****************************************************************************
* Copyright (c) 2021, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

// Example of program loading Cathare and running a computation while
#include <ICoCoProblem.hxx>
#include <dlfcn.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;
using ICoCo::Problem;

// Open a shared library
// Find the function getProblem
// Run it and return a pointer to the Problem
// Prints the errors if any

Problem* openLib(const char* libname, void* & handle) {
    Problem *(*getProblem)();
    handle=dlopen(libname, RTLD_LAZY | RTLD_LOCAL);
    if (!handle) {
        cout << libname << " Y a une erreur :-( " << endl;
        cerr << dlerror() << endl;
        throw 0;
    }
    getProblem=(Problem* (*)())dlsym(handle, "getProblem");
    if (!getProblem) {
        cout << dlerror() << endl;
        throw 0;
    }
    return (*getProblem)();
}

// Close a shared library
// Prints the errors if any

void closeLib(void* handle) {
    if (dlclose(handle)) {
        cout << dlerror() << endl;
        throw 0;
    }
}

// Performs a simple time loop on Problem P :
// Uses the timestep suggested by P
// Modifies it to stop at tmax

void runUntil(Problem *P, double tmax,int strict=0 ) {
    bool stop=false;  // P wants to stop
    bool ok=false;     // P solveTimeStep was OK

    while (!stop) {                     // Loop on timesteps

        ok=false;

        while (!ok && !stop) {                // Loop on timestep size

            double present=P->presentTime();
            double dt=P->computeTimeStep(stop);

            // modify dt to stop at exactly tmax without restraining too much the time step.
            // - modified dt will always be less than dt
            // - and always more than min(dt,previous dt)/2
            // where previous dt is the value returned by computeTimeStep for the last validated time step.
            if (present*(1+1e-5)>=tmax) // dt/1000 is an epsilon to handle machine precision errors
                stop=true;
            else if (1) {
                if (present+dt>tmax)
                    dt=tmax-present;
                else if (present+2*dt>tmax)
                    dt=(tmax-present)/2;
            }
            if (dt<0)
                stop=true;
            if (stop)
                break;
            P->initTimeStep(dt);

            ok=P->solveTimeStep();
            if (!ok) {
                P->abortTimeStep();
            }
            else
                P->validateTimeStep();
//     if (present+dt>tmax) // dt/1000 is an epsilon to handle machine precision errors
//    	stop=true;
        }                                     // End loop on timestep size

    }                                   // End loop on timesteps

}

int main(int argc, char** argv) {

    void* handle;
    Problem *C;
    C=openLib("./libTest.so",handle);
    C->setDataFile("Test.dat");
    C->initialize();
    runUntil(C,1);
    C->setInputDoubleValue("a",0.);
    runUntil(C,2);
    C->setInputDoubleValue("b",4.);
    runUntil(C,4.);
    C->terminate();
    delete C;
    closeLib(handle);
    return 0;

}
