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
//////////////////////////////////////////////////////////////////////////////
//
// File:        ProblemTest.cpp
//
//////////////////////////////////////////////////////////////////////////////

#include <ProblemTest.hxx>
#include <ICoCoExceptions.hxx>
#include <stdlib.h>
#include <iostream>

using ICoCo::Problem;
using ICoCo::ProblemTest;
using std::string;
using std::vector;


ProblemTest::~ProblemTest()
{
//	std::cout<<__func__<<std::endl;
}
////////////////////////////
//                        //
//   interface ProblemTest    //
//                        //
////////////////////////////

// When the dynamic library is loaded via dlopen(), getting a handle
// on this function is the only way to create a Problem object.

extern "C" Problem* getProblem()
{
  //std::cerr<<"coucou getProblem"<<std::endl;
  Problem* T=new ProblemTest;
  return T;
}

// Description:
// As initialize doesn't have any arguments, they can be passed to the Problem
// Precondition: None.
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// Problem instantiated

ProblemTest::ProblemTest()
{
}

void ProblemTest::setDataFile(const string& file)
{
//  (*my_params).data_file=file;
	_time = 0;
	_a = 1;
	_y= 0;
	_b =0;
	_dtmax=.1;
	_t0 = 1.;
}
void ProblemTest::setMPIComm(void* mpicomm)
{
}
// Description:
// This method is called once at the beginning, before any other one of
// the interface Problem.
// Precondition: Problem is instantiated, not initialized
// (*my_params) have been filled by constructor.
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// Problem initialized.
// Unknown and given fields are initialized at initial time
bool ProblemTest::initialize()
{
  return true;
}

// Description:
// This method is called once at the end, after any other one.
// It frees the memory and saves anything that needs to be saved.
// Precondition: initialize, but not yet terminate
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// The object is ready to be destroyed.
void ProblemTest::terminate()
{
}


///////////////////////////////////
//                               //
//   interface UnsteadyProblem   //
//                               //
///////////////////////////////////


// Description:
// Returns the present time.
// This value may change only at the call of validateTimeStep.
// A surcharger
// Precondition: initialize, not yet terminate
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: present time
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// ProblemTest unchanged
double ProblemTest::presentTime() const
{
  return _time;
}

// Description:
// Compute the value the Problem would like for the next time step.
// This value will not necessarily be used at the call of initTimeStep,
// but it is a hint.
// This method may use all the internal state of the Problem.
// Precondition: initialize, not yet terminate
// Parametre: stop
//    Signification: Does the Problem want to stop ?
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: The desired time step
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// ProblemTest unchanged
double ProblemTest::computeTimeStep(bool& stop) const
{
  return _dtmax;
}


// Description:
// This method allocates and initializes the unknown and given fields
// for the future time step.
// The value of the interval is imposed through the parameter dt.
// In case of error, returns false.
// Precondition: initialize, not yet terminate, timestep not yet initialized, dt>0
// Parametre: double dt
//    Signification: the time interval to allocate
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: bool
//    Signification: true=OK, false=error, not able to tackle this dt
//    Contraintes:
// Exception: WrongContext, WrongArgument
// Effets de bord:
// Postcondition:
// Enables the call to several methods for the next time step
bool ProblemTest::initTimeStep(double dt)
{
 _dt =dt;
 return true;
}


// Description:
// Calculates the unknown fields for the next time step.
// The default implementation uses iterations.
// Precondition: initTimeStep
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: bool
//    Signification: true=OK, false=unable to find the solution.
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// The unknowns are updated over the next time step.
bool ProblemTest::solveTimeStep()
{
  //explicit _yp = _y + _dt* (_a - _b * _y  );
#ifdef _step_
  if (_time+_dt<_t0)
	  _yp = _a;
  else
	  _yp = _b ;
  std::cout<<"solveTimeStep " << _time<<" t0 "<< _t0<< " step : " <<_yp << " a "<< _a <<std::endl;
#else
  std::cout<<"solveTimeStep " << _time<<" with dt "<< _dt<<  std::endl;
  _yp = (_y + _dt* _a)/(1 + _dt* _b );
#endif

  return true;
}


