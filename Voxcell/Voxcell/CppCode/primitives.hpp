#ifndef _MAKENA_PRIMITIVES_HPP_
#define _MAKENA_PRIMITIVES_HPP_

#include <array>
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <exception>
#include <stdexcept>
#include <cmath>


#ifdef UNIT_TESTS
#include "gtest/gtest_prod.h"
#endif


/**
 * @file primitives.hpp
 *
 * @brief Implements Vectors of 2D and 3D, and 3x3 matrixand their basic
 *        operations. It also provides a 3x3symmetric Eigenvalue finder and 
 *        a principal component analyzer assuming the 3x3 matrixrepresents 
 *        covariance.
 *
 * @reference
 *
 */

namespace Makena {

using namespace std;

/** @brief tolerance for an element to be considered numerically equal
 *         to another to find degeneracy
 */
static constexpr double EPSILON_CUBED   = 0.00000001;// For det(M)
static constexpr double EPSILON_SQUARED = 0.00000001;// For squared distance
static constexpr double EPSILON_LINEAR  = 0.00000001;// For distance
static constexpr double EPSILON_ANGLE   = 0.00000001;// Length of cross product

#ifndef M_PI
static constexpr double M_PI             = 3.14159265358979323846;
#endif
static constexpr double PI2OVER3         = M_PI*2.0/3.0;

static constexpr double COSINE_75_DEGREE = 0.258819045102521;

using  GenerationT = unsigned long long;

class Mat3x3;

/** @class Vec3
 *
 *  @brief 3-dimentional vector intended to realtime geometric operations
 */
class Vec3 {

  public:

    inline Vec3();
    inline Vec3(double x, double y, double z);
    inline Vec3(std::array<double,3>& v);
    inline Vec3(double* v);
    inline virtual ~Vec3();
    inline Vec3&      operator += (const Vec3& rhs);
    inline Vec3&      operator -= (const Vec3& rhs);
    inline bool       operator == (const Vec3& rhs) const;
    inline bool       operator != (const Vec3& rhs) const;
    inline const Vec3 operator +  (const Vec3& rhs) const;
    inline const Vec3 operator -  (const Vec3& rhs) const;
    inline const Vec3 operator *  (double rhs) const;

    /** @remark index is 1-base to be alinged with standard math notation
     */
    inline double& operator[](size_t index);
    inline double  dot(const Vec3& rhs) const;
    inline Vec3    cross(const Vec3& rhs) const;

    /** @brief returns an arbitrary vector perpendicular to this
     */
    inline Vec3 perp() const;

    /** @brief returns a 3x3 matrix R such that Rv = r x v
     *         where v is an arbitrary 3-vector, r is this r-vector, and
     *         x is a cross operator.
     */
    inline Mat3x3 crossMat() const;

    /** @brief returns norm of the vector (norm2).
     */
    inline double norm2() const;

    /** @brief returns a squared norm.
     */
    inline double squaredNorm2() const;

    /** @brief multiple the elements with scalar s */
    inline void scale(double s);

    /** @brief set the values of this vector*/
    inline void set(double x, double y, double z);

    inline void set(double* v);
    inline void setX(double x);
    inline void setY(double y);
    inline void setZ(double z);

    inline void zero();

    inline double* get_array();

    inline void normalize();

    inline double x() const;
    inline double y() const;
    inline double z() const;

    /** @remark used to dump bit-by-bit precise value. */
    inline void decDump(std::ostream& os) const;

  private:

    array<double,3> mV;


#ifdef UNIT_TESTS
    friend class PrimitivesTests;
#endif

};


inline std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    os << "(" << v.x() << ", " << v.y() << ", " << v.z() << ")";
    return os;
}



/** @class Vec2
 *
 *  @brief 2-dimentional vector intended to realtime geometric operations
 */
class Vec2 {

  public:

