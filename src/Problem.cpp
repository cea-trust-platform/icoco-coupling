//////////////////////////////////////////////////////////////////////////////
//
// File:        Problem.cpp
// Directory:   $TRUST_ROOT/Kernel/ICoCo
// Version:     /main/4
//
//////////////////////////////////////////////////////////////////////////////


// ICoCo file common to several codes
// Problem.cpp
// version 1.2 10/05/2010

#include <Problem.h>
#include <Exceptions.h>
#include <iostream>
using namespace ICoCo;
using std::string;
using std::vector;
using ParaMEDMEM::MEDCouplingFieldDouble;



///////////////////////////
//                       //
//   interface Problem   //
//                       //
///////////////////////////
Problem::Problem() { }
Problem::~Problem() { }


// Specify the data file used for this Problem.
// Precondition: Problem is not initialized.
// datafile: name of a datafile for the code.
void Problem::setDataFile(const string& datafile)
{
  throw NotImplemented("type_of_Problem_not_set","setDataFile");
}

// Specify the MPI communicator this Problem should use internally
// For sequential calculation, mpicomm is 0.
// Precondition: Problem is not initialized.
// mpicom: the MPI communicator to be used internally.
void Problem::setMPIComm(void* mpicomm)
{
  if (mpicomm!=0)
    throw NotImplemented("type_of_Problem_not_set","setMPIComm with comm<>0");
}

// This method must be called before any other one except
// setDataFile and setMPIComm.
// Precondition: Problem is not initialized.
//               setDataFile & setMPIComm should have been called.
// Return value: true=OK, false=error
bool Problem::initialize()
{
  throw NotImplemented("type_of_Problem_not_set","initialize");
  return false;
}

// This method is called once at the end, after any other one.
// It frees the memory and saves anything that needs to be saved.
// Precondition: initialize, but not yet terminate.
// Postcondition: the object is ready to be destroyed or initialized again.
void Problem::terminate()
{
  throw NotImplemented("type_of_Problem_not_set","terminate");
}


///////////////////////////////////
//                               //
//   interface UnsteadyProblem   //
//                               //
///////////////////////////////////


// Returns the present time.
// This value may change only at the call of validateTimeStep.
// Precondition: initialize, not yet terminate
double Problem::presentTime() const
{
  throw NotImplemented("type_of_Problem_not_set","presentTime");
  return 0;
}

// Compute the value the Problem would like for the next time step.
// This value must not necessarily be used for the next call to initTimeStep,
// but it is a hint.
// Precondition: initialize, not yet terminate
// stop: return value indicating if the Problem wants to stop
// return value: the desired time step
double Problem::computeTimeStep(bool& stop) const
{
  throw NotImplemented("type_of_Problem_not_set","computeTimeStep");
  return 0;
}

// This method allocates and initializes the future time step.
// The value of the interval is imposed through the parameter dt.
// Precondition: initialize, not yet terminate, timestep not yet initialized, dt>0
// dt: the time interval to allocate
// return value: true=OK, false=error, not able to tackle this dt
// Postcondition: Enables the call to several methods for the next time step
bool Problem::initTimeStep(double dt)
{
  throw NotImplemented("type_of_Problem_not_set","initTimeStep");
  return false;
}

// Calculates the unknown fields for the next time step.
// Precondition: initTimeStep
// Return value: true=OK, false=unable to find the solution.
// Postcondition: The unknowns are updated over the next time step.
bool Problem::solveTimeStep()
{
  throw NotImplemented("type_of_Problem_not_set","solveTimeStep");
  return false;
}

// Validates the calculated unknown by moving the present time
// at the end of the time step.
// This method can free past values of the fields (point of no return)
// Precondition: initTimeStep
// Postcondition: The present time has moved forward.
void Problem::validateTimeStep()
{
  throw NotImplemented("type_of_Problem_not_set","terminate");
}

// Tells if the Problem unknowns have changed during the last time step.
// Precondition: validateTimeStep, not yet initTimeStep
// Return value: true=stationary, false=not stationary
bool Problem::isStationary() const
{
  throw NotImplemented("type_of_Problem_not_set","isStationary");
  return false;
}

// Aborts the resolution of the current time step.
// Precondition: initTimeStep
// Postcondition: Can call initTimeStep again with a new dt.
void Problem::abortTimeStep()
{
  throw NotImplemented("type_of_Problem_not_set","abortTimeStep");
}

