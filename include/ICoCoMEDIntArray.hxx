// ICoCo file common to several codes
// Version 2 -- 02/2021
//
// WARNING: this file is part of the official ICoCo API and should not be modified.
// The official version can be found at the following URL:
//
//    https://github.com/cea-trust-platform/icoco-coupling

#ifndef ICoCoMEDIntArray_included
#define ICoCoMEDIntArray_included

#include <ICoCo_DeclSpec.hxx>

namespace MEDCoupling
{
  class DataArrayInt32;
}

namespace ICoCo
{
  /*! @brief Array of values stored internally as a MEDCoupling object.
   *
   * This class is a wrapper around a MEDCoupling::DataArrayInt32 object, which holds the data array.
   * From version 2 of ICoCo, MEDCoupling objects are not anymore exposed directly into the API
   * of ICoCo::Problem. The rationale is to make the interface ICoCo::Problem free of external dependencies
   * (MEDCoupling particularly).
   *
   * @sa the MEDCoupling documentation, notably the reference counter mechanism used to manage the lifecycle of
   * MEDCoupling objects.
   */
  class ICOCO_EXPORT MEDIntArray
  {
  public:
    /*! Builds an empty array (internal MEDCoupling object not set).
     */
    MEDIntArray();

    /*! @brief Builds a array and assign its internal DataArrayInt32 array.
     *
     * @param array MEDCoupling array instance to use. The array reference counter is incremented.
     */
    MEDIntArray(MEDCoupling::DataArrayInt32* array);

    /*! @brief Copy construcotr.
     */
    MEDIntArray(const MEDIntArray& array);

    /*! @brief Assignement operator.
     * @param array another MEDIntArray instance. The previous internal MEDCoupling array reference (if any) has
     * its counter decremented.
     */
    MEDIntArray& operator=(const MEDIntArray& array);

    /*! @brief Destructor.
     */
    virtual ~MEDIntArray();

    /*! @brief Get the internal MEDCoupling array object.
     * @return a pointer to the DataArrayInt32 object detained by this instance. Note that the corresponding
     * object should not be deleted, or its reference counter should not be decreased! Doing so will result in an
     * invalid instance of the current object.
     */
    MEDCoupling::DataArrayInt32 *getMCArray() const;

    /*! @brief Set the internal MEDCoupling array object.
     *
     * Any previously set array is discarded (its reference counter is decreased) and the reference counter of the
     * array being set is increased.
     *
     * @param array DataArrayInt32 object to be used.
     */
    void setMCArray(MEDCoupling::DataArrayInt32 * array);

  private:
    MEDCoupling::DataArrayInt32 *_array;
  };
} // namespace ICoCo

#endif