    inline Vec2();
    inline Vec2(double x, double y);
    inline Vec2(std::array<double,2>& v);
    inline Vec2(double* v);
    inline virtual ~Vec2();
    inline Vec2& operator += (const Vec2& rhs);
    inline Vec2& operator -= (const Vec2& rhs);
    inline bool operator == (const Vec2& rhs) const;
    inline bool operator != (const Vec2& rhs) const;
    inline const Vec2 operator + (const Vec2& rhs) const;
    inline const Vec2 operator - (const Vec2& rhs) const;
    inline const Vec2 operator * (double rhs) const;

    /** @remark index is 1-base to be alinged with standard math notation
     */
    inline double& operator[](size_t index);
    inline double dot(const Vec2& rhs) const;
    inline Vec3 cross(const Vec2& rhs) const;

    /** @brief returns the perpendicular vector.
     */
    inline Vec2 perp() const;

    /** @brief returns norm of the vector (norm2).
     */
    inline double norm2() const;

    /** @brief returns a squared norm.
     */
    inline double squaredNorm2() const;

    /** @brief multiple the elements with scalar s */
    inline void scale(double s);

    /** @brief set the values of this vector*/
    inline void set(double x, double y, double z);

    inline void set(double* v);
    inline void setX(double x);
    inline void setY(double y);

    inline void zero();

    inline void normalize();

    inline double x() const;
    inline double y() const;

    inline double* get_array();

    inline void decDump(std::ostream& os) const;

  private:

    array<double,2> mV;


#ifdef UNIT_TESTS
    friend class PrimitivesTests;
#endif

};


inline std::ostream& operator<<(std::ostream& os, const Vec2& v) {
    os << "(" << v.x() << ", " << v.y() << ")";
    return os;
}



/** @class Mat3x3
 *
 *  @brief 3x3 matrix intended to realtime geometric operations
 */
class Mat3x3 {

public:
    inline Mat3x3();

    inline Mat3x3(double *v);

    inline Mat3x3(double v1, double v2, double v3,
                  double v4, double v5, double v6,
                  double v7, double v8, double v9 );

    inline Mat3x3(const Vec3& v1, const Vec3& v2, const Vec3& v3);

    inline virtual ~Mat3x3();

    inline Mat3x3& operator += (const Mat3x3& rhs);

    inline Mat3x3& operator -= (const Mat3x3& rhs);

    inline bool operator == (const Mat3x3& rhs) const;

    inline bool operator != (const Mat3x3& rhs) const;

    inline Mat3x3 operator + (const Mat3x3& rhs) const;

    inline Mat3x3 operator - (const Mat3x3& rhs) const;

    inline const Mat3x3 transpose() const;

    inline const void transposeInPlace();

    inline Mat3x3 operator * (const Mat3x3& rhs) const;

    inline Mat3x3 pow2() const;

    inline Vec3 operator * (const Vec3& rhs) const;

    inline double det() const;

    inline const Mat3x3 inverse() const;

    inline void scale(double s);

    inline double& cell(size_t i, size_t j);

    inline double val(size_t i, size_t j) const;

    inline double* get_array();

    inline double trace() const;

    inline Vec3 col(size_t i) const;

    inline Vec3 row(size_t i) const;

    inline void zero();

    /** @brief returns the eigen values and the eigen vectors
     *         assuming the matrix is symmetric.
     *
     *  @param eValues  (out) : eigen values.  The values are arranged in the
     *                          descending order as in x() >= y() >= z().
     *
     *  @return eigen vectors in each column in the order corresponding to
     *          the eigen values in eValues.
     *
     *  @reference : [DE14] https://www.geometrictools.com/
     *                               Documentation/RobustEigenSymmetric3x3.pdf
     *               [W13] https://en.wikipedia.org/wiki/Eigenvalue_algorithm
     */
    Mat3x3 EigenVectorsIfSymmetric(Vec3& eValues);


protected:

    void EigenValuesIfSymmetric(double& a1, double& a2, double& a3);

    Vec3 findEigenVectorByCrossProducts(double alpha);

