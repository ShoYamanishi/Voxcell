#include "quaternion.hpp"

/**
 * %file quaternion.cpp
 *
 * %brief
 *
 * %reference
 *
 */

namespace Makena {

using namespace std;


/** @brief finds the average of the given quaternions
 * 
 *  %param quats   (in): list of quaternions
 *
 *  %param weights (in): list of weights per quaternion. The sum must be 1.0.
 *
 *
 *  %return average quaterion
 *
 *  @remark this algorithm is susceptible to numerical drifts, and the weight
 *          must add up to 1.0.
 *          
 *
 *  @remark on the algorithm
 *
 *  The average of quaternions is defined as follows: [1]
 *
 *                                               2
 *     Minimize u(q) = 1/2 Σ( wi ||R(q) - R(qi)||  )
 *                         i                     F
 *
 *     s.t. q is a normalized quaternion representing a rotation
 *
 * where 
 *      qi     : the i-th quaternion
 *
 *      wi     : the weight for the i-th quaternion for averaging
 *
 *      R(q)   : 3x3 rotation matrix (a.k.a. attitude matrix) of q
 *
 *           2
 *      ||M||  : Frobenium norm of 3x3 matrix
 *           F
 *
 * In other words, the average of the quaternions is the quaterion
 * whose total distance to the others is minimized in Frobenius norm sense.
 *
 * [1] states minimizing u(q) is equivalent to maximizing the following
 * function called gain function
 *
 *                     t
 *    g(q) = Tr[ R(q)*B  ]
 *
 * where  B  =  Σ (wi * R(qi)) and Σ (wi) = 1.0
 *              i                  i
 *
 *
 *                    |  |   |xq|
 *                    |Vq|   |yq|
 * Let quaternion q = |  | = |zq|
 *                    |--|   |--|
 *                    |sq|   |sq|
 *
 *
 * Then
 *                                               t
 *     R(q) = ( (sq)^2 - (Vq dot Vq) ) I + 2 Vq*Vq - 2sqMq
 *
 *                 |  0 -zq  yq|
 *                 |           |
 *     where  Mq = | zq   0 -xq|
 *                 |           |
 *                 |-yq  xq   0|
 *
 * and then gain function becomes:
 *
 *                                     t               t  t
 *     g(q) = (sq)^2 - (Vq dot Vq) Tr[B ] + 2 Tr [ Vq Vq B ]
 *
 *                           t
 *            - 2sq Tr[Mq * B ]
 *
 * This process is essentially the same as the QUEST algorithm in [2].
 *
 * Let 
 *
 *     σ  =  Tr [B]
 *
 *               t
 *     Sm = B + B
 *                                      
 *                            x    t     
 *     z  : vector such that z  = B  - B,
 *
 *                    | 0  -k   j |
 *               x    |           |          |i|
 *          and z  =  | k   0  -i | when z = |j|
 *                    |           |          |k|
 *                    |-j   i   0 |
 *
 * Element-wise:
 *
 *     σ  =  Σ ( ai (3(si)^2 - (xi)^2 - (yi)^2 - (zi)^2) )
 *           i
 *
 *       |Σai(si^2+xi^2-yi^2-zi^2)   2 Σ ai(xi yi)        2 Σ ai(xi zi)       |
 *       |                                                                    |
 * Sm = 2|    2 Σ ai(xi yi)     Σai(si^2-xi^2+yi^2-zi^2)  2 Σ ai(yi zi)       |
 *       |                                                                    |
 *       |    2 Σ ai(xi zi)          2 Σ ai(yi zi)    Σai(si^2-xi^2-yi^2+zi^2)|
 *
 *
 *           | Σ ai (si xi) |
 *    z = -4 | Σ ai (si yi) |
 *           | Σ ai (si zi) |
 *
 *
 *                        |xi|
 *                        |yi|
 *  where  qaternion qi = |zi|
 *                        |--|
 *                        |si|
 *           
 *
 * Then
 *                                       t
 *     g(q) = ((sq)^2 - (Vq dot Vq))σ + Vq*Sm*Vq + 2s(qVq*z)
 *
 *                               |  |
 *                               |Vq|
 *     g(q) = [ Vq^t |sq ] * K * |  |
 *                               |--|
 *                               |sq|
 * where
 * 
 *         |       | |
 *         |Sm - σI|Z|
 *     K = |       | |
 *         |-------+-|
 *         | Z^t   |σ|
 *
 * Then g(q) is maximized when [Vq|xq] is the eigen vector of the largest
 * eigen value λ of K.
 * 
 *    |       | | |  |    |  |
 *    |Sm - σI|Z| |Vq|    |Vq|
 *    |       | | |  | = λ|  |
 *    |-------+-| |--|    |--|
 *    | Z^t   |σ| |sq|    |sq|
 *
 * 
 * If decomposed into two parts:
 *
 *    (S - σI)Vq + Z*sq = λVq   => (S - (σ + λ)I)Vq + Zsq = 0
 * 
 *                                                 -1
 *                            => Vq/sq = ((σ+λ)I - S) Z
 *
 *    Z dot Vq + σ(sq) = λ(sq)  => Z dot Vq/(sq) + σ = λ
 *
 * Let y = Vq/sq
 *
 * Then Vq/sq = (Vq/|Vq|)(|Vq|/sq)
 *            = (Vq/|Vq|)*(cos(θ/2)/sin(θ/2))
 *            = (Vq/|Vq|)*tan(θ/2)
 *
 * Pleas note lim |y| = ∞ (infinity)
 *            θ->π
 *
 * Substituting y into the above:
 *
 *                    -1
 *     y = ((σ+λ)I - S) z
 * 
 *     λ = z dot y + σ
 * 
 * When the rotation angle θ approaches π, then |y| approaches inifinity, and
 * therefore ((σ+λ)I - S) becomes singular, and finding inverse of it 
 * becomes impossible. We can represent the inverse of ((σ+λ)I - S) in 
 * a quadratic polynomial form after expanding the inverse into a power series
 * and then reducing them into lower order polynomial using Cayley-Hamilton
 * theorem.
 *
 *                -1
 *    ((σ+λ)I - S)  =  (αI + βS + S^2)/γ
 *
 *    α = λ^2 - σ^2 + κ
 *
 *    β = λ - σ
 *
 *    γ = (λ+σ)(λ^2 - σ^2 + κ) - Δ
 *
 *    κ = Tr[adj(S)]
 *
 *    Δ = det(S)
 *
 *    σ =  Tr [B] (the same as the previous definition)
 *
 * Then:
 *
 *    y = x / γ where x = (αI + βS + S^2)z
 *
 * and the optimum solution q_opt is:
 *
 *                                  | |
 *                                  |x|
 *     q_opt = 1.0/sqrt(γ^2 + |x|^2)| |
 *                                  |-|
 *                                  |γ|
 *
 *
 * According to [1] and [2], the characteristic equation is:
 *
 *     f(λ) = λ^4 - (a+b)λ^2 - cλ + (ab + cσ - d)
 *
 *     a = σ^2 - κ
 *             
 *     b = σ^2 + (z dot z)
 *               t
 *     c = Δ  + z * Sm * z
 *         t
 *    d = z * (Sm*Sm) * z
 *
 * According to [3], f(λ) must be implemented as in the following for 
 * numerical stability.
 * 
 *   f(λ) = (λ^2 -a)(λ^2 -b) - cλ + (cσ - d)
 * 
 * and 
 *
 *   f'(λ) = 2λ(2λ^2 - (a+b)) - c
 *
 * The newton-raphson process is formulated as:
 *
 *     λ_{n+1} = λ_n - f(λ_n)/f'(λ_n)
 *
 *    λ_0 = 1.0
 *
 *
 * It is ilconditioned when sqrt(γ^2 + |x|^2) approaches 0.
 * [2] states the condition in which this occurs is when |z| = 0  and
 * as well as γ = 0.
 * [2] states γ = 0 means the rotation angle θ = π.
 *
 * |z| = 0 implies that B is symmetric matrix.
 * 
 * If |z| = 0, then g(q) is simplified to
 *
 *                               |  |
 *                               |Vq|
 *     g(q) = [ Vq^t | 0 ] * K * |  |
 *                               |--|
 *                               | 0|
 * where
 * 
 *         |       | |
 *         |Sm - σI|0|
 *     K = |       | |
 *         |-------+-|
 *         |   0   |σ|
 *
 * and eigen value and the vector are as follows:
 *    |       | | |  |    |  |
 *    |Sm - σI|0| |Vq|    |Vq|
 *    |       | | |  | = λ|  |
 *    |-------+-| |--|    |--|
 *    |   0   |σ| |sq|    |sq|
 *
 * This is decomposed to two equations:
 *
 *    |       | |  |    |  |
 *    |Sm - σI| |Vq| = λ|Vq|
 *    |       | |  |    |  |
 *   
 *    σ*sq = λ*sq
 *
 * θ = π implised sq = 0, and the second equation vanishes.
 * The first equation can be solved by symmetric 3x3 eigen vector finder for
 * the specific eigen value λ.
 *  
 *  @references
 *   [1] Averaging Quaternions", 
 *       F. Landis Markley, Yang Cheng, John Lucas Crassidis, & Yaakov Oshman. 
 *       Journal of Guidance, Control, and Dynamics
 *       Vol. 30, No. 4 (2007), pp. 1193-1197.
 *
 *   [2] Approximate Algorithms for Fast Optimal Attitude Computation
 *       Malcolm D. Shuster, Guidance and Control Conference, 1978-1249
 *       American Institute of Aeronautics and Astronautics
 *
 *   [3] Improvement to the Implementation of the QUEST Algorithm
 *       Yang Cheng & Malcolm D. Shuster
 *       Journal of Guidance, Control, and Dynamics, Vol. 37, No. 1 (2014),
 *       pp. 301-305. 
 *       American Institute of Aeronautics and Astronautics
 *
 */
static const double AVG_EPSILON   = 0.00000000000001;
static const long   MAX_ITERATION = 100;

Quaternion Quaternion::average(
    vector<Quaternion>& quats,
    vector<double>&     weights
) {

    Mat3x3 B;

    for (size_t i = 0; i < quats.size(); i++) {
        auto R = quats[i].rotationMatrix();
        R.scale(weights[i]/3.0);
        B += R;

    }

    Mat3x3 Bt = B.transpose();

    double sigma = B.trace();

    Mat3x3 Sm = B + Bt;

    Vec3   z ( B.cell(2,3)-B.cell(3,2),
               B.cell(3,1)-B.cell(1,3),
               B.cell(1,2)-B.cell(2,1) );

    if (z.squaredNorm2() < AVG_EPSILON) {

        // z is a null vector.
        Mat3x3 sigmaI( sigma,   0.0,   0.0,
                         0.0, sigma,   0.0,
                         0.0,   0.0, sigma );

        Mat3x3 M(Sm - sigmaI);

        Vec3   eigenValues;

        Mat3x3 Evec = M.EigenVectorsIfSymmetric(eigenValues);

        Quaternion q_opt(0.0, Evec.cell(1,1), Evec.cell(2,1), Evec.cell(3,1));
           
        q_opt.normalize();

        Quaternion q_pi(0.0, Evec.cell(1,1), Evec.cell(2,1), Evec.cell(3,1));
        q_pi.normalize();

        return q_opt * q_pi;
    }

    double delta = Sm.det();
    
    double kappa = ( Sm.cell(2,2) * Sm.cell(3,3) +
                     Sm.cell(1,1) * Sm.cell(3,3) +
                     Sm.cell(1,1) * Sm.cell(2,2)  ) -
                   ( Sm.cell(2,3) * Sm.cell(3,2) +
                     Sm.cell(1,3) * Sm.cell(3,1) +
                     Sm.cell(1,2) * Sm.cell(2,1)  ); // Tr[adj(S)]

    double a = sigma * sigma - kappa;

    double b = sigma * sigma + z.squaredNorm2();

    double c = delta + z.dot(Sm * z);

    double d = z.dot( (Sm * Sm) * z );

    // Find lambda with Newton-Raphson.

    double lambda = 1.0;

    for (size_t iter = 0; iter < MAX_ITERATION; iter++) {

        double lambda2 = lambda*lambda;
        double f       = (lambda2 - a)*(lambda2 - b) - c * (lambda-sigma) - d;
        double f_dash  = 2.0 * lambda * ( 2.0 * lambda2 - (a + b)) - c;

        if (fabs(f_dash) < AVG_EPSILON) {
            break;
        }

        double nr_delta = f / f_dash;

        if (fabs(nr_delta) < AVG_EPSILON) {
            break;
        }
       
        lambda -= nr_delta;

    }

    // Get the quaternion
    double alpha = lambda*lambda - sigma*sigma + kappa;

    double beta  = lambda - sigma;

    double gamma = -1.0 * ((lambda + sigma) * alpha - delta);

    Mat3x3 alphaI( alpha,   0.0,   0.0, 
                     0.0, alpha,   0.0, 
                     0.0,   0.0, alpha );

    Mat3x3 betaS(Sm);
    betaS.scale(beta);

    Vec3 x( (alphaI + betaS + (Sm * Sm))*z );

    Quaternion q_opt(gamma, x);

    q_opt.normalize();

    Quaternion q_pi(0, x);
    q_pi.normalize();

    return q_opt;

}

}// namespace Makena

