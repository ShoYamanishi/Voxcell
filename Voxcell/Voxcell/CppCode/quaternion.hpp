#ifndef _MAKENA_QUATERNION_HPP_
#define _MAKENA_QUATERNION_HPP_

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <exception>
#include "primitives.hpp"

#ifdef UNIT_TESTS
#include "gtest/gtest_prod.h"
#endif

/**
 * @file quaternion.hpp
 *
 * @brief Implemens data structures and basic operations for Quaternion.
 *        it also finds the weighted averages of quaternions by a variant of
 *        QUEST algorithm.
 *
 *
 *        rotation (sin(θ/2), v·cos(θ/2))
 *        ===============================
 *
 *    ^
 *   1+####          ********
 *    |    ###    ****        ****  cos(θ/2) 
 *    |       ***                ***
 *    |    ***   ##                 ***
 *    |  **        ##                  **
 *    |**            ##                   **
 * ---*----------------##-------------------*-
 *   0|                 π ##                2π
 *    |                     ##
 *    |                       ###
 *    |                          ###
 *    |                             ###
 *  -1+                                ###### sin(θ/2) 
 *
 *
 * @reference
 *
 */

namespace Makena {

class Quaternion {

public:

    /** @brief default constructor */
    inline Quaternion();

    /** @brief constructor with s, x, y, and z values. */
    inline Quaternion(
           const double& s, const double& x, const double& y, const double& z);

    /** @brief another constructor with s, x, y, and z values. */
    inline Quaternion(const double& s, const Vec3& v);

    /** @brief constructor with direction vector and an angle around it. */
    inline Quaternion(const Vec3& d, const double& rad);
       
    /** @brief constructor from a rotation matrix. */
    inline Quaternion(Mat3x3& rm);

    /** @brief constructor from an orientation vector relative to 
     *        (1,0,0) and (0,1,0).
     */
    inline Quaternion(Vec3 v1, Vec3 v2);

    inline ~Quaternion();

    inline Quaternion& operator += (const Quaternion& rhs);

    inline Quaternion& operator -= (const Quaternion& rhs);

    inline bool operator == (const Quaternion& rhs) const ;

    inline bool operator != (const Quaternion& rhs) const ;

    inline const Quaternion operator + (const Quaternion& rhs) const;

    inline const Quaternion operator-(const Quaternion& rhs) const;

    inline const Quaternion operator*(const Quaternion& rhs) const;

    inline void scale(const double& s);

    inline const Quaternion conjugate() const;

    inline const Mat3x3 rotationMatrix() const;

    /** @brief returns 4x3 matrix Q in a double[12] in row-major. 
     *         The caller must supply the array of 12 elemnts.
     *                 +-----------+
     *                 | -x  -y  -z|
     *                 |           |
     *                 |  s   z  -y|
     *         Q = 1/2 |           |
     *                 | -z   s   x|
     *                 |           |
     *                 |  y  -x   s| 
     *                 +-----------+
     *
     *   @param  v (out): the array of double to which Q is written.
     */
    inline const void matrix4x3(double *v) const;

    /** @brief returns the time derivative of the orientation when the 
     *         angular velocity is given in w.
     *
     *  @param w (in): angular velocity
     */
    inline const Quaternion derivative(Vec3 const& w) const;

    /** @brief returns a vector rotated according to this quaternion, which
     *         is [cos(theta), n*sin(theta)] where theta is the angle and
     *         n is the normalized axis vector.
     *
     *  @param  p (in) : vector to be rotated
     *
     *  @return the vector rotated
     */
    inline const Vec3 rotate(Vec3 const& p) const;

    inline double s() const;
    inline double i() const;
    inline double j() const;
    inline double k() const;
    inline double x() const;
    inline double y() const;
    inline double z() const;

    inline void normalize();

    static constexpr double EPSILON_EQUAL = 0.00000001;

    /** @brief finds the average of quaternions as in the following formula
     *
     *      _                                            2
     *      q =   argmin   { Sigma_i=1^n [ ||A(q) - A(qi)|| ] }
     *           q \in S^3                               F
     *
     *              _ 
     *      where   q    :  the desired average
     *
     *              S^3  :  the unit 3-sphere
     *
     *              A(q) :  the rotation matrix (aka attitude matrix)
     *
     *                  2
     *             ||X|| :  Frobenius norm of matrix X
     *                  F
     *      This is solved by QUEST algorithm.
     *
     *  @param quats   (in): list of quaternions
     *
     *  @param weights (in): list of weights per quaternion. 
     *                       The sum must be 1.0.
     *
     *  @return average quaterion
     *
     *  @remark this algorithm is susceptible to numerical drifts, and the
     *          weight must add up to 1.0.
     */
    static Quaternion average(
        vector<Quaternion>& quats,
        vector<double>&     weights
    );

protected:

