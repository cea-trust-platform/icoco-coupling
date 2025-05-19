// ICoCo file common to several codes
// Version 3 -- 03/2025
//
// WARNING: this file is part of the official ICoCo API and should not be modified.
// The official version can be found at the following URL:
//
//    https://github.com/cea-trust-platform/icoco-coupling

#ifndef ICoCoProblem_included
#define ICoCoProblem_included

#ifdef ICOCO_VERSION
#   error "ICOCO_VERSION already defined!! Are you including twice two versions of the ICoCo interface?"
#else
#   define ICOCO_VERSION "3.0"
#   define ICOCO_MAJOR_VERSION 3
#   define ICOCO_MINOR_VERSION 0
#endif

#include <vector>
#include <string>

/*! @brief The namespace ICoCo (Interface for code coupling) encompasses all the classes
 * and methods needed for the coupling of codes.
 * See the class Problem and the function getProblem() to start with.
 */
namespace ICoCo
{
  class MEDDoubleField;
  class MEDIntField;
  class MEDStringField;
  class MEDDoubleArray;
  class MEDIntArray;
  class MEDStringArray;
  class AlgebraicData;

  /*! @brief The various possible types for fields or scalar values.
   */
  enum class ValueType  // ### TODO: Expliciter les valeurs des enums permet de les reprendre en Python sans trop de risque. ###
  {
    Double = 0,  ///< Double scalar value or field type
    Int = 1,     ///< Int scalar value or field type
    String = 2   ///< String scalar value or field type
  };

  /*! @brief API that a code has to implement in order to comply with the ICoCo (version 3) norm.
   *
   * This abstract class represents the methods that a given code may implement to comply (partially or fully) to the
   * ICoCo standard. For organization and documentation purposes the interface is separated into several sections but
   * this does not correspond to any code constraint.
   * Note that not all the methods need to be implemented! Notably the methods belonging to the sections
   *   - Restorable
   *   - Field I/O
   *   - Scalar values I/O
   * are not always needed since a code might not have any integer field to work with for example.
   * Consequently, default implementation for all methods of this interface is to raise an ICoCo::NotImplemented
   * exception. ### TODO: ne devrait-on pas mettre en virtuelle pure les quelques methodes obligatoires? ###
   *
   *
   * Some of the methods may not be called (or have a different behavior) when some conditions are not met. Thus
   * in this documentation we define the "CALCULATION_DEFINED context" as the context that the code finds itself, when the
   * method initTimeStep() or initStationary() has been called, and the method validateTimeStep() / validateStationary()
   * (or abortTimeStep() / abortStationary()) has not yet been called. A CALCULATION_DEFINED context opened with initTimeStep()
   * is also called TIME_STEP_DEFINED and a CALCULATION_DEFINED context opened with initStationary() is also called
   * STATIONARY_DEFINED.
   *
   * Fields and scalar values that are set within the CALCULATION_DEFINED context are invalidated (undefined behavior) after
   * the context has been closed. They need to be set at each calculation. However, fields and scalar values that are set outside
   * of this context (before the first time step for example, or after the resolution of the last time step) are permanent
   * (unless modified afterward).
   *
   * Within the CALCULATION_DEFINED context, calling a solving methods (solveTimeStep(), iterateTimeStep(), solveStationary(), or
   * iterateStationary()) updates available scalar and field outputs, even if validation (validateTimeStep() or
   * validateStationary()) has not been called yet.
   *
   *
   * Objects returned by ICoCo, WITH THE EXCEPTION OF MED FIELD UNDERLYING MESHES, are the responsability of the caller. They
   * can be freely modified or deleted by the caller. This includes the "AlgebraicData". Field underlying meshes, however,
   * may not be copied when a field is get (only a reference on an internal MED mesh may be provided to the output field).
   * The caller must therefore refrain from deleting or modifying them.
   *
   *
   * Some codes solve a large number of equations, and it can be useful to drive the resolution of these equations separately.
   * In this case, a first solution may be to expose completely separated ICoCo interfaces. Solving each set of equations then
   * behaves like a different “code”. However, this is not always possible to go that far, and for this reason we introduce the
   * notion of mode. For example, a code solving equations A and B may exhibit modes “A”, “B” (these modes are referred to
   * hereafter as elementary) and “A + B” (compound mode, solving the coupling between A and B). All possible calculation types
   * must have an elementary mode (in the previous example, mode “B” is mandatory if “A” and “A + B” exist).
   *
   * A mode is selected by setting a scalar value (int or string). It should not be possible to select a compound mode whose
   * elementary modes are not synchronized (same presentTime(), see below).
   *
   * The behavior of the resolution methods (sections StationaryManagement and TimeStepManagement with the exception of
   * setStationaryMode() and getStationaryMode()) must depend on the selected mode (have no impact on elementary modes not
   * included in the selected mode). In particular, presentTime() depends on the selected mode. For example, if mode “A” is
   * selected just after initialize(), and three time steps of duration 1 are solved in this mode, presentTime() in mode A
   * must return 3, but presentTime() in mode B must return 0.
   *
   * The behavior of restorable methods (save, restore, forget) may also depend on the mode. It can also be the case for some
   * get / set features.
   *
   * A mode can be selected either within or outside the CALCULATION_DEFINED context. However, a mode can be selected within the
   * CALCULATION_DEFINED context only if it was already included in the (compound) mode selected when the context was opened.
   * For example, inside the CALCULATION_DEFINED context, we can switch between modes "A" and "B" if "A + B" was selected when
   * the context was opened. The context can only be closed in the mode selected when opened.
   *
   *
   * In addition to well defined data types (fields, arrays and scalar), ICoCo also introduces the vague type "AlgebraicData".
   * The rational is to offer the caller the possibility of accelerating an internal iterative process from the outside.
   * These data can be manipulated without their exact nature needing to be known, and then re-injected into the code that
   * produced them. It is therefore the responsibility of codes providing this kind of data to be able to re-read them.
   *
   *
   * Within the computation of a time step (so within TIME_STEP_DEFINED context), the temporal semantics of data (any kind of
   * data like fields or scalars) is not imposed by the norm. Said differently, it does not require the data to be defined at the
   * start/middle/end of the current time step, this semantics must be agreed on between the codes being coupled.
   * Methods get...TimeSemantics() can be implemented to help achieve this agreement.
   *
   *
   * Finally, the ICoCo interface may be wrapped in Python using SWIG or PyBind11. For an example of the former see the
   * TRUST implementation of ICoCo. Notably the old methods returning directly MEDCoupling::MEDCouplingFieldDouble objects
   * (version 1.x of ICoCo) are easily re-instanciated in Python SWIG.
   */
  class Problem
  {

