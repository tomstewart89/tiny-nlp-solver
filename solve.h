#include <sstream>

#include "lu_decompose.h"

namespace tiny_sqp_solver
{
template <int X, int P>
struct EqualityConstrainedQuadraticProblem
{
    Matrix<X, X> Q;
    Matrix<X> c;
    Matrix<X, P> A;
    Matrix<P> Ax_b;
};

template <int X, int P>
Matrix<X> solve(const EqualityConstrainedQuadraticProblem<X, P>& problem)
{
    Matrix<X + P, X + P> KKT;
    Matrix<X + P> kkt;

    kkt.template view<0, X, 0, 1>() = problem.c * -1.0;
    kkt.template view<X, X + P, 0, 1>() = problem.Ax_b * -1.0;

    KKT.template view<0, X, 0, X>() = problem.Q;
    KKT.template view<0, X, X, X + P>() = problem.A;
    KKT.template view<X, X + P, 0, X>() = problem.A.transpose();
    KKT.template view<X, X + P, X, X + P>() = Zeros<P, P>();

    auto decomp = lu_decompose(KKT);

    if (decomp.singular)
    {
        std::stringstream strm;
        strm << "KKT matrix was singular: KKT = " << KKT;
        throw std::runtime_error(strm.str());
    }

    return lu_solve(decomp, kkt).template view<0, X, 0, 1>();
}

}  // namespace tiny_sqp_solver
