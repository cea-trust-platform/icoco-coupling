// ICoCo file common to several codes
// Version 3 -- 03/2025
//
// WARNING: this file is part of the official ICoCo API and should not be modified.
// The official version can be found at the following URL:
//
//    https://github.com/cea-trust-platform/icoco-coupling

#ifndef ICoCoAlgebraicData_included
#define ICoCoAlgebraicData_included

namespace ICoCo
{
  /*! @brief This abstract class can be used to handle any data structure that implements few operators, enabling it to be
   * presented as a vector in the mathematical sense.
   *
   * This kind of data is intended to be used in a convergence acceleration algorithm (for example), then returned to the code
   * that produced it. For this reason, it is not necessary to specify the data format precisely.
   */
  class AlgebraicData
  {
  public:
    /*! @brief Return a clone of this.
     *
     * @return a pointer on a clone of this. The data are cloned.
     */
    virtual AlgebraicData* clone() const = 0;

    /*! @brief Copy the content of another object.
     *
     * @throw std::exception if the actual type of data is not the same as that of this.    ### TODO: euh... je ne sais pas quoi mettre en type d'erreur. Les arguments de ICoCo::WrongArgument ne correspondent pas trop. ###
     * @param[in] data another AlgebraicData to be copied.
     * @return reference on this.
     */
    virtual AlgebraicData& copy(const AlgebraicData& data) = 0;

    /*! @brief Return the norm max (maximum value) of this.
     *
     * @return norm max of this.
     */
    virtual double normMax() const = 0;

    /*! @brief Return the norm 1 (sum of absolute values) of this.
     *
     * @return norm 1 of this.
     */
    virtual double norm1() const = 0;

    /*! @brief Return the norm 2 (square root of the sum of squared values) of this.
     *
     * @return norm 2 of this.
     */
    virtual double norm2() const = 0;

    /*! @brief Return this + data.
     *
     * @throw std::exception if the actual type of data is not the same as that of this.
     * @param[in] data another AlgebraicData.
     * @return a pointer on a new AlgebraicData.
     */
    virtual AlgebraicData* operator+(const AlgebraicData& data) const = 0;

    /*! @brief In place sum operator.
     *
     * @throw std::exception if the actual type of data is not the same as that of this.
     * @param[in] data another AlgebraicData.
     * @return reference on this.
     */
    virtual AlgebraicData& operator+=(const AlgebraicData& data) = 0;

    /*! @brief Return this - data.
     *
     * @throw std::exception if the actual type of data is not the same as that of this.
     * @param[in] data another AlgebraicData.
     * @return a pointer on a new AlgebraicData.
     */
    virtual AlgebraicData* operator-(const AlgebraicData& data) const = 0;

    /*! @brief In place difference operator.
     *
     * @throw std::exception if the actual type of data is not the same as that of this.
     * @param[in] data another AlgebraicData.
     * @return reference on this.
     */
    virtual AlgebraicData& operator-=(const AlgebraicData& data) = 0;

    /*! @brief Return scalar * this.
     *
     * @param[in] scalar double value to use for the multiplication.
     * @return a pointer on a new AlgebraicData.
     */
    virtual AlgebraicData* operator*(double scalar) const = 0;

    /*! @brief In place multiplication with scalar.
     *
     * @param[in] scalar double value to use for the multiplication.
     * @return reference on this.
     */
    virtual AlgebraicData& operator*=(const AlgebraicData& data) = 0;

    /*! @brief Return the scalar product of this with data.
     *
     * @throw std::exception if the actual type of data is not the same as that of this.
     * @param[in] data another AlgebraicData.
     * @return value of the scalar product.
     */
    virtual double dot(const AlgebraicData& data) const = 0;

  }; // class AlgebraicData
} // namespace ICoCo

#endif