  public :
    /*! @brief Return ICoCo interface major version number.
     * @return ICoCo interface major version number (3 at present)
     */
    static int GetICoCoMajorVersion() { return ICOCO_MAJOR_VERSION; }

    // ******************************************************
    // section Problem
    // ******************************************************

    /*! @brief Constructor.
     *
     * Internal set up and initialization of the code should not be done here, but rather in initialize().
     */
    Problem();

    /*! @brief Destructor.
     *
     * The release of the resources allocated by the code should not be performed here, but rather in terminate().
     */
    virtual ~Problem();

    /*! @brief (Optional) Provide the relative path of a data file to be used by the code.
     *
     * This method must be called before initialize().
     *
     * @param[in] datafile relative path to the data file.
     * @throws ICoCo::WrongContext exception if called multiple times or after initialize().
     * @throws ICoCo::WrongArgument if an invalid path is provided.
     */
    virtual void setDataFile(const std::string& datafile);

    /*! @brief (Optional) Provide the MPI communicator to be used by the code for parallel computations.
     *
     * This method must be called before initialize(). The communicator should include all the processes
     * to be used by the code. For a sequential code, the call to setMPIComm is optional or mpicomm should be
     * nullptr.
     *
     * @param[in] mpicomm pointer to a MPI communicator. Type void* to avoid to include mpi.h for sequential codes.
     * @throws ICoCo::WrongContext exception if called multiple times or after initialize().
     */
    virtual void setMPIComm(void* mpicomm);

    /*! @brief (Mandatory) Initialize the current problem instance.
     *
     * In this method the code should allocate all its internal structures and be ready to execute. File reads, memory
     * allocations, and other operations likely to fail should be performed here, and not in the constructor (and not in
     * the setDataFile() or in the setMPIComm() methods either).
     * This method must be called only once (after a potential call to setMPIComm() and/or setDataFile()) and cannot be
     * called again before terminate() has been performed.
     *
     * @return true if all OK, otherwise false.
     * @throws ICoCo::WrongContext exception if called multiple times or after initialize().
     */
    virtual bool initialize();

    /*! @brief (Mandatory) Terminate the current problem instance and release all allocated resources.
     *
     * Terminate the computation, free the memory and save whatever needs to be saved. This method is called once
     * at the end of the computation or after a non-recoverable error.
     * No other ICoCo method except setDataFile(), setMPIComm() and initialize() may be called after this.
     *
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongContext exception if called inside the CALCULATION_DEFINED context (see Problem documentation).
     */
    virtual void terminate();