    Vec3 findEigenVectorInSubSpace(Vec3& Ev1, double lambda2);

    array<double,9>  mV;


#ifdef UNIT_TESTS
    friend class PrimitivesTests;
#endif

};


/** @brief performs principal component analysis on a list of points and
 *         finds the principal component axes and the variance along them.
 *
 *  @param points (in): list of points
 *
 *  @param spread (out): the variance along the principal component axes
 *                       in the descending order.
 *
 *  @param mean   (out): the mean
 *
 *  @return a matrix whose 3 columns specify the principal component axes.
 *          They are normalized vectors.
 */
Mat3x3 findPrincipalComponents(vector<Vec3>& points, Vec3& spread, Vec3& mean);




/** @brief performs principal component analysis on a list of points and
 *         finds the principal component axes and the variance along them.
 *
 *  @param points (in): list of points
 *
 *  @param spread (out): variance in the direction of principal axes
 *
 *  @param mean   (out): mean 
 *
 *  @param axis1  (out): primary principal axis
 *
 *  @param axis2  (out): secondary principal axis
 *
 */
void findPrincipalComponents(
    vector<Vec2>& points,
    Vec2&         spread,
    Vec2&         mean,
    Vec2&         axis1,
    Vec2&         axis2
);



/** @brief performs eigen value decomposition on 2x2 matrix
 *
 *  @param m11 , m12 , m21 , m22 (in): matrix elements.
 *
 *  @param values (out): two eigen values.
 *
 *  @param v1    (out): eigen vector 1
 *
 *  @param v2    (out): eigen vector 2
 */
void findEigenVectors(
   double m11,
   double m12,
   double m21,
   double m22,
   Vec2&  values,
   Vec2&  v1,
   Vec2&  v2
);



inline Vec3::Vec3():mV{{0.0, 0.0, 0.0}}{;}
inline Vec3::Vec3(double x, double y, double z):mV{{x, y, z}}{;}

inline Vec3::Vec3(std::array<double,3>& v) {
                               memcpy(mV.data(), v.data(), sizeof(double)*3); }

inline Vec3::Vec3(double* v) { memcpy(mV.data(), v, sizeof(double)*3); }

inline Vec3::~Vec3(){;}

inline Vec3& Vec3::operator += (const Vec3& rhs){
    mV[0] += (rhs.mV[0]);
    mV[1] += (rhs.mV[1]);
    mV[2] += (rhs.mV[2]);
    return *this;
}

inline Vec3& Vec3::operator -= (const Vec3& rhs){
    mV[0] -= (rhs.mV[0]);
    mV[1] -= (rhs.mV[1]);
    mV[2] -= (rhs.mV[2]);
    return *this;
}

inline bool Vec3::operator == (const Vec3& rhs) const {
    return (fabs(mV[0] - rhs.mV[0]) < EPSILON_LINEAR) &&
           (fabs(mV[1] - rhs.mV[1]) < EPSILON_LINEAR) &&
           (fabs(mV[2] - rhs.mV[2]) < EPSILON_LINEAR);
}

inline bool Vec3::operator != (const Vec3& rhs) const {return !(*this == rhs);}

inline const Vec3 Vec3::operator + (const Vec3& rhs) const {
                                                   return Vec3(*this) += rhs; }

inline const Vec3 Vec3::operator - (const Vec3& rhs) const {
                                                   return Vec3(*this) -= rhs; }

inline const Vec3 Vec3::operator * (double rhs) const {
                                       Vec3 V(*this); V.scale(rhs); return V; }

/** @remark index is 1-base to be alinged with standard math notation
 */
inline double& Vec3::operator[](size_t index){
    return mV[index-1];
}

inline double Vec3::dot(const Vec3& rhs) const {
                   return mV[0]*rhs.mV[0] + mV[1]*rhs.mV[1] + mV[2]*rhs.mV[2];}

