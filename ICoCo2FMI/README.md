# How to build a FMU Component from a ICoCo library ?


## Two aims -> two norms 



ICoCo stands for **Interface for Code Coupling**. This is a norm that a code
 may implement to facilitate its coupling with other ICoCo-compliant codes.
 This interface allows complex fields manipulations (see MEDCoupling library) 
and can implements exchanges inside a timestep (between iterations).
 See https://github.com/cea-trust-platform/icoco-coupling for full reference.

FMI stands for **Functional Mock-up Interface**. 
See https://fmi-standard.org/  for full reference.


## Generating FMI interface version 2.0 for CoSimulation 


In order to generate a fmu component, we need:

   * a name for the component

   * a library containing the ICoCo implementation

   * a data file (see ICoCo::Problem::setDatafile)

   * a file describing the in/out variables (see varsfile description)


Using these information and the <code>ICoCo2FMI.sh</code> script, we can 
create a my_component.fmu file (once generated this component doesn't need 
the data file and the varsfile anymore).
This can be performed via the following commands:

```
git clone --branch ICoCo2FMI https://github.com/cea-trust-platform/icoco-coupling
icoco-coupling/ICoCo2FMI/ICoCo2FMI.sh --datafile path_to_datafile --varsfile path_to_varsfile  \
                                      --name my_component --library path_to_lib
```


## varsfile description 

The varsfile file is a json file (JavaScript Object Notation, https://en.wikipedia.org/wiki/JSON).

This file containing the list of the input variables and the output variables, in the vars table.

For each variable, we need:

 * the name (according to ICoCo::Problem::getOutputDoubleValue)

 * the type (Real, Integer, or String)

 * a description

 * the inout flag (output or input)


From this file, the script <code>ICoCo2FMI.sh</code> (called in the previous section) 
has generated the modelDescription.xml file and a part of the sources for the component.

### Example of json file:



```json
{
  "vars": [
    {
      "varname": "OUTVAR",
      "type": "Real",
      "description": "ouput var",
      "inout": "output"
    },
    {
      "varname": "OUTVAR2",
      "type": "Real",
      "description": "ouput var2",
      "inout": "output"
    },
    {
      "varname": "flag",
      "type": "Integer",
      "description": "modif flux",
      "inout": "input"
    }
  ]
}
```

In this case, we have:

* a integer input variable (flag) 
* two real ouput variables (OUTVAR, OUTVAR2) 



Part of the xml generated:

```xml
<ModelVariables>
  <ScalarVariable name="flag"
    description="modif flux"
    valueReference="0"
    causality="input" 
    variability="discrete" >
    <Integer start="0"/>
  </ScalarVariable>
<ScalarVariable name="OUTVAR"
       description="ouput var"
       valueReference="0"
       causality="output" 
       variability="discrete" 
       initial='exact'>
       <Real start="0" />
  </ScalarVariable>
<ScalarVariable name="OUTVAR2"
       description="ouput var2"
       valueReference="1"
       causality="output" 
       variability="discrete" 
       initial='exact'>
       <Real start="0" />
  </ScalarVariable>
</ModelVariables>
```

## Test 

The component generated has been tested with fmuCheck.linux64 
(see https://github.com/modelica-tools/FMUComplianceChecker) 
and fmpy simulate (see https://fmpy.readthedocs.io/en/latest/).

Example:

```
   fmuCheck.linux64  -s 100. -h 0.1 -i file_in.csv  my_component.fmu 
   fmpy simulate  --stop-time 100.  --output-interval 0.1 --show-plot  --input-file file_in.csv \
                                        --fmi-logging --output-file res.csv  my_component.fmu
 ``` 

##  methods implemented 

* fmi2Instantiate (call ICoCo::Problem::setDataFile and  ICoCo::Problem::initialize)
* fmi2SetupExperiment 
* fmi2EnterInitializationMode
* fmi2ExitInitializationMode

* fmi2GetReal/fmi2SetReal (call ICoCo::Problem::getOutputDoubleValue/ICoCo::Problem::setInputDoubleValue)
* fmi2GetInteger/fmi2SetInteger (call ICoCo::Problem::getOutputIntValue/ICoCo::Problem::setInputIntValue)
* fmi2GetString/fmi2SetString (call ICoCo::Problem::getOutputStringValue/ICoCo::Problem::setInputStringValue)

* fmi2DoStep (call ICoCo::Problem::computeTimeStep, ICoCo::Problem::initTimeStep,
       ICoCo::Problem::solveTimeStep, ICoCo::Problem::validateTimeStep)

* fmi2Terminate
* fmi2FreeInstance  (call ICoCo::Problem:terminate)




The FMI interface is more detailed than the ICoCo Interface for the initialization part, and less for the transient part.