    /*! @brief (Optional) Return an error message related to the last failure return (False to initialize() or solveTimeStep()
     * for example).
     *
     * New in version 3 of ICoCo.
     *
     * Can be called any time between initialize() and terminate().
     *
     * @return message related to the last failure return.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::string getLastErrorMessage() const;

    // ******************************************************
    // section TimeStepManagement
    // ******************************************************

    /*! @brief (Mandatory) Return the current time of the simulation.
     *
     * Can be called any time between initialize() and terminate().
     * The current time can only change during a call to validateTimeStep() or to resetTime().
     *
     * @return the current (physical) time of the simulation
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual double presentTime() const;

    /*! @brief (Mandatory if initTimeStep() is implemented) Return the next preferred time step (time increment) for this code
     * (starting from presentTime()), and whether the code wants to stop.
     *
     * Both data are only indicative, the supervisor is not required to take them into account. This method is
     * however marked as mandatory, since most of the coupling schemes expect the code to provide this
     * information (those schemes then typically compute the minimum of the time steps of all the codes being coupled).
     * Hence a possible implementation is to return a huge value, if a precise figure can not be computed.
     *
     * Can be called whenever the code is outside the CALCULATION_DEFINED context (see Problem documentation).
     *
     * It can also be called inside TIME_STEP_DEFINED context. This is typically used to suggest a new time step for a second
     * attempt after a first failed resolution (the call is then made between a failed solveTimeStep() and abortTimeStep()).
     *
     * The method cannot be called inside STATIONARY_DEFINED context.
     *
     * @param[out] stop set to true if the code wants to stop. It can be used for example to indicate that, according to
     * a certain criterion, the end of the transient computation is reached from the code point of view.
     * @return the preferred time step for this code (only valid if stop is false).
     * @throws ICoCo::WrongContext exception if called inside the STATIONARY_DEFINED context (see Problem documentation).
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual double computeTimeStep(bool& stop) const;

    /*! @brief (Optional) Provide the next time step (time increment) to be used by the code.
     *
     * After this call (if successful), the computation time step is defined to ]t, t + dt] where t is the value
     * returned by presentTime().
     *
     * Can be called whenever the code is outside the CALCULATION_DEFINED context (see Problem documentation).
     * The code enters the CALCULATION_DEFINED (TIME_STEP_DEFINED) context.
     *
     * @param[in] dt the time step to be used by the code. Must be > 0.0.
     * @return false means that given time step is not compatible with the code time scheme.
     *
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongContext exception if called inside the CALCULATION_DEFINED context (see Problem documentation).
     * @throws ICoCo::WrongArgument exception if dt is invalid (dt <= 0.0).
     */
    virtual bool initTimeStep(double dt);

    /*! @brief (Mandatory if initTimeStep() is implemented) Perform the computation on the current time interval.
     *
     * Can be called (only once) whenever the code is inside the TIME_STEP_DEFINED context (see Problem documentation).
     *
     * @return true if computation was successful, false otherwise.
     * @throws ICoCo::WrongContext exception if called outside the TIME_STEP_DEFINED context (see Problem documentation).
     * @throws ICoCo::WrongContext exception if called several times without a call to validateTimeStep() or to
     * abortTimeStep().
     */
    virtual bool solveTimeStep();

    /*! @brief (Mandatory if initTimeStep() is implemented) Validate the computation performed by solveTimeStep() or
     * iterateTimeStep().
     *
     * Can be called when the code is inside the TIME_STEP_DEFINED context (see Problem documentation), if solveTimeStep() or
     * iterateTimeStep() have been called.
     *
     * After this call:
     * - the present time is advanced to the end of the computation time step
     * - the computation time step is undefined (the code leaves the TIME_STEP_DEFINED (and CALCULATION_DEFINED) context).
     *
     * @throws ICoCo::WrongContext exception if called outside the TIME_STEP_DEFINED context (see Problem documentation).
     * @throws ICoCo::WrongContext exception if called before solveTimeStep() or iterateTimeStep() methods.
     * @sa abortTimeStep()
     */
    virtual void validateTimeStep();

    /*! @brief (Mandatory if initTimeStep() is implemented) Set whether the next time step calculations will be performed to
     * obtain an stationary solution or if we are really interested in the transient.
     *
     * By default the code is assumed to be in stationary mode false (i.e. set up for a transient computation).
     * If set to true, solveTimeStep() (or iterateTimeStep()) methods solve a time step in view of an asymptotic solution.
     * In this mode, the code is allowed to produce a wrong transient in order to speed up the convergence to the steady-state.
     * This typically allows to reduce certain inertial terms.
     *
     * The stationary mode status of the code can only be modified by this method (or by a call to terminate()
     * followed by initialize()).
     *
     * Can be called whenever the code is outside the CALCULATION_DEFINED context (see Problem documentation).
     *
     * @param[in] stationaryMode true if the code should compute a stationary solution.
     *
     * @throws ICoCo::WrongContext exception if called inside the CALCULATION_DEFINED context (see Problem documentation).
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual void setStationaryMode(bool stationaryMode);

    /*! @brief (Mandatory if initTimeStep() is implemented) Indicate whether the code is in stationary mode or not.
     *
     * See also setStationaryMode().
     *
     * Can be called whenever. ### TODO: Je ne vois pas de raison de mettre un contexte au get ? ###
     *
     * @return true if the code has been set to compute a stationary solution.
     *
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual bool getStationaryMode() const;

    /*! @brief (Optional) Return whether the code has reached a stationary solution.
     *
     * Can be called whenever the code is outside the CALCULATION_DEFINED context (see Problem documentation).
     *
     * @return true if the code has reached a stationary solution.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongContext exception if called inside the CALCULATION_DEFINED context (see Problem documentation).
     */
    virtual bool isStationary() const;

    /*! @brief (Optional) Abort the computation on the current time step.
     *
     * Can be called whenever the code is inside the TIME_STEP_DEFINED context (see Problem documentation).
     * The code then leaves the TIME_STEP_DEFINED (and CALCULATION_DEFINED) context.
     *
     * After this call, the code must return to its state just before the previous initTimeStep() call.
     * Everything that has happened since that call must be forgotten. In particular, what was set to the code should be
     * forgotten, and outputs produced by the code must get back their previous values.
     *
     * This method is designed to get out a code from a corrupted state. Use iterateTimeStep() instead to repeat the calculation
     * of a time step.
     *
     * @throws ICoCo::WrongContext exception if called outside the CALCULATION_DEFINED context (see Problem documentation).
     * @sa validateTimeStep()
     */
    virtual void abortTimeStep();