inline Vec3 Vec3::cross(const Vec3& rhs) const {
    Vec3 r;
    r.mV[0] = mV[1]*rhs.mV[2] - mV[2]*rhs.mV[1];
    r.mV[1] = mV[2]*rhs.mV[0] - mV[0]*rhs.mV[2];
    r.mV[2] = mV[0]*rhs.mV[1] - mV[1]*rhs.mV[0];
    return r;
}

/** @brief returns an arbitrary vector perpendicular to this
 */
inline Vec3 Vec3::perp() const {
    if (mV[0] > mV[1]) {
        if (mV[1] > mV[2]) {
            Vec3 r(0.0, 0.0, 1.0);
            return cross(r);
        }
        else {
            Vec3 r(0.0, 1.0, 0.0);
            return cross(r);
        }
    }
    else {
        if (mV[0] > mV[2]) {
            Vec3 r(0.0, 0.0, 1.0);
            return cross(r);
        }
        else {
            Vec3 r(1.0, 0.0, 0.0);
            return cross(r);
        }
    }
}

/** @brief returns a 3x3 matrix R such that Rv = r x v
 *         where v is an arbitrary 3-vector, r is this r-vector, and
 *         x is a cross operator.
 */
inline Mat3x3 Vec3::crossMat() const {
    Mat3x3 M(       0.0, -1.0*mV[2],      mV[1],
                  mV[2],        0.0, -1.0*mV[0],
             -1.0*mV[1],      mV[0],        0.0  );
    return M;
}


/** @brief returns norm of the vector (norm2).
 */
inline double Vec3::norm2() const { return sqrt(squaredNorm2()); }

/** @brief returns a squared norm.
 */
inline double Vec3::squaredNorm2() const {
                              return mV[0]*mV[0] + mV[1]*mV[1] + mV[2]*mV[2]; }

/** @brief multiple the elements with scalar s */
inline void Vec3::scale(double s) {
                      mV[0] = s * mV[0]; mV[1] = s * mV[1]; mV[2] = s* mV[2]; }

/** @brief set the values of this vector*/
inline void Vec3::set(double x, double y, double z) {
                                              mV[0] = x; mV[1] = y; mV[2] = z;}

inline void Vec3::set(double* v) { memcpy(mV.data(), v, sizeof(double)*3); }

inline void Vec3::setX(double x) { mV[0] = x; }
inline void Vec3::setY(double y) { mV[1] = y; }
inline void Vec3::setZ(double z) { mV[2] = z; }

inline void Vec3::zero() { memset(mV.data(), 0, sizeof(double)*3); }

inline void Vec3::normalize()
{ 
    double s = norm2(); 
    if (s >= EPSILON_SQUARED) {
        scale(1.0/s);
    }
}

inline double Vec3::x() const { return mV[0]; }
inline double Vec3::y() const { return mV[1]; }
inline double Vec3::z() const { return mV[2]; }

inline double* Vec3::get_array() { return mV.data(); }

inline void Vec3::decDump(std::ostream& os) const
{
    double dx = x();
    double dy = y();
    double dz = z();
    unsigned char* p;
    p = (unsigned char*)&dx;
    for (long i = 0; i < 8; i++) {
        os << int(*p) << " ";
        p++;
    }
    p = (unsigned char*)&dy;
    for (long i = 0; i < 8; i++) {
        os << int(*p) << " ";
        p++;
    }
    p = (unsigned char*)&dz;
    for (long i = 0; i < 8; i++) {
        os << int(*p) << " ";
        p++;
    }
}

inline Vec2::Vec2():mV{{0.0, 0.0}}{;}
inline Vec2::Vec2(double x, double y):mV{{x, y}}{;}

inline Vec2::Vec2(std::array<double,2>& v) {
                               memcpy(mV.data(), v.data(), sizeof(double)*2); }

inline Vec2::Vec2(double* v) { memcpy(mV.data(), v, sizeof(double)*2); }

inline Vec2::~Vec2(){;}

inline Vec2& Vec2::operator += (const Vec2& rhs){
    mV[0] += (rhs.mV[0]);
    mV[1] += (rhs.mV[1]);
    return *this;
}