// Description:
// Validates the calculated unknown by moving the present time
// at the end of the time step.
// This method is allowed to free past values of the unknown and given
// fields.
// Precondition: initTimeStep
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// The present time has moved forward.
void ProblemTest::validateTimeStep()
{
 _y = _yp;
 _time += _dt;
  std::cout<<_time<<" "<< _y<< " res "<<  std::endl;
}

// Description:
// Tells if the Problem unknowns have changed during the last time step.
// Precondition: validateTimeStep
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: bool
//    Signification: true=stationary, false=not stationary
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// ProblemTest unchanged
bool ProblemTest::isStationary() const
{
  return false;
}

void ProblemTest::setStationaryMode(bool stationary)
{
  // TODO TODO FIXME
  std::cerr << "ProblemTest::setStationaryMode() not impl!" << std::endl;
  throw;
}

bool ProblemTest::getStationaryMode() const
{
  // TODO TODO FIXME
  std::cerr << "ProblemTest::getStationaryMode() not impl!" << std::endl;
  throw;
  return true;
}


// Description:
// Aborts the resolution of the current time step.
// Precondition: initTimeStep
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// Can call again initTimeStep with a new dt.
void ProblemTest::abortTimeStep()
{
  // TODO TODO FIXME
  std::cerr << "ProblemTest::abortTimeStep() not impl!" << std::endl;
  throw;
}

/////////////////////////////////////////////
//                                         //
//   interface IterativeUnsteadyProblem    //
//                                         //
/////////////////////////////////////////////

// Description:
// In the case solveTimeStep uses an iterative process,
// this method executes a single iteration.
// It is thus possible to modify the given fields between iterations.
// converged is set to true if the process has converged, ie if the
// unknown fields are solution to the problem on the next time step.
// Otherwise converged is set to false.
// The return value indicates if the convergence process behaves normally.
// If false, the ProblemTest wishes to abort the time step resolution.
// Precondition: initTimeStep
// Parametre: bool& converged
//    Signification: It is a return value :
//                   true if the process has converged, false otherwise.
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: bool
//    Signification: true=OK, false=unable to converge
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// The unknowns are updated over the next time step.
bool ProblemTest::iterateTimeStep(bool& converged)
{
	throw;
}

 #if 0
// Description:
// This method is used to find the names of input fields understood by the Problem
// Precondition: initTimeStep
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: vector<string>
//    Signification: list of names usable with getInputFieldTemplate and setInputField
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
// ProblemTest unchanged
vector<string> ProblemTest::getInputFieldsNames() const
{
  vector<string> v;
    v.push_back("a");
    v.push_back("b");
    v.push_back("dt_max");
  return v;
}

// Description:
// This method is used to get a template of a field expected for the given name.
// Precondition: initTimeStep, name is one of getInputFieldsNames
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
// afield contains a field with values neither allocated nor filled, describing the
// field expected by setInputField for that name.
// ProblemTest unchanged
void ProblemTest::getInputFieldTemplate(const std::string& name, TrioField& afield) const
{
  Nom nom(name.c_str());
  pb->getInputFieldTemplate(nom,afield);
}

// Description:
// This method is used to provide the Problem with an input field.
// Precondition: initTimeStep, name is one of getInputFieldsNames, afield is like in getInputFieldTemplate
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
// Values of afield have been used (copied inside the ProblemTest).
void ProblemTest::setInputField(const std::string& name, const TrioField& afield)
{
  Nom nom(name.c_str());
  pb->setInputField(nom,afield);
}
#endif

void ProblemTest::setInputDoubleValue(const std::string& name, const double& val)
{
	if (name=="a")
		_a=val;
	else if (name=="b")
		_b=val;
#ifdef _step_
	else if (name=="t0")
		_t0=val;
#endif
	else if (name=="dt_max")
		_dtmax=val;
	else
	{
		std::cout<<" try to set var "<< name<< " with "<<val<<std::endl;
		std::cout<< "ignored...."<<std::endl;
		//throw;
	}
#ifdef _step_
	if (_time==0) 
	{
          if (0<_t0)
	   _y = _a;
          else
	   _y = _b ;
	}
#endif
}
double ProblemTest::getOutputDoubleValue(const std::string& name) const
{
  if (name=="y")
    return _y;
 std::cout<<" Tyy to get "<< name<< " ... return -1e99"<<std::endl;
  return -1e99;
}
#if 0
vector<string> ProblemTest::getOutputFieldsNames() const
{

  // const Probleme_base& pb_base=ref_cast(Probleme_base,*pb);
  Noms my_names;
  pb->getOutputFieldsNames(my_names);
  vector<string> output_names;
  for (int i=0; i<my_names.size(); i++)
    output_names.push_back(my_names(i).getChar());
  return  output_names;

}