    /*! @brief (Optional) Reset the current time of the Problem to a given value. ### TODO: passer en obligatoire ??? ###
     *
     * Particularly useful for the initialization of complex transients: the starting point of the transient
     * of interest is computed first, the time is reset to 0, and then the actual transient of interest starts with proper
     * initial conditions, and global time 0.
     *
     * Can be called whenever the code is outside the CALCULATION_DEFINED context (see Problem documentation).
     *
     * @param[in] time the new current time.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongContext exception if called inside the CALCULATION_DEFINED context (see Problem documentation)
     */
    virtual void resetTime(double time);

    /*! @brief (Mandatory if abortTimeStep() is implemented) Similar to solveTimeStep() but can be called several times.
     *
     * This method allows to repeat the computation of a given time step without calling abortTimeStep(). It is designed for
     * iterative coupling algorithms.
     *
     * The method may behave a little differently from solveTimeStep(). The maximum number of iterations of the internal solving
     * method can typically be different. However, calling iterateTimeStep() until converged is true must be equivalent to
     * calling solveTimeStep(), within the code convergence threshold.
     *
     * Can be called (potentially several times) inside the TIME_STEP_DEFINED context (see Problem documentation).
     *
     * @param[out] converged set to true if the solution is converged.
     * @return false if the computation failed.
     * @throws ICoCo::WrongContext exception if called outside the TIME_STEP_DEFINED context (see Problem documentation)
     * @sa solveTimeStep()
     */
    virtual bool iterateTimeStep(bool& converged);

    // ******************************************************
    // section StationaryManagement
    // ******************************************************

    /*! @brief (Optional) Get the code ready to compute a stationary solution.
     *
     * New in version 3 of ICoCo.
     *
     * The code enters the CALCULATION_DEFINED (STATIONARY_DEFINED) context.
     *
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongContext exception if called inside the CALCULATION_DEFINED context (see Problem documentation).
     */
    virtual void initStationary();

    /*! @brief (Mandatory if initStationary() is implemented) Perform the computation of a stationary solution.
     *
     * New in version 3 of ICoCo.
     *
     * Can be called (only once) whenever the code is inside the STATIONARY_DEFINED context (see Problem documentation).
     *
     * @return true if computation was successful, false otherwise.
     * @throws ICoCo::WrongContext exception if called outside the STATIONARY_DEFINED context (see Problem documentation).
     * @throws ICoCo::WrongContext exception if called several times without a call to validateStationary() or to
     * abortStationary().
     */
    virtual bool solveStationary();

    /*! @brief (Mandatory if initStationary() is implemented) Validate the computation performed by solveStationary() or
     * iterateStationary().
     *
     * New in version 3 of ICoCo.
     *
     * Can be called when the code is inside the STATIONARY_DEFINED context (see Problem documentation), if solveStationary() or
     * iterateStationary() have been called.
     *
     * After this call the code leaves the STATIONARY_DEFINED (and CALCULATION_DEFINED) context.
     *
     * @throws ICoCo::WrongContext exception if called outside the STATIONARY_DEFINED context (see Problem documentation).
     * @throws ICoCo::WrongContext exception if called before solveStationary() or iterateStationary() methods.
     * @sa abortStationary()
     */
    virtual void validateStationary();

    /*! @brief (Optional) Abort the ongoing computation on stationary solution.
     *
     * New in version 3 of ICoCo.
     *
     * Can be called whenever the code is inside the STATIONARY_DEFINED context (see Problem documentation).
     * The code then leaves the STATIONARY_DEFINED (and CALCULATION_DEFINED) context.
     *
     * After this call, the code must return to its state just before the previous initStationary() call.
     * Everything that has happened since that call must be forgotten. In particular, what was set to the code should be
     * forgotten, and outputs produced by the code must get back their previous values.
     *
     * This method is designed to get out a code from a corrupted state. Use iterateStationary() instead to repeat the
     * calculation of a stationary.
     *
     * @throws ICoCo::WrongContext exception if called outside the STATIONARY_DEFINED context (see Problem documentation).
     * @sa validateStationary()
     */
    virtual void abortStationary();

    /*! @brief (Mandatory if abortStationary() is implemented) Similar to solveStationary() but can be called several times.
     *
     * New in version 3 of ICoCo.
     *
     * This method allows to repeat the computation of a stationary without calling abortStationary(). It is designed for
     * iterative coupling algorithms.
     *
     * The method may behave a little differently from solveStationary(). The maximum number of iterations of the internal
     * solving method can typically be different. However, calling iterateStationary() until converged is true must be
     * equivalent to calling solveStationary(), within the code convergence threshold.
     *
     * Can be called (potentially several times) inside the STATIONARY_DEFINED context (see Problem documentation).
     *
     * @param[out] converged set to true if the solution is converged.
     * @return false if the computation failed.
     * @throws ICoCo::WrongContext exception if called outside the STATIONARY_DEFINED context (see Problem documentation).
     * @sa solveStationary()
     */
    virtual bool iterateStationary(bool& converged);

