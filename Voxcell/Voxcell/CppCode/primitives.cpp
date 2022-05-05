#include "primitives.hpp"

/**
 * @file primitives.hpp
 *
 * %brief
 *
 * @reference
 *
 */

namespace Makena {

using namespace std;


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
Mat3x3 Mat3x3::EigenVectorsIfSymmetric(Vec3& eValues)
{
    /** Check if the matrix is 0.
     */
    double absMax = 0.0;
    for (size_t i = 0; i < 9; i++) {
        absMax = std::max(absMax, fabs(mV[i]));
    }

    if (absMax < EPSILON_LINEAR) {

        eValues[1]  = 0.0;
        eValues[2]  = 0.0;
        eValues[3]  = 0.0;

        Mat3x3 E;
        E.mV[0] = 1.0;
        E.mV[4] = 1.0;
        E.mV[8] = 1.0;
        return E;
    }
    /* Check the off-diagonal elements.
     * If either or both of the upper and the lower off-diagonal elements
     * are zero, the diagonal elements are eigen values and axis unit vectors
     * are the eigen vectors.
     */
    if (fabs(mV[1]) + fabs(mV[2]) + fabs(mV[5]) < EPSILON_LINEAR || 
        fabs(mV[3]) + fabs(mV[6]) + fabs(mV[7]) < EPSILON_LINEAR    ) {
        eValues[1]  = mV[0];
        eValues[2]  = mV[4];
        eValues[3]  = mV[8];

        Vec3 Ev1(1.0, 0.0, 0.0);
        Vec3 Ev2(0.0, 1.0, 0.0);
        Vec3 Ev3(0.0, 0.0, 1.0);
       
        // Bubble sort the eigen values in the descending order
        if (fabs(eValues[1]) < fabs(eValues[2])){
            std::swap(eValues[1], eValues[2]);
            std::swap(Ev1, Ev2);
        }
        if (fabs(eValues[2]) < fabs(eValues[3])){
            std::swap(eValues[2], eValues[3]);
            std::swap(Ev2, Ev3);
        }
        if (fabs(eValues[1]) < fabs(eValues[2])){
            std::swap(eValues[1], eValues[2]);
            std::swap(Ev1, Ev2);
        }
        Mat3x3 E(Ev1, Ev2, Ev3);
        return E;
    }
    /* Normalize the elements to avoid numerical overflow according to
     * DE[14]
     */
    Mat3x3 A(*this);
    A.scale(1.0/absMax);

    /* Find eigen values. alpha1 <= alhpa2 <= alpha3 */
    double alpha1, alpha2, alpha3;
    A.EigenValuesIfSymmetric(alpha1, alpha2, alpha3);
    if (fabs(alpha1) <= EPSILON_LINEAR &&
        fabs(alpha2) <= EPSILON_LINEAR &&
        fabs(alpha3) <= EPSILON_LINEAR    ) {

        eValues[1]  = 0.0;
        eValues[2]  = 0.0;
        eValues[3]  = 0.0;

        Mat3x3 E;
        E.mV[0] = 1.0;
        E.mV[4] = 1.0;
        E.mV[8] = 1.0;
        return E;
    }

    Vec3 Ev1, Ev2, Ev3;
    if ((alpha3 - alpha2) > (alpha2 - alpha1)) {
        Ev3 = A.findEigenVectorByCrossProducts(alpha3);
        Ev2 = A.findEigenVectorInSubSpace(Ev3, alpha2);
        Ev1 = Ev3.cross(Ev2);
        Ev1.normalize();
    }
    else {
        Ev1 = A.findEigenVectorByCrossProducts(alpha1);
        Ev2 = A.findEigenVectorInSubSpace(Ev1, alpha2);
        Ev3 = Ev2.cross(Ev1);
        Ev3.normalize();
    }

    eValues[1] = alpha3;
    eValues[2] = alpha2;
    eValues[3] = alpha1;

    eValues.scale(absMax);

    Mat3x3 E(Ev3, Ev2, Ev1);
    return E;
}


/** @brief finds engen values assuming this matrix is symmetric.
 *         Let A be this matrix. An eigen value lambda meets the following.
 *
 *
 *  @param a1 a2, a3 (out): eigen values a1 <= a2 <= a3
 *
 *             det(A - lambda * I) = 0
 *
 *         We solve the following instead of above.
 *
 *             det(beta * I - B) = beta^3 = 3*beta - det(B) = 0
 *
 *                                where  A = p*B + q*I
 *                                       p = sqrt (tr((A - q*I)^2)/6)
 *                                       q = tr(A)/3
 *         The solusion is given by
 *
 *              beta = 2cos(acos(det(B)/2)/3 + (2pi/3)*k), k = 0, 1, 2.
 *
 *              beta1 = 2cos(acos(det(B)/2)/3 + (2pi/3))
 *              beta2 = 2cos(acos(det(B)/2)/3 + (4pi/3))
 *              beta3 = 2cos(acos(det(B)/2)/3 )
 *
 *         And finally,
 *              lambda1 = p * beta1 + q
 *              lambda2 = p * beta2 + q
 *              lambda3 = p * beta3 + q
 *
 *
 *         Note
 *              beta1 <= beta2 <= beta3
 *              beta1 < 0,    0 < beta3
 *
 *        |det(B)| <= 2 =>  0 <= acos(det(B)/2)/3 <= pi/3
 *
 *
 *        det(B) = 2   -1 = beta1 = beta3 < 0 < beta1 = 2
 *        -----------
 *
 *               beta1 0.5pi
 *                   \   |
 *                    \  |
 *                     \ |
 *                      \|
 *          pi-----------+=========---> [0,2pi)
 *                      /|       beta3
 *                     / |
 *                    /  |
 *                   /   |
 *                beta2 1.5pi
 *
 *
 *
 *        det(B) =  0  -sqrt(3) = beta1 < 0 beta2 = 0 < beta3 = sqrt(3)
 *        -----------
 *
 *               beta1 0.5pi
 *                       |
 *                       |
 *         beta1--__     |    __--beta3
 *                   --__|__--
 *          pi-----------+------------> [0,2pi)
 *            beta1      #
 *                       #
 *                       #
 *                       #beta2
 *                       |
 *                      1.5pi beta2
 *
 *
 *
 *        det(B) = -2  -2 = beta1 < 0 < beta2 = beta3 = 1
 *        -----------
 *
 *               beta1 0.5pi  beta3
 *                       |   /
 *                       |  /
 *                       | /
 *                       |/
 *          pi-==========+------------> [0,2pi)
 *            beta1      |\
 *                       | \
 *                       |  \
 *                       |   \
 *                      1.5pi beta2
 *
 */
void Mat3x3::EigenValuesIfSymmetric(double& a1, double& a2, double& a3)
{
    double q  = trace()/3.0;

    Mat3x3 M1(*this);
    M1.mV[0] -= q;
    M1.mV[4] -= q;
    M1.mV[8] -= q;         // M1 = A - qI

    Mat3x3 M2 = M1.pow2(); // M2 = (A - qI)^2
    double p = sqrt(M2.trace()/6.0);
    if (p < EPSILON_LINEAR) {
        // Possibly rank 1
        a1 = 0.0;
        a2 = 0.0;
        a3 = 0.0;
    }

    Mat3x3& B(M1);
    B.scale(1.0/p); // B = M1 = (A - qI)/p
    double Bdet = B.det();
    if (Bdet > 2.0) {
        Bdet = 2.0;
    }
    if (Bdet < -2.0) {
        Bdet = -2.0;
    }

    double theta = acos(Bdet/2.0) / 3.0;

    // beta1 <= beta2 <= beta 3
    double beta1 = 2.0 * cos(theta+PI2OVER3);
    double beta2 = 2.0 * cos(theta+2.0*PI2OVER3);
    double beta3 = 2.0 * cos(theta);

    a1 = p * beta1 + q;
    a2 = p * beta2 + q;
    a3 = p * beta3 + q;
}


/** @brief find the corresponding eigen vector for
 *         the given eigen value.
 *
 *         It finds the eigen vector by solving
 *
 *             (A - lambda * I)V = 0
 *
 *            +----------+ +-+
 *            |[  r1^t  ]| | |         r1 dot V = 0
 *            |[  r2^t  ]| |V| = 0 =>  r2 dot V = 0 => V is perpendicular to
 *            |[  r3^t  ]| | |         r3 dot V = 0    r1, r2, and r3.
 *            +----------+ +-+
 *             NOTE: The rank of A-lambda*I is 2, and two of r1, r2, and r3 are
 *                   colinear.
 *                   The eigen vector is found as the cross product of the two
 *                   independent vectors among r1, r2, and r3.
 *
 *  @param lambda (in): eigen value.
 *
 *  @return Eigen vector
 */
Vec3 Mat3x3::findEigenVectorByCrossProducts(double lambda)
{
    Vec3 r1(mV[0] - lambda, mV[1],          mV[2]         );
    Vec3 r2(mV[3],          mV[4] - lambda, mV[5]         );
    Vec3 r3(mV[6],          mV[7],          mV[8] - lambda);

    Vec3 cross12 = r1.cross(r2);
    Vec3 cross23 = r2.cross(r3);
    Vec3 cross31 = r3.cross(r1);

    double dist12 = fabs(cross12.squaredNorm2());
    double dist23 = fabs(cross23.squaredNorm2());
    double dist31 = fabs(cross31.squaredNorm2());

    if (dist12 > dist23) {
        if (dist12 > dist31) {
            cross12.scale(1.0/sqrt(dist12));
            return cross12;
        }
        else {
            cross31.scale(1.0/sqrt(dist31));
            return cross31;
        }
    }
    else {
        if (dist23 > dist31) {
            cross23.scale(1.0/sqrt(dist23));
            return cross23;
        }
        else {
            cross31.scale(1.0/sqrt(dist31));
            return cross31;
        }
    }
}


/** @brief find the second  eigen vector for
 *         the 2nd eigen value after the 1st one is dicsovered.
 *         The multiplicity of the remining eigen values may be 2,
 *         in which case, the rank of the subspace is 0.
 *
 *  @param Ev1     (in): 1st Eigen vector
 *  @param lambda2 (in): 2nd Eigen value
 *
 *  @return 2nd Eigen vector
 */

Vec3 Mat3x3::findEigenVectorInSubSpace(Vec3& Ev1, double lambda2)
{
    // Find 2 arbitrary spanning vectors for the 2d subspace.
    Vec3 Sv1 = Ev1.perp();
    Vec3 Sv2 = Ev1.cross(Sv1);
    Vec3 Zero;

    Sv1.normalize();
    Sv2.normalize();

    Mat3x3 A_aI(*this);
    A_aI.mV[0] -= lambda2;
    A_aI.mV[4] -= lambda2;
    A_aI.mV[8] -= lambda2;

    Mat3x3 J(Sv1, Sv2, Zero);
    Mat3x3 Jt(J);
    Jt.transpose();

    // M is actually a 2x2 matrix projected in the axes of Sv1 and Sv2.
    Mat3x3 M = Jt * A_aI * J;
    double fm11 = fabs(M.cell(1,1));
    double fm12 = fabs(M.cell(1,2));
    double fm21 = fabs(M.cell(2,1));
    double fm22 = fabs(M.cell(2,2));

    if ( (fm11+fm12+fm21+fm22) < EPSILON_LINEAR ) {
        // Zero matrix. The original matrix is rank 1. 
        // Returning an arbitrary vector in the subspace.
        return Sv1;
    }

    double x1, x2;
    if (fm11 > fm22) {
        double scale = 1.0/sqrt(fm11*fm11 + fm12*fm12);
        x1 = M.cell(1,2)*scale;
        x2 = M.cell(1,1)*scale;
    }
    else {
        double scale = 1.0/sqrt(fm21*fm21 + fm22*fm22);
        x1 = M.cell(2,2)*scale;
        x2 = M.cell(2,1)*scale;
    }

    Sv1.scale(x1);
    Sv2.scale(x2);

    return Sv1 - Sv2;
}


Mat3x3 findPrincipalComponents(vector<Vec3>& points, Vec3& spread, Vec3& mean)
{
    const Vec3 zero(0.0, 0.0, 0.0);
    if (points.size()==0) {
        mean   = zero;
        spread = zero;
        Mat3x3 dummy;
        return dummy;
    }
    Vec3   m;
    for(auto& p : points) {
        m += p;
    }
    m.scale(1.0/points.size());
    Mat3x3 cov;
    for(auto& p : points) {
        Vec3   d = p - m;
        Mat3x3 c;
        c.cell(1,1) = d[1]*d[1];
        c.cell(1,2) = d[1]*d[2];
        c.cell(1,3) = d[1]*d[3];
        c.cell(2,1) = c.cell(1,2);
        c.cell(2,2) = d[2]*d[2];
        c.cell(2,3) = d[2]*d[3];
        c.cell(3,1) = c.cell(1,3);
        c.cell(3,2) = c.cell(2,3);
        c.cell(3,3) = d[3]*d[3];
        cov += c;
    }
    if (points.size()==1) {
        Mat3x3 dummy;
        spread = zero;
        return dummy;
    }
    cov.scale(1.0/(double(points.size())-1.0));

    Mat3x3 pca = cov.EigenVectorsIfSymmetric(spread);
    mean = m;

    return pca;
}


void findPrincipalComponents(
    vector<Vec2>& points,
    Vec2&         spread,
    Vec2&         mean,
    Vec2&         axis1,
    Vec2&         axis2
) {
    if (points.size()==0) {
        spread = Vec2(0.0, 0.0);
        mean   = Vec2(0.0, 0.0);
        axis1  = Vec2(1.0, 0.0);
        axis2  = Vec2(0.0, 1.0);
    }

    Vec2 m(0.0, 0.0);
    for (auto& p : points) {
        m += p;
    }
    m.scale(1.0/points.size());
    mean = m;

    if (points.size()==1) {
        spread = Vec2(0.0, 0.0);
        axis1  = Vec2(1.0, 0.0);
        axis2  = Vec2(0.0, 1.0);
    }

    double cov11 = 0.0;
    double cov12 = 0.0;
    double cov22 = 0.0;
    for (auto& p : points) {
        Vec2 d = p - m;
        cov11 += (d.x() * d.x());
        cov12 += (d.x() * d.y());
        cov22 += (d.y() * d.y());
    }
    double denom = double(points.size())- 1.0;

    cov11 = cov11 / denom;
    cov12 = cov12 / denom;
    cov22 = cov22 / denom;
    findEigenVectors(cov11, cov12, cov12, cov22, spread, axis1, axis2);

    if (fabs(spread.x()) < fabs(spread.y())) {
        auto tmp = spread.x();
        spread.setX(spread.y());
        spread.setY(tmp);
        std::swap(axis1, axis2);
    }

}


void findEigenVectors(
   double m11,
   double m12,
   double m21,
   double m22,
   Vec2&  values,
   Vec2&  v1,
   Vec2&  v2
) {

    double det = m11 * m22 - m12 * m21;

    if (fabs(det) <= EPSILON_SQUARED) {
        values = Vec2(0.0, 0.0);
        v1 = Vec2(1.0, 0.0);
        v2 = Vec2(0.0, 1.0);
        return ;
    }

    if (fabs(m21) < EPSILON_LINEAR && fabs(m12) < EPSILON_LINEAR) {
        values = Vec2(m11, m22);
        v1     = Vec2(1.0, 0.0);
        v2     = Vec2(0.0, 1.0);
        return ;
    }

    double trace      = m11 + m22;
    double halfTrace  = trace / 2.0;
    double commonExpr = sqrt(trace*trace/4.0 - det);
    double L1         = halfTrace + commonExpr;
    double L2         = halfTrace - commonExpr;

    values = Vec2(L1, L2);

    if (fabs(m21) >= EPSILON_LINEAR) {
        v1 = Vec2(L1 - m22, m21);
        v2 = Vec2(L2 - m22, m21);

    }
    else {
        v1 = Vec2(m12, L1 - m11);
        v2 = Vec2(m12, L2 - m11);
    }
    v1.normalize();
    v2.normalize();
}


}// namespace Makena