inline Vec2& Vec2::operator -= (const Vec2& rhs){
    mV[0] -= (rhs.mV[0]);
    mV[1] -= (rhs.mV[1]);
    return *this;
}

inline bool Vec2::operator == (const Vec2& rhs) const {
    return (fabs(mV[0] - rhs.mV[0]) < EPSILON_LINEAR) &&
           (fabs(mV[1] - rhs.mV[1]) < EPSILON_LINEAR) ;
}

inline bool Vec2::operator != (const Vec2& rhs) const {return !(*this == rhs);}

inline const Vec2 Vec2::operator + (const Vec2& rhs) const {
                                                   return Vec2(*this) += rhs; }

inline const Vec2 Vec2::operator - (const Vec2& rhs) const {
                                                   return Vec2(*this) -= rhs; }

inline const Vec2 Vec2::operator * (double rhs) const {
                                       Vec2 V(*this); V.scale(rhs); return V; }

/** @remark index is 1-base to be alinged with standard math notation
 */
inline double& Vec2::operator[](size_t index){
    return mV[index-1];
}

inline double Vec2::dot(const Vec2& rhs) const {
                                    return mV[0]*rhs.mV[0] + mV[1]*rhs.mV[1]; }

inline Vec2 Vec2::perp() const {
    Vec2 v(-1.0 * this->mV[1], this->mV[0]);
    return v;
}

/** @brief returns norm of the vector (norm2).
 */
inline double Vec2::norm2() const { return sqrt(squaredNorm2()); }

/** @brief returns a squared norm.
 */
inline double Vec2::squaredNorm2() const { return mV[0]*mV[0] + mV[1]*mV[1]; }
                                          

/** @brief multiple the elements with scalar s */
inline void Vec2::scale(double s) { mV[0] = s * mV[0]; mV[1] = s * mV[1]; }

/** @brief set the values of this vector*/
inline void Vec2::set(double x, double y, double z) { mV[0] = x; mV[1] = y; }

inline void Vec2::set(double* v) { memcpy(mV.data(), v, sizeof(double)*2); }

inline void Vec2::setX(double x) { mV[0] = x; }
inline void Vec2::setY(double y) { mV[1] = y; }

inline void Vec2::zero() { memset(mV.data(), 0, sizeof(double)*2); }

inline void Vec2::normalize()
{ 
    double s = norm2(); 
    if (s >= EPSILON_SQUARED) {
        scale(1.0/s);
    }
}

inline double Vec2::x() const { return mV[0]; }
inline double Vec2::y() const { return mV[1]; }

inline double* Vec2::get_array() { return mV.data(); }

inline void Vec2::decDump(std::ostream& os) const
{
    double dx = x();
    double dy = y();
    unsigned char* p;
    p = (unsigned char*)&dx;
    for (long i = 0; i < 8; i++) {
        os << int(*p) << " ";
        p++;
    }
    p = (unsigned char*)&dy;
    for (long i = 0; i < 8; i++) {
        os << int(*p) << " ";
        p++;
    }
}

inline Mat3x3::Mat3x3() { memset(mV.data(), 0, sizeof(double)*9); }

inline Mat3x3::Mat3x3(double *v) { memcpy(mV.data(), v, sizeof(double)*9); }

inline Mat3x3::Mat3x3(double v1, double v2, double v3,
                      double v4, double v5, double v6,
                      double v7, double v8, double v9 ) {
    mV[0]=v1; mV[1]=v2; mV[2]=v3;
    mV[3]=v4; mV[4]=v5; mV[5]=v6;
    mV[6]=v7; mV[7]=v8; mV[8]=v9;
}

inline Mat3x3::Mat3x3(const Vec3& v1, const Vec3& v2, const Vec3& v3) {

    mV[0]=v1.x(); mV[1]=v2.x(); mV[2]=v3.x();
    mV[3]=v1.y(); mV[4]=v2.y(); mV[5]=v3.y();
    mV[6]=v1.z(); mV[7]=v2.z(); mV[8]=v3.z();
}