    // ******************************************************
    // section Restorable
    // ******************************************************

    /*! @brief (Optional) Save the state of the code.
     *
     * The saved state is identified by the combination of label and method arguments, while the content argument allows to
     * select what should be saved. If an empty string is provided (default value) to content argument, a "full" saving is done.
     *
     * If save() has already been called with the same label and method arguments but with different content argument, there are
     * two possible behavors (check the code documentation): (i) the previous saving may be either completely deleted, or (ii)
     * just the common contents overwritten.
     *
     * This method is const indicating that saving the state of the code should not change its behaviour with respect to
     * all other ICoCo methods. Implementation may rely on a mutable attribute (e.g. if saving to memory is desired).
     * ### TODO: Vous voulez vraiment faire ca avec uniquement des mutable en C++ ? Ca me semble tres contraignant. ###
     *
     * @param[in] label a user- (or code-) defined value identifying the state.
     * @param[in] method a string specifying which method is used to save the state of the code. A code can provide
     * different methods (for example in memory, on disk, etc.).
     * @param[in] content a string specifying what should be saved. By default (empty string), the saving is complete.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongContext exception if called inside the CALCULATION_DEFINED context (see Problem documentation).
     * @throws ICoCo::WrongArgument exception if the method or label argument is invalid.
     */
    virtual void save(int label, const std::string& method, const std::string& content = std::string()) const;

    /*! @brief (Mandatory if save() is implemented) Restore the state of the code.
     *
     * The state to be restored is identified by the combination of label and method arguments.
     * The save() method must have been called at some point or in some previous run with this combination.
     * The content argument provided to the restore method should refer to a sub-part (possibly all) of the saved content.
     *
     * @param[in] label a user- (or code-) defined value identifying the state.
     * @param[in] method a string specifying which method is used to restore the state of the code. A code can provide
     * different methods (for example in memory, on disk, etc.).
     * @param[in] content a string specifying what should be restored. By default (empty string), a complete restore is required.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongContext exception if called inside the CALCULATION_DEFINED context (see Problem documentation).
     * @throws ICoCo::WrongArgument exception if the method or label argument is invalid.
     * @sa save()
     */
    virtual void restore(int label, const std::string& method, const std::string& content = std::string());

    /*! @brief (Optional) Discard a previously saved state of the code.
     *
     * After this call, the save-point cannot be restored anymore. This method can be used to free the space occupied by
     * unused saved states.
     * This method is const indicating that forgeting a previous state of the code should not change its behaviour with
     * respect to all other ICoCo methods. Implementation may rely on a mutable attribute (e.g. if saving to memory is
     * desired).
     * ### TODO: Vous voulez vraiment faire ca avec uniquement des mutable en C++ ? Ca me semble tres contraignant. ###
     *
     * @param[in] label a user- (or code-) defined value identifying the state.
     * @param[in] method a string specifying which method is used to restore the state of the code. A code can provide
     * different methods (for example in memory, on disk, etc.).
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongArgument exception if the method or label argument is invalid.
     */
    virtual void forget(int label, const std::string& method) const;

    // ******************************************************
    // section Field insight.
    // ******************************************************

    /*! @brief (Optional) Get the list of input fields accepted by the code.
     *
     * @return the list of field names that represent inputs of the code
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::vector<std::string> getInputFieldsNames() const;

    /*! @brief (Optional) Get the list of output fields that can be provided by the code.
     *
     * @return the list of field names that can be produced by the code
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::vector<std::string> getOutputFieldsNames() const;

    /*! @brief (Optional) Get the type of a field managed by the code.
     *
     * The three possible types are int, double and string, as defined in the ValueType enum.
     *
     * @param[in] name field name
     * @return one of ValueType::Double, ValueType::Int or ValueType::String
     * @throws ICoCo::WrongArgument exception if the field name is invalid.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @sa ValueType
     */
    virtual ValueType getFieldType(const std::string& name) const;