// In the case solveTimeStep uses an iterative process,
// this method executes a single iteration.
// It is thus possible to modify the given fields between iterations.
// converged is set to true if the process has converged, ie if the
// unknown fields are solution to the problem on the next time step.
// Otherwise converged is set to false.
// The return value indicates if the convergence process behaves normally.
// If false, the Problem wishes to abort the time step resolution.
// Precondition: initTimeStep
// converged:  return value, true if the process has converged, false if not yet
// return value: true=OK, false=unable to converge
// Postcondition: The unknowns are updated over the next time step.
bool Problem::iterateTimeStep(bool& converged)
{
  throw NotImplemented("type_of_Problem_not_set","iterateTimeStep");
  return false;
}

// Save the state of the Problem.
// Precondition: initialize, not yet terminate
// label : an int identifying the saved state for a future restore
// method : the saving method
// Postcondition: The state is saved and can be restored at any time
void Problem::save(int label, const string& method) const
{
  throw NotImplemented("type_of_Problem_not_set","save");
}

// Restores a previously saved state
// Precondition: initialize, not yet terminate, state has been saved
// label : an int identifying an already saved state
// method : the saving method
// Postcondition: The subsequent calls to any methods should give the exact same result
// as if they were made after the corresponding call to save.
void Problem::restore(int label, const string& method)
{
  throw NotImplemented("type_of_Problem_not_set","restore");
}

// Forgets a saved state
// Precondition: initialize, not yet terminate, state has been saved
// label : an int identifying an already saved state
// method : the saving method
// Postcondition: Memory/disk/... is freed as if the corresponding call to save had not been made.
void Problem::forget(int label, const string& method) const
{
  throw NotImplemented("type_of_Problem_not_set","forget");
}

// This method is used to find the names of input fields understood by the Problem
// Precondition: initTimeStep
// Return value: list of names usable with getInputFieldTemplate and setInputField
vector<string> Problem::getInputFieldsNames() const
{
  throw NotImplemented("type_of_Problem_not_set","getInputFieldsNames");
  vector<string> v;
  return v;
}

// This method is used to get a template of a field expected for the given name, i.e.
// a field similar to the one expected by setInputField with the same name.
// Precondition: initTimeStep, name is one of getInputFieldsNames
// name: the name of the input field
// afield: return value, a field similar to the one expected by setInputField
void Problem::getInputFieldTemplate(const string& name, TrioField& afield) const
{
  throw NotImplemented("type_of_Problem_not_set","getInputFieldTemplate");
}

// This method is used to provide the Problem with an input field.
// Precondition: initTimeStep, name is one of getInputFieldsNames, afield is like in getInputFieldTemplate
// name: the name of the input field
// afield: the input field
// Postcondition: Values of afield have been used (copied inside the Problem).
void Problem::setInputField(const string& name, const TrioField& afield)
{
  throw NotImplemented("type_of_Problem_not_set","setInputField");
}

// This method is used to find the names of output fields understood by the Problem
// It is not implemented and returns a void list (it would be very
// difficult to list all cathare computation variables...)
// Precondition: initTimeStep
// Return value: list of names usable with getOutputField
vector<string> Problem::getOutputFieldsNames() const
{
  throw NotImplemented("type_of_Problem_not_set","getOutputFieldsNames");
  vector<string> v;
  return v;
}

// This method is used to get an output field for the given name
// Precondition: initTimeStep, name is one of getOutputFieldsNames
// name : the name of the output field
// afield: return value, the output field, with geometry and values filled
void Problem::getOutputField(const string& name, TrioField& afield) const
{
  throw NotImplemented("type_of_Problem_not_set","getOutputField");
}

// This method is similar to getInputFieldTemplate but with a MEDCoupling field
// Precondition: initTimeStep, name is one of getInputFieldsNames
// name: the name of the input field
// return value: a field similar to the one expected by setInputField
MEDCouplingFieldDouble* Problem::getInputMEDFieldTemplate(const string& name) const
{
  throw NotImplemented("type_of_Problem_not_set","getInputMEDFieldTemplate");
  return 0;
}

// This method is similar to setInputField but with a MEDCoupling field
// Precondition: initTimeStep, name is one of getInputFieldsNames, afield is like in getInputFieldTemplate
// name: the name of the input field
// afield: the input field
// Postcondition: Values of afield have been used (copied inside the Problem).
void Problem::setInputMEDField(const string& name, const MEDCouplingFieldDouble* afield)
{
  throw NotImplemented("type_of_Problem_not_set","setInputMEDField");
}

// This method is similar to getOutputField but with a MEDCoupling field
// Precondition: initTimeStep, name is one of getOutputFieldsNames
// name : the name of the output field
// return value: the output field, with geometry and values filled
MEDCouplingFieldDouble* Problem::getOutputMEDField(const string& name) const
{
  throw NotImplemented("type_of_Problem_not_set","getOutputMEDField");
  return 0;
}