inline Mat3x3::~Mat3x3(){;}

inline Mat3x3& Mat3x3::operator += (const Mat3x3& rhs) {
    for (size_t i = 0; i<9; i++) {
         mV[i] += rhs.mV[i];
    }
    return *this;
}

inline Mat3x3& Mat3x3::operator -= (const Mat3x3& rhs) {
    for (size_t i = 0; i<9; i++) {
         mV[i] -= rhs.mV[i];
    }
    return *this;
}

inline bool Mat3x3::operator == (const Mat3x3& rhs) const {
    for(size_t i=0; i<9; i++){
       if (fabs(mV[i] - rhs.mV[i]) > EPSILON_LINEAR) {
            return false;
        }
    }
    return true;
}


inline bool Mat3x3::operator != (const Mat3x3& rhs) const {
    return !((*this) ==rhs);
}


inline Mat3x3 Mat3x3::operator + (const Mat3x3& rhs) const {
    return Mat3x3(*this) += rhs;
}

inline Mat3x3 Mat3x3::operator - (const Mat3x3& rhs) const {
    return Mat3x3(*this) -= rhs;
}

inline const Mat3x3 Mat3x3::transpose() const {
    Mat3x3 r;

    r.mV[0] = mV[0];
    r.mV[1] = mV[3];
    r.mV[2] = mV[6];
    r.mV[3] = mV[1];
    r.mV[4] = mV[4];
    r.mV[5] = mV[7];
    r.mV[6] = mV[2];
    r.mV[7] = mV[5];
    r.mV[8] = mV[8];
    return r;
}

inline const void Mat3x3::transposeInPlace() {
    swap(mV[1], mV[3]);
    swap(mV[2], mV[6]);
    swap(mV[5], mV[7]);
}

inline Mat3x3 Mat3x3::operator * (const Mat3x3& rhs) const {

    Mat3x3 r;

    r.mV[0] = mV[0]*rhs.mV[0] + mV[1]*rhs.mV[3] + mV[2]*rhs.mV[6];
    r.mV[1] = mV[0]*rhs.mV[1] + mV[1]*rhs.mV[4] + mV[2]*rhs.mV[7];
    r.mV[2] = mV[0]*rhs.mV[2] + mV[1]*rhs.mV[5] + mV[2]*rhs.mV[8];
    r.mV[3] = mV[3]*rhs.mV[0] + mV[4]*rhs.mV[3] + mV[5]*rhs.mV[6];
    r.mV[4] = mV[3]*rhs.mV[1] + mV[4]*rhs.mV[4] + mV[5]*rhs.mV[7];
    r.mV[5] = mV[3]*rhs.mV[2] + mV[4]*rhs.mV[5] + mV[5]*rhs.mV[8];
    r.mV[6] = mV[6]*rhs.mV[0] + mV[7]*rhs.mV[3] + mV[8]*rhs.mV[6];
    r.mV[7] = mV[6]*rhs.mV[1] + mV[7]*rhs.mV[4] + mV[8]*rhs.mV[7];
    r.mV[8] = mV[6]*rhs.mV[2] + mV[7]*rhs.mV[5] + mV[8]*rhs.mV[8];

    return r;
}

inline Mat3x3 Mat3x3::pow2() const {

    Mat3x3 r;

    r.mV[0] = mV[0]*mV[0] + mV[1]*mV[3] + mV[2]*mV[6];
    r.mV[1] = mV[0]*mV[1] + mV[1]*mV[4] + mV[2]*mV[7];
    r.mV[2] = mV[0]*mV[2] + mV[1]*mV[5] + mV[2]*mV[8];
    r.mV[3] = mV[3]*mV[0] + mV[4]*mV[3] + mV[5]*mV[6];
    r.mV[4] = mV[3]*mV[1] + mV[4]*mV[4] + mV[5]*mV[7];
    r.mV[5] = mV[3]*mV[2] + mV[4]*mV[5] + mV[5]*mV[8];
    r.mV[6] = mV[6]*mV[0] + mV[7]*mV[3] + mV[8]*mV[6];
    r.mV[7] = mV[6]*mV[1] + mV[7]*mV[4] + mV[8]*mV[7];
    r.mV[8] = mV[6]*mV[2] + mV[7]*mV[5] + mV[8]*mV[8];

    return r;
}

