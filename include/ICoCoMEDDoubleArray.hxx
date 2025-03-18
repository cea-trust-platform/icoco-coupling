// ICoCo file common to several codes
// Version 2 -- 02/2021
//
// WARNING: this file is part of the official ICoCo API and should not be modified.
// The official version can be found at the following URL:
//
//    https://github.com/cea-trust-platform/icoco-coupling

#ifndef ICoCoMEDDoubleArray_included
#define ICoCoMEDDoubleArray_included

#include <ICoCo_DeclSpec.hxx>

namespace MEDCoupling
{
  class DataArrayDouble;
}

namespace ICoCo
{
  /*! @brief Array of values stored internally as a MEDCoupling object.
   *
   * This class is a wrapper around a MEDCoupling::DataArrayDouble object, which holds the data array.
   * From version 2 of ICoCo, MEDCoupling objects are not anymore exposed directly into the API
   * of ICoCo::Problem. The rationale is to make the interface ICoCo::Problem free of external dependencies
   * (MEDCoupling particularly).
   *
   * @sa the MEDCoupling documentation, notably the reference counter mechanism used to manage the lifecycle of
   * MEDCoupling objects.
   */
  class ICOCO_EXPORT MEDDoubleArray
  {
  public:
    /*! Builds an empty array (internal MEDCoupling object not set).
     */
    MEDDoubleArray();

    /*! @brief Builds a array and assign its internal DataArrayDouble array.
     *
     * @param array MEDCoupling array instance to use. The array reference counter is incremented.
     */
    MEDDoubleArray(MEDCoupling::DataArrayDouble* array);

    /*! @brief Copy construcotr.
     */
    MEDDoubleArray(const MEDDoubleArray& array);

    /*! @brief Assignement operator.
     * @param array another MEDDoubleArray instance. The previous internal MEDCoupling array reference (if any) has
     * its counter decremented.
     */
    MEDDoubleArray& operator=(const MEDDoubleArray& array);

    /*! @brief Destructor.
     */
    virtual ~MEDDoubleArray();

    /*! @brief Get the internal MEDCoupling array object.
     * @return a pointer to the DataArrayDouble object detained by this instance. Note that the corresponding
     * object should not be deleted, or its reference counter should not be decreased! Doing so will result in an
     * invalid instance of the current object.
     */
    MEDCoupling::DataArrayDouble *getMCArray() const;

    /*! @brief Set the internal MEDCoupling array object.
     *
     * Any previously set array is discarded (its reference counter is decreased) and the reference counter of the
     * array being set is increased.
     *
     * @param array DataArrayDouble object to be used.
     */
    void setMCArray(MEDCoupling::DataArrayDouble * array);

  private:
    MEDCoupling::DataArrayDouble *_array;
  };
} // namespace ICoCo

#endif