    /*! @brief (Optional) Get the (length) unit used to define the meshes supporting the fields.
     *
     * @return length unit in which the mesh coordinates should be understood (e.g. "m", "cm", ...)
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::string getMeshUnit() const;

    /*! @brief (Optional) Get the physical unit used for a given field.
     *
     * @param[in] name field name
     * @return unit in which the field values should be understood (e.g. "W", "J", "Pa", ...)
     * @throws ICoCo::WrongArgument exception if the field name is invalid.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::string getFieldUnit(const std::string& name) const;

    /*! @brief (Optional) Get information about time semantics of a given field.
     *
     * New in version 3 of ICoCo.
     *
     * The method is designed to help build coherent time schemes.
     * For an output it can for example says that the data is provided at the beginning, end or middle of time step, or that
     * a time average is calculated.
     * For an input it can says that the provided data is taken as constant over the time scheme (order 0), or that it is seen
     * as a first (or higher) order in time function, and that the end of time step point is required.
     *
     * @param[in] name field name
     * @return explanation about time semantics of the field.
     * @throws ICoCo::WrongArgument exception if the field name is invalid.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::string getFieldTimeSemantics(const std::string& name) const;

    /*! @brief (Optional) Get the name of the mesh support of a given field.
     *
     * New in version 3 of ICoCo.
     *
     * The method allows to determine whether two fields use the same mesh.
     *
     * @param[in] name field name
     * @return name of the underlying mesh.
     * @throws ICoCo::WrongArgument exception if the field name is invalid.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::string getNameOfFieldUnderlyingMesh(const std::string& name) const;

    // ******************************************************
    // section MED fields I/O
    // ******************************************************

    /*! @brief (Optional) Retrieve an empty shell for an input field. This shell can be filled by the caller and then be
     * given to the code via setInputField(). The field has the MEDDoubleField format.
     *
     * The code uses this method to populate 'afield' with all the data that represents the context
     * of the field (i.e. its support mesh, its discretization -- on nodes, on elements, ...).
     * The remaining job for the caller of this method is to fill the actual values of the field itself.
     * When this is done the field can be sent back to the code through the method setInputMEDDoubleField().
     * This method is not mandatory but is useful to know the mesh, discretization... on which an input field is
     * expected.
     *
     * @param[in] name name of the field for which we would like the empty shell
     * @param[out] afield field object (in MEDDoubleField format) that will be populated with all the contextual information.   ### TODO: pourquoi 'afield' et pas 'field' ? C'est moche afield, non ? ###
     * Any previous information in this object will be discarded.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongArgument exception if the field name is invalid.
     */
    virtual void getInputMEDDoubleFieldTemplate(const std::string& name, MEDDoubleField& afield) const;

    /*! @brief (Optional) Provide the code with input data in the form of a MEDDoubleField.
     *
     * The method getInputMEDDoubleFieldTemplate(), if implemented, may be used first to prepare an empty shell of the field to
     * set to the code.
     *
     * @param[in] name name of the field that is given to the code.
     * @param[in] afield field object (in MEDDoubleField format) containing the input data to be read by the code.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongArgument exception if the field name ('name' parameter) is invalid.
     *  ### TODO: je supprime ces histoires de verification du temps a l'interieur du champ. Je n'ai jamais vu ca utilise et ca ne me semble pas forcement souhaitable (ca interdirait d'initier un calcul par le resultat du pas de temps precedent par exemple, non ?)
     */
    virtual void setInputMEDDoubleField(const std::string& name, const MEDDoubleField& afield);

    /*! @brief (Optional) Retrieve output data from the code in the form of a MEDDoubleField.
     *
     * Gets the output field corresponding to name from the code into the afield argument.
     *
     * @param[in] name name of the field that the caller requests from the code.
     * @param[out] afield field object (in MEDDoubleField format) populated with the data read by the code.
     * If the provided field is adequate, data may be written in place.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongArgument exception if the field name ('name' parameter) is invalid.
     */
    virtual void getOutputMEDDoubleField(const std::string& name, MEDDoubleField& afield) const;

    /*! @brief Similar to getInputMEDDoubleFieldTemplate() but for MEDIntField.
     * @sa getInputMEDDoubleFieldTemplate()
     */
    virtual void getInputMEDIntFieldTemplate(const std::string& name, MEDIntField& afield) const;

    /*! @brief Similar to setInputMEDDoubleField() but for MEDIntField.
     * @sa setInputMEDDoubleField()
     */
    virtual void setInputMEDIntField(const std::string& name, const MEDIntField& afield);

    /*! @brief Similar to getOutputMEDDoubleField() but for MEDIntField.
     * @sa getOutputMEDDoubleField()
     */
    virtual void getOutputMEDIntField(const std::string& name, MEDIntField& afield) const;

    /*! @brief Similar to getInputMEDDoubleFieldTemplate() but for MEDStringField.
     *
     * @b WARNING: at the time of writing, MEDStringField are not yet implemented anywhere.
     * @sa getInputMEDDoubleFieldTemplate()
     */
    virtual void getInputMEDStringFieldTemplate(const std::string& name, MEDStringField& afield) const;

    /*! @brief Similar to setInputMEDDoubleField() but for MEDStringField.
     *
     * @b WARNING: at the time of writing, MEDStringField are not yet implemented anywhere.
     * @sa setInputMEDDoubleField()
     */
    virtual void setInputMEDStringField(const std::string& name, const MEDStringField& afield);

    /*! @brief Similar to getOutputMEDDoubleField() but for MEDStringField.
     *
     * @b WARNING: at the time of writing, MEDStringField are not yet implemented anywhere.
     * @sa getOutputMEDDoubleField()
     */
    virtual void getOutputMEDStringField(const std::string& name, MEDStringField& afield) const;

    /*! @brief (Optional) Get MEDCoupling major version, if the code was built with MEDCoupling support.
     *
     * This can be used to assess compatibility between codes when coupling them.
     *
     * @return the MEDCoupling major version number (typically 7, 8, 9, ...)
     */
    virtual int getMEDCouplingMajorVersion() const;

    /*! @brief (Optional) Indicate whether the code was built with a 64-bits version of MEDCoupling.
     *
     * Implemented if the code was built with MEDCoupling support.
     * This can be used to assess compatibility between codes when coupling them.
     *
     * @return the MEDCoupling major version number
     */
    virtual bool isMEDCoupling64Bits() const;

    // ******************************************************
    // section Array insight.
    // ******************************************************