    inline void fromMatrix3x3(Mat3x3& rm);

    double mS;
    Vec3   mV;



#ifdef UNIT_TESTS
friend class QuaternionTests;
#endif

};


inline Quaternion::Quaternion():mS(0.0){;}

inline Quaternion::Quaternion(
    const double& s,
    const double& x,
    const double& y,
    const double& z
):mS(s),mV(x,y,z){;}


inline Quaternion::Quaternion(const double& s, const Vec3& v):mS(s),mV(v){;}

inline Quaternion::Quaternion(const Vec3& d, const double& rad):mV(d) {
    mS = cos(rad/2.0);
    mV.scale(sin(rad/2.0)/d.norm2());
}


inline void Quaternion::fromMatrix3x3(Mat3x3& rm) {

    /*
     * From the rotaion matrix:
     *  r11 = s2+x2-y2-z2        |\      r11+r22+r33+1 = 4*s2
     *  r22 = s2-x2+y2-z2    ----  \     r11-r22-r33+1 = 4*x2
     *  r33 = s2-x2-y2+z2    ----  /    -r11+r22-r33+1 = 4*y2
     *    1 = s2+x2+y2+z2        |/     -r11-r22+r33+1 = 4*z2
     */
    double s2_4 = 1.0 + rm.cell(1,1) + rm.cell(2,2) + rm.cell(3,3);
    double x2_4 = 1.0 + rm.cell(1,1) - rm.cell(2,2) - rm.cell(3,3);
    double y2_4 = 1.0 - rm.cell(1,1) + rm.cell(2,2) - rm.cell(3,3);
    double z2_4 = 1.0 - rm.cell(1,1) - rm.cell(2,2) + rm.cell(3,3);

    if (s2_4 > x2_4 && s2_4 > y2_4 && s2_4 > z2_4) {

        // s = 0.5*sqrt(s2_4)
        // x = (r32 - r23)/4s = (2*(sx+yz)-2*(yz-sx))/4s = 4sx/4s = x
        // y = (r13 - r31)/4s = (2*(sy+xz)-2*(xz-sy))/4s = 4sy/4s = y
        // z = (r21 - r12)/4s = (2*(xy+sz)-2*(xy-sz))/4s = 4sz/4s = z

        mS    = 0.5 * sqrt(s2_4);
        auto s4 = mS * 4.0;
        mV[1] = (rm.cell(3,2) - rm.cell(2,3))/s4;
        mV[2] = (rm.cell(1,3) - rm.cell(3,1))/s4;
        mV[3] = (rm.cell(2,1) - rm.cell(1,2))/s4;

    }
    else if (x2_4 > y2_4 && x2_4 > z2_4) {

        // s = (r32 - r23)/4x  = (2*(sx+yz)-2*(yz-sx))/4s = 4sx/4x = s
        // x = 0.5*sqrt(x2_4)
        // y = (r12 + r21)/4s = (2*(xy+sz)+2*(xy-sz))/4s = 4xy/4x = y
        // z = (r13 + r31)/4s = (2*(sy+xz)+2*(xz-sy))/4s = 4xz/4x = z

        mV[1] = 0.5 * sqrt(x2_4);
        auto x4 = mV[1] * 4.0;
        mS    = (rm.cell(3,2) - rm.cell(2,3))/x4;
        mV[2] = (rm.cell(1,2) + rm.cell(2,1))/x4;
        mV[3] = (rm.cell(1,3) + rm.cell(3,1))/x4;

    }
    else if (y2_4 > z2_4) {

        // s = (r13 - r31)/4y = (2*(sy+xz)-2*(xz-sy))/4y = 4sy/4y = s
        // x = (r12 + r21)/4y = (2*(xy+sz)+2*(xy-sz))/4y = 4xy/4y = x
        // y = 0.5*sqrt(y2_4)
        // z = (r32 + r23)/4y  = (2*(sx+yz)+2*(yz-sx))/4y = 4yz/4y = z

        mV[2] = 0.5 * sqrt(y2_4);
        auto y4 = mV[2] * 4.0;
        mS    = (rm.cell(1,3) - rm.cell(3,1))/y4;
        mV[1] = (rm.cell(1,2) + rm.cell(2,1))/y4;
        mV[3] = (rm.cell(3,2) + rm.cell(2,3))/y4;

    }
    else {

        // s = (r21 - r12)/4z = (2*(xy+sz)-2*(xy-sz))/4z = 4sz/4z = s
        // x = (r13 + r31)/4z = (2*(sy+xz)+2*(xz-sy))/4z = 4xz/4z = x
        // y = (r32 + r23)/4y = (2*(sx+yz)+2*(yz-sx))/4z = 4yz/4z = y
        // z = 0.5*sqrt(z2_4)

        mV[3] = 0.5 * sqrt(z2_4);
        auto z4 = mV[3] * 4.0;
        mS    = (rm.cell(2,1) - rm.cell(1,2))/z4;
        mV[1] = (rm.cell(1,3) + rm.cell(3,1))/z4;
        mV[2] = (rm.cell(3,2) + rm.cell(2,3))/z4;

    }
}

inline Quaternion::Quaternion(Mat3x3& rm){
    fromMatrix3x3(rm);
}

inline Quaternion::Quaternion(Vec3 v1, Vec3 v2)
{
    // [  ][  ]   [   ][1][0]    [  ][  ]   [[  ][  ][  ]][1][0]
    // [v1][v2] = [ R ][0][1] => [v1][v2] = [[v1][v2][v3]][0][1]
    // [  ][  ]   [   ][0][0]    [  ][  ]   [[  ][  ][  ]][0][0]
    //
    // v3 = v1 x v2

    v1.normalize();
    v2.normalize();
    if (v1.dot(v2) > EPSILON_SQUARED) {
        // Safety. v1, v2, and v3 should be orthonormal.
        mS = 1.0;
        mV.setX(0.0);
        mV.setY(0.0);
        mV.setZ(0.0);
    }
    else {
        Vec3 v3 = v1.cross(v2);
        v3.normalize();
        Mat3x3 M(v1, v2, v3);
        fromMatrix3x3(M);
    }
}

inline Quaternion::~Quaternion(){;}

inline Quaternion& Quaternion::operator += (const Quaternion& rhs){
    mS += rhs.mS; mV += rhs.mV;
    return *this;
}

inline Quaternion& Quaternion::operator -= (const Quaternion& rhs){
    mS -= rhs.mS; mV -= rhs.mV;
    return *this;
}

inline bool Quaternion::operator == (const Quaternion& rhs) const {
                  return (fabs(mS - rhs.mS) < EPSILON_EQUAL) && mV == rhs.mV; }

inline bool Quaternion::operator != (const Quaternion& rhs) const {
                                                    return !((*this) == rhs); }

inline const Quaternion Quaternion::operator + (const Quaternion& rhs) const {
                                             return Quaternion(*this) += rhs; }

inline const Quaternion Quaternion::operator-(const Quaternion& rhs) const {
                                             return Quaternion(*this) -= rhs; }

inline const Quaternion Quaternion::operator*(const Quaternion& rhs) const {

    Quaternion r;

    r.mS = mS * rhs.mS - mV.dot(rhs.mV);
    Vec3 sv1(rhs.mV);
    sv1.scale(mS);
    Vec3 sv2(mV);
    sv2.scale(rhs.mS);
    r.mV = sv1 + sv2 + mV.cross(rhs.mV);
    // The PI (180) duality is solved in favor of (1,0) to avoid ambiguity.
    if (r.mV.x()==0.0&&r.mV.y()==0.0&&r.mV.z()==0.0&&r.mS==-1.0) {
        r.mS = 1.0;
    }
    return r;
}

inline void Quaternion::scale(const double& s) { mS = s * mS; mV.scale(s); }

inline const Quaternion Quaternion::conjugate() const {
    Quaternion r;
    r.mS = mS;
    r.mV = mV;
    r.mV.scale(-1.0);
    return r;
}


/**
 *
 * @reference http://www.ee.ucr.edu/~farrell/AidedNavigation/
 *                                           D_App_Quaternions/Rot2Quat.pdf
 *
 *    +---------------------------------------------------+
 *    |s^2+x^2-y^2-z^2      2*(xy-sz)         2*(sy+xz)   |
 *    |   2(xy+sz)       s^2-x^2+y^2-z^2      2*(yz-sx)   |
 *    |   2(xz-sy)          2*(sx+yz)      s^2-x^2-y^2+z^2|
 *    +---------------------------------------------------+
 *
 */
inline const Mat3x3 Quaternion::rotationMatrix() const {
    Mat3x3 r;
    double s2 = mS * mS;
    double x2 = mV.x() * mV.x();
    double y2 = mV.y() * mV.y();
    double z2 = mV.z() * mV.z();
    double sx = mS  * mV.x();
    double sy = mS  * mV.y();
    double sz = mS  * mV.z();
    double xy = mV.x() * mV.y();
    double xz = mV.x() * mV.z();
    double yz = mV.y() * mV.z();

    r.cell(1,1) = s2 + x2 - y2 - z2;
    r.cell(1,2) = 2.0*(xy - sz);
    r.cell(1,3) = 2.0*(xz + sy);
    r.cell(2,1) = 2.0*(xy + sz);
    r.cell(2,2) = s2 - x2 + y2 - z2;
    r.cell(2,3) = 2.0*(yz - sx);
    r.cell(3,1) = 2.0*(xz - sy);
    r.cell(3,2) = 2.0*(yz + sx);
    r.cell(3,3) = s2 - x2 - y2 + z2;

    return r;
}


/** @brief returns 4x3 matrix Q in a double[12] in row-major.
 *         The caller must supply the array of 12 elemnts.
 *                 +-----------+
 *                 | -x  -y  -z|
 *                 |           |
 *                 |  s   z  -y|
 *         Q = 1/2 |           |
 *                 | -z   s   x|
 *                 |           |
 *                 |  y  -x   s|
 *                 +-----------+
 *
 *   @param  v (out): the array of double to which Q is written.
 */
inline const void Quaternion::matrix4x3(double *v) const {

    v[0] = -1.0*mV.x() * 0.5; // a11
    v[1] = -1.0*mV.y() * 0.5; // a12
    v[2] = -1.0*mV.z() * 0.5; // a13
    v[3] = mS          * 0.5; // a21
    v[4] = mV.z()      * 0.5; // a22
    v[5] = -1.0*mV.y() * 0.5; // a23
    v[6] = -1.0*mV.z() * 0.5; // a31
    v[7] = mS          * 0.5; // a32
    v[8] = mV.x()      * 0.5; // a33
    v[9] = mV.y()      * 0.5; // a41
    v[10]= -1.0*mV.x() * 0.5; // a42
    v[11]= mS          * 0.5; // a43
}

/** @brief returns the time derivative of the orientation when the
 *         angular velocity is given in w.
 *
 *  @param w (in): angular velocity
 */
inline const Quaternion Quaternion::derivative(Vec3 const& w) const {

    Quaternion t(*this);
    Quaternion zw(0.0, w);
    Quaternion r;
    r = zw * t;
    r.scale(0.5);
    return r;

}

/** @brief returns a vector rotated according to this quaternion, which
 *         is [cos(theta), n*sin(theta)] where theta is the angle and
 *         n is the normalized axis vector.
 *
 *  @param  p (in) : vector to be rotated
 *
 *  @return the vector rotated
 */
inline const Vec3 Quaternion::rotate(Vec3 const& p) const {

    Quaternion p2(0.0, p);
    Quaternion e1(*this);
    Quaternion e2;
    e2 = e1 * p2 * conjugate();
    return e2.mV;

}

inline double Quaternion::s() const { return mS; }
inline double Quaternion::i() const { return mV.x(); }
inline double Quaternion::j() const { return mV.y(); }
inline double Quaternion::k() const { return mV.z(); }
inline double Quaternion::x() const { return mV.x(); }
inline double Quaternion::y() const { return mV.y(); }
inline double Quaternion::z() const { return mV.z(); }

inline void Quaternion::normalize() {
    double squaredNorm   = mS * mS + mV.squaredNorm2();
    double invNorm = 1.0 / sqrt(squaredNorm);
    mS = mS * invNorm;
    mV.scale(invNorm);
}

}// namespace Makena


#endif/*_MAKENA_QUATERNION_HPP_*/


