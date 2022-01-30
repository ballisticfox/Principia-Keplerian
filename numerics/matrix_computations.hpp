﻿
#pragma once

#include "quantities/named_quantities.hpp"

namespace principia {
namespace numerics {
namespace internal_matrix_computations {

using quantities::Quotient;
using quantities::SquareRoot;

template<typename U>
struct CholeskyDecompositionGenerator;

template<typename M, typename L, typename U>
struct LUDecompositionGenerator;

template<typename V, typename U>
struct ᵗRDRDecompositionGenerator;

template<typename M, typename V>
struct SubstitutionGenerator;

template<typename M, typename V>
struct RayleighQuotientGenerator;

// If A is the upper half of a symmetric positive definite matrix, returns R so
// that A = ᵗR R.
template<typename UpperTriangularMatrix>
typename CholeskyDecompositionGenerator<UpperTriangularMatrix>::Result
CholeskyDecomposition(UpperTriangularMatrix const& A);

template<typename Matrix,
         typename LowerTriangularMatrix,
         typename UpperTriangularMatrix>
typename LUDecompositionGenerator<Matrix,
                                  LowerTriangularMatrix,
                                  UpperTriangularMatrix>::Result
LUDecomposition(Matrix const& A);

// If A is the upper half of a symmetric matrix, returns R and D so that
// A = ᵗR D R.  The diagonal matrix is represented as a vector.
template<typename Vector, typename UpperTriangularMatrix>
typename ᵗRDRDecompositionGenerator<Vector, UpperTriangularMatrix>::Result
ᵗRDRDecomposition(UpperTriangularMatrix const& A);

// Returns x such that U x = b.
template<typename UpperTriangularMatrix, typename Vector>
typename SubstitutionGenerator<UpperTriangularMatrix, Vector>::Result
BackSubstitution(UpperTriangularMatrix const& U,
                 Vector const& b);

// Returns x such that L x = b.
template<typename LowerTriangularMatrix, typename Vector>
typename SubstitutionGenerator<LowerTriangularMatrix, Vector>::Result
ForwardSubstitution(LowerTriangularMatrix const& L,
                    Vector const& b);

// Returns the Rayleigh quotient r(x) = ᵗx A x / ᵗx x.
template<typename Matrix, typename Vector>
typename RayleighQuotientGenerator<Matrix, Vector>::Result
RayleighQuotient(Matrix const& A, Vector const& x);

}  // namespace internal_matrix_computations

using internal_matrix_computations::BackSubstitution;
using internal_matrix_computations::CholeskyDecomposition;
using internal_matrix_computations::ForwardSubstitution;
using internal_matrix_computations::LUDecomposition;
using internal_matrix_computations::RayleighQuotient;
using internal_matrix_computations::ᵗRDRDecomposition;

}  // namespace numerics
}  // namespace principia

#include "numerics/matrix_computations_body.hpp"