    /*! @brief (Optional) Get the list of input arrays accepted by the code.
     *
     * New in version 3 of ICoCo.
     *
     * @return the list of array names that represent inputs of the code
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::vector<std::string> getInputArraysNames() const;

    /*! @brief (Optional) Get the list of output arrays that can be provided by the code.
     *
     * New in version 3 of ICoCo.
     *
     * @return the list of array names that can be produced by the code
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::vector<std::string> getOutputArraysNames() const;

    /*! @brief (Optional) Get the type of an array managed by the code.
     *
     * New in version 3 of ICoCo.
     *
     * The three possible types are int, double and string, as defined in the ValueType enum.
     *
     * @param[in] name array name
     * @return one of ValueType::Double, ValueType::Int or ValueType::String
     * @throws ICoCo::WrongArgument exception if the array name is invalid.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @sa ValueType
     */
    virtual ValueType getArrayType(const std::string& name) const;

    /*! @brief (Optional) Get the physical unit used for a given array.
     *
     * New in version 3 of ICoCo.
     *
     * @param[in] name array name
     * @return unit in which the array values should be understood (e.g. "W", "J", "Pa", ...)
     * @throws ICoCo::WrongArgument exception if the array name is invalid.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::string getArrayUnit(const std::string& name) const;

    /*! @brief (Optional) Get information about time semantics of a given array.
     *
     * New in version 3 of ICoCo.
     *
     * The method is designed to help build coherent time schemes.
     * For an output it can for example says that the data is provided at the beginning, end or middle of time step, or that
     * a time average is calculated.
     * For an input it can says that the provided data is taken as constant over the time scheme (order 0), or that it is seen
     * as a first (or higher) order in time function, and that the end of time step point is required.
     *
     * @param[in] name array name
     * @return explanation about time semantics of the array.
     * @throws ICoCo::WrongArgument exception if the array name is invalid.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::string getArrayTimeSemantics(const std::string& name) const;

    // ******************************************************
    // section MED arrays I/O
    // ******************************************************

    /*! @brief (Optional) Provide the code with input data in the form of a MEDDoubleArray.
     *
     * New in version 3 of ICoCo.
     *
     * @param[in] name name of the array that is given to the code.
     * @param[in] array array object (in MEDDoubleArray format) containing the input data to be read by the code.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongArgument exception if the array name ('name' parameter) is invalid.
     */
    virtual void setInputMEDDoubleArray(const std::string& name, const MEDDoubleArray& array);

    /*! @brief (Optional) Retrieve output data from the code in the form of a MEDDoubleArray.
     *
     * New in version 3 of ICoCo.
     *
     * Gets the output array corresponding to name from the code into the array argument.
     *
     * @param[in] name name of the array that the caller requests from the code.
     * @param[out] array array object (in MEDDoubleArray format) populated with the data read by the code.
     * If the provided array is adequate, data may be written in place.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongArgument exception if the array name ('name' parameter) is invalid.
     */
    virtual void getOutputMEDDoubleArray(const std::string& name, MEDDoubleArray& array) const;

    /*! @brief Similar to setInputMEDDoubleArray() but for MEDIntArray.
     *
     * New in version 3 of ICoCo.
     *
     * @sa setInputMEDDoubleArray()
     */
    virtual void setInputMEDIntArray(const std::string& name, const MEDIntArray& array);

    /*! @brief Similar to getOutputMEDDoubleArray() but for MEDIntArray.
     *
     * New in version 3 of ICoCo.
     *
     * @sa getOutputMEDDoubleArray()
     */
    virtual void getOutputMEDIntArray(const std::string& name, MEDIntArray& array) const;

    /*! @brief Similar to setInputMEDDoubleArray() but for MEDStringArray.
     *
     * New in version 3 of ICoCo.
     *
     * @b WARNING: at the time of writing, MEDStringArray are not yet implemented anywhere.
     * @sa setInputMEDDoubleArray()
     */
    virtual void setInputMEDStringArray(const std::string& name, const MEDStringArray& array);

    /*! @brief Similar to getOutputMEDDoubleArray() but for MEDStringArray.
     *
     * New in version 3 of ICoCo.
     *
     * @b WARNING: at the time of writing, MEDStringArray are not yet implemented anywhere.
     * @sa getOutputMEDDoubleArray()
     */
    virtual void getOutputMEDStringArray(const std::string& name, MEDStringArray& array) const;

    // ******************************************************
    // section AlgebraicData insight.
    // ******************************************************