ICoCo::ValueType ProblemTest::getFieldType(const std::string& name) const
{
  vector<string> fld = getOutputFieldsNames();
  if (std::find(fld.begin(), fld.end(), name) == fld.end())
    {
      fld = getInputFieldsNames();
      if (std::find(fld.begin(), fld.end(), name) == fld.end())
        throw WrongArgument((*my_params).problem_name,"getFieldType","name","invalid field name!!");
    }
  // All fields thus far are Double field
  return ICoCo::ValueType::Double;
}

void ProblemTest::getOutputField(const std::string& name_, TrioField& afield) const
{
  Motcle name(name_.c_str());
  pb->getOutputField(name,afield);
}

// TODO: provide a more efficient version of this:
void ProblemTest::updateOutputField(const std::string& name, TrioField& afield) const
{
  getOutputField(name, afield);
}

void ProblemTest::getOutputMEDDoubleField(const std::string& name,MEDDoubleField& medfield) const
{
#ifndef NO_MEDFIELD
  TrioField  triofield;
  getOutputField(name,triofield);
  medfield= build_medfield(triofield);

#else
  throw NotImplemented("No ParaMEDMEM","getInputMEDDoubleField");
#endif
}

void ProblemTest::getInputMEDDoubleFieldTemplate(const std::string& name, MEDDoubleField& medfield) const
{
#ifndef NO_MEDFIELD
  TrioField  triofield;
  getInputFieldTemplate(name,triofield);
  medfield=build_medfield(triofield);
#else
  throw NotImplemented("No ParaMEDMEM","getInputMEDDoubleFieldTemplate");
#endif
}

void ProblemTest::setInputMEDDoubleField(const std::string& name, const MEDDoubleField& afield)
{
#ifndef NO_MEDFIELD
  // bof en attendant mieux
  TrioField  triofield;
  getInputFieldTemplate(name,triofield);
#ifdef OLD_MEDCOUPLING
  const ParaMEDMEM::DataArrayDouble *fieldArr=afield.getField()->getArray();
#else
  const MEDCoupling::DataArrayDouble *fieldArr=afield.getMCField()->getArray();
#endif
  triofield._field=const_cast<double*> (fieldArr->getConstPointer());
  // il faut copier les valeurs
  setInputField(name,triofield);
  triofield._field=0;
  //fieldArr->decrRef();

  // Assigning proper component names
  const unsigned int nbcomp = fieldArr->getNumberOfComponents();
  std::vector<std::string> compo_names = fieldArr->getInfoOnComponents();

  REF(Field_base) ch = pb->findInputField(Nom(name.c_str()));
  assert(nbcomp == (unsigned)ch->nb_comp());
  for (unsigned int i = 0; i < nbcomp; i++)
    {
      Nom compo_name(compo_names[i].c_str());
      ch->fixer_nom_compo((int)i, compo_name);
    }
#else
  throw NotImplemented("No ParaMEDMEM","setInputMEDDoubleField");
#endif
}

// TODO TODO FIXME do this more cleverly
void ProblemTest::updateOutputMEDDoubleField(const std::string& name, MEDDoubleField& medfield) const
{
#ifndef NO_MEDFIELD
  TrioField  triofield;
  getOutputField(name,triofield);
  medfield= build_medfield(triofield);

#else
  throw NotImplemented("No ParaMEDMEM","getInputMEDDoubleField");
#endif
}


int ProblemTest::getMEDCouplingMajorVersion() const
{
#ifdef OLD_MEDCOUPLING
  return 7;
#else
  return MEDCOUPLING_VERSION_MAJOR;
#endif
}

bool ProblemTest::isMEDCoupling64Bits() const
{
#ifdef OLD_MEDCOUPLING
  return false;
#else
  return sizeof(mcIdType) == 8;
#endif
}


#endif
