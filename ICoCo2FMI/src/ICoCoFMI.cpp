/* ---------------------------------------------------------------------------*
 * Sample implementation of an FMU from an ICoCo component 
 * ---------------------------------------------------------------------------*/

// define class name and unique id
#include "define_model.h"
#include "ICoCoProblem.hxx" 
// include fmu header files, typedefs and macros
#include "fmuTemplatebis.h"

// include code that implements the FMI based on the above definitions
#include "fmuTemplatebis.c"