    /*! @brief (Optional) Get the list of AlgebraicData available in the code.
     *
     * New in version 3 of ICoCo.
     *
     * @return the list of AlgebraicData names of the code
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::vector<std::string> getAlgebraicDataNames() const;

    /*! @brief (Optional) Get information about time semantics of a given AlgebraicData.
     *
     * New in version 3 of ICoCo.
     *
     * The method is designed to help build coherent time schemes.
     * For an output it can for example says that the data is provided at the beginning, end or middle of time step, or that
     * a time average is calculated.
     * For an input it can says that the provided data is taken as constant over the time scheme (order 0), or that it is seen
     * as a first (or higher) order in time function, and that the end of time step point is required.
     * CAUTION: AlgebraicData are always both input and output.
     *
     * @param[in] name AlgebraicData name
     * @return explanation about time semantics of the AlgebraicData.
     * @throws ICoCo::WrongArgument exception if the AlgebraicData name is invalid.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::string getAlgebraicDataTimeSemantics(const std::string& name) const;

    // ******************************************************
    // section AlgebraicData I/O
    // ******************************************************

    /*! @brief (Optional) Provide the code with input data in the form of a AlgebraicData.
     *
     * New in version 3 of ICoCo.
     *
     * @param[in] name name of the AlgebraicData that is given to the code.
     * @param[in] data AlgebraicData object to be read by the code.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongArgument exception if the AlgebraicData name ('name' parameter) is invalid.
     */
    virtual void setAlgebraicData(const std::string& name, const AlgebraicData& data);

    /*! @brief (Optional) Retrieve an AlgebraicData from the code.
     *
     * New in version 3 of ICoCo.
     *
     * @param[in] name name of the AlgebraicData that the caller requests from the code.
     * @param[out] data AlgebraicData object populated with the data read by the code.
     * * If the provided AlgebraicData is adequate, data may be written in place.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongArgument exception if the AlgebraicData name ('name' parameter) is invalid.
     */
    virtual void getAlgebraicData(const std::string& name, AlgebraicData& data) const;

    // ******************************************************
    // section Scalar values insight.
    // ******************************************************

    /*! @brief (Optional) Get the list of input scalars accepted by the code.
     *
     * @return the list of scalar names that represent inputs of the code
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::vector<std::string> getInputValuesNames() const;

    /*! @brief (Optional) Get the list of output scalars that can be provided by the code.
     *
     * @return the list of scalar names that can be returned by the code
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::vector<std::string> getOutputValuesNames() const;

    /*! @brief (Optional) Get the type of a scalar managed by the code (input or output)
     *
     * The three possible types are int, double and string, as defined in the ValueType enum.
     *
     * @param[in] name scalar value name
     * @return one of ValueType::Double, ValueType::Int or ValueType::String
     * @throws ICoCo::WrongArgument exception if the scalar name is invalid.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @sa ValueType
     */
    virtual ValueType getValueType(const std::string& name) const;

    /*! @brief (Optional) Get the physical unit used for a given value.
     *
     * @param[in] name scalar value name
     * @return unit in which the scalar value should be understood (e.g. "W", "J", "Pa", ...)
     * @throws ICoCo::WrongArgument exception if the value name is invalid.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::string getValueUnit(const std::string& name) const;

    /*! @brief (Optional) Get information about time semantics of a given value.
     *
     * New in version 3 of ICoCo.
     *
     * The method is designed to help build coherent time schemes.
     * For an output it can for example says that the data is provided at the beginning, end or middle of time step, or that
     * a time average is calculated.
     * For an input it can says that the provided data is taken as constant over the time scheme (order 0), or that it is seen
     * as a first (or higher) order in time function, and that the end of time step point is required.
     *
     * @param[in] name value name
     * @return explanation about time semantics of the value.
     * @throws ICoCo::WrongArgument exception if the value name is invalid.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::string getValueTimeSemantics(const std::string& name) const;

    // ******************************************************
    // section Scalar values I/O.
    // ******************************************************

    /*! @brief (Optional) Provide the code with a scalar double data.
     *
     * @param[in] name name of the scalar value that is given to the code.
     * @param[in] val value passed to the code.
     * @throws ICoCo::WrongArgument exception if the scalar name ('name' parameter) is invalid.
     */
    virtual void setInputDoubleValue(const std::string& name, const double& val);

    /*! @brief (Optional) Retrieve a scalar double value from the code.
     *
     * @param[in] name name of the scalar value to be read from the code.
     * @return the double value read from the code.
     * @throws ICoCo::WrongArgument exception if the scalar name ('name' parameter) is invalid.
     */
    virtual double getOutputDoubleValue(const std::string& name) const;

    /*! @brief (Optional) Similar to setInputDoubleValue() but for an int value.
     * @sa setInputDoubleValue()
     */
    virtual void setInputIntValue(const std::string& name, const int& val);

    /*! @brief (Optional) Similar to getOutputDoubleValue() but for an int value.
     * @sa getOutputDoubleValue()
     */
    virtual int getOutputIntValue(const std::string& name) const;

    /*! @brief (Optional) Similar to setInputDoubleValue() but for an string value.
     * @sa setInputDoubleValue()
     */
    virtual void setInputStringValue(const std::string& name, const std::string& val);

    /*! @brief (Optional) Similar to getOutputDoubleValue() but for an string value.
     * @sa getOutputDoubleValue()
     */
    virtual std::string getOutputStringValue(const std::string& name) const;
  };

}

/*! @brief (Mandatory) Retrieve an instance of the class defined by the code (and inheriting Problem).
 *
 * The main purpose of this function is to facilitate the instantiation of a problem when the code is loaded
 * in the supervisor using a dlopen() mechanism. Code implementing the ICoCo interface should implement this
 * method by returning a new instance of their derived class of Problem.
 * No default implementation can be provided for this.
 *
 * @return a new instance of the derived class of Problem implemented by the code.
 */
extern "C" ICoCo::Problem* getProblem();

#endif