inline Vec3 Mat3x3::operator * (const Vec3& rhs) const {

    Vec3 r;

    r.setX(mV[0]*rhs.x()+mV[1]*rhs.y()+mV[2]*rhs.z());
    r.setY(mV[3]*rhs.x()+mV[4]*rhs.y()+mV[5]*rhs.z());
    r.setZ(mV[6]*rhs.x()+mV[7]*rhs.y()+mV[8]*rhs.z());

    return r;
}

inline double Mat3x3::det() const {

    return   mV[0]*(mV[4]*mV[8]-mV[5]*mV[7])
           + mV[1]*(mV[5]*mV[6]-mV[3]*mV[8])
           + mV[2]*(mV[3]*mV[7]-mV[4]*mV[6]);
}

inline const Mat3x3 Mat3x3::inverse() const {

    Mat3x3 r;
    double d = det();

    if (fabs(d) < EPSILON_CUBED) {
         
        throw std::underflow_error("MATRIX SINGULAR!");
    }

    r.mV[0] = (mV[4]*mV[8]-mV[5]*mV[7])/d;
    r.mV[1] = (mV[2]*mV[7]-mV[1]*mV[8])/d;
    r.mV[2] = (mV[1]*mV[5]-mV[2]*mV[4])/d;
    r.mV[3] = (mV[5]*mV[6]-mV[3]*mV[8])/d;
    r.mV[4] = (mV[0]*mV[8]-mV[2]*mV[6])/d;
    r.mV[5] = (mV[2]*mV[3]-mV[0]*mV[5])/d;
    r.mV[6] = (mV[3]*mV[7]-mV[4]*mV[6])/d;
    r.mV[7] = (mV[1]*mV[6]-mV[0]*mV[7])/d;
    r.mV[8] = (mV[0]*mV[4]-mV[1]*mV[3])/d;

    return r;
}

inline void Mat3x3::scale(double s) {

    mV[0] *= s; mV[1] *= s; mV[2] *= s;
    mV[3] *= s; mV[4] *= s; mV[5] *= s;
    mV[6] *= s; mV[7] *= s; mV[8] *= s;

}

inline double& Mat3x3::cell(size_t i, size_t j){ return mV[(i-1)*3+(j-1)]; }

inline double Mat3x3::val(size_t i, size_t j)const{return mV[(i-1)*3+(j-1)];}

inline double* Mat3x3::get_array() { return mV.data(); }

inline double Mat3x3::trace() const { return mV[0] + mV[4] + mV[8]; }

inline Vec3 Mat3x3::col(size_t i) const {
    auto base = i - 1;
    return Vec3(mV[base] , mV[base+3], mV[base+6]);
}

inline Vec3 Mat3x3::row(size_t i) const {
    auto base = (i - 1) * 3;
    return Vec3(mV[base], mV[base+1], mV[base+2]);
}

inline void Mat3x3::zero() { memset(mV.data(), 0, sizeof(double)*9); }


inline std::ostream& operator<<(std::ostream& os, const Mat3x3& M) {
    os << M.val(1,1) << "," << M.val(1,2) << "," << M.val(1,3) << "\n";
    os << M.val(2,1) << "," << M.val(2,2) << "," << M.val(2,3) << "\n";
    os << M.val(3,1) << "," << M.val(3,2) << "," << M.val(3,3) << "\n";
    return os;
}


}// namespace Makena


#endif/*_MAKENA_PRIMITIVES_HPP_*/





