/* ----------------------------------------------------------------------- *//**
 *
 * @file LinearRegression_impl.hpp
 *
 *//* ----------------------------------------------------------------------- */

#ifndef MADLIB_MODULES_CENTRALITY_DEGREE_CENTRALITY_IMPL_HPP
#define MADLIB_MODULES_CENTRALITY_DEGREE_CENTRALITY_IMPL_HPP

// #include <boost/math/distributions.hpp>
// #include <modules/prob/student.hpp>

namespace madlib {

namespace modules {

namespace centrality {

template <class Container>
inline CentralityAccumulator<Container>::CentralityAccumulator(
    Init_type& inInitialization)
  : Base(inInitialization) {
    
    this->initialize();
}

/**
 * @brief Bind all elements of the state to the data in the stream
 *
 * The bind() is special in that even after running operator>>() on an element,
 * there is no guarantee yet that the element can indeed be accessed. It is
 * cruicial to first check this.
 *
 * Provided that this methods correctly lists all member variables, all other
 * methods can, however, rely on that fact that all variables are correctly
 * initialized and accessible.
 */
template <class Container>
inline
void
CentralityAccumulator<Container>::bind(ByteStream_type& inStream) {
    inStream >> numRows;
    inStream >> data.rebind(numRows);
}

/**
 * @brief Update the accumulation state
 *
 * We update the number of rows \f$ n \f$, the partial
 * sums \f$ \sum_{i=1}^n y_i \f$ and \f$ \sum_{i=1}^n y_i^2 \f$, the matrix
 * \f$ X^T X \f$, and the vector \f$ X^T \boldsymbol y \f$.
 */
template <class Container>
inline
CentralityAccumulator<Container>&
CentralityAccumulator<Container>::operator<<(const tuple_type& inTuple) {
    const int& x = std::get<0>(inTuple);
    const int& y = std::get<1>(inTuple);

    // The following checks were introduced with MADLIB-138. It still seems
    // useful to have clear error messages in case of infinite input values.
    if (!std::isfinite(y))
        throw std::domain_error("Dependent variables are not finite.");
    else if (!std::isfinite(x))
        throw std::domain_error("Design matrix is not finite.");

    // Initialize in first iteration
    if (numRows == 0) {
        this->resize();
    }

    numRows+=2;
    data << x;
    data << y;
    return *this;
}

/**
 * @brief Merge with another accumulation state
 */
template <class Container>
template <class OtherContainer>
inline
CentralityAccumulator<Container>&
CentralityAccumulator<Container>::operator<<(
    const CentralityAccumulator<OtherContainer>& inOther) {

    // Initialize if necessary
    if (numRows == 0) {
        *this = inOther;
        return *this;
    } else if (inOther.numRows == 0)
        return *this;

    numRows += inOther.numRows;
    data.noalias() += inOther.data;
    return *this;
}

template <class Container>
template <class OtherContainer>
inline
CentralityAccumulator<Container>&
CentralityAccumulator<Container>::operator=(
    const CentralityAccumulator<OtherContainer>& inOther) {

    this->copy(inOther);
    return *this;
}


template <class Container>
DegreeCentrality::DegreeCentrality(
    const CentralityAccumulator<Container>& inState) {

    compute(inState);
}

/**
 * @brief Transform a linear-regression accumulation state into a result
 *
 * The result of the accumulation phase is \f$ X^T X \f$ and
 * \f$ X^T \boldsymbol y \f$. We first compute the pseudo-inverse, then the
 * regression coefficients, the model statistics, etc.
 *
 * @sa For the mathematical description, see \ref grp_linreg.
 */
template <class Container>
inline
DegreeCentrality&
DegreeCentrality::compute(
    const CentralityAccumulator<Container>& inState) {

    Allocator& allocator = defaultAllocator();

    // The following checks were introduced with MADLIB-138. It still seems
    // useful to have clear error messages in case of infinite input values.
    // if (!isfinite(inState.data))
    //     throw std::domain_error("Design matrix is not finite.");

    int len = 0;
    for (unsigned int i = 0; i < inState.numRows; i+=2) {
        int v1 = inState.data(i);
        int v2 = inState.data(i+1);
        int found1 = 0, found2 = 0;
        for (int j = 0; j < len; j++) {
            if (vertex(j) == v1) {
                degree(j) += 1;
                found1 = 1;
            }
            if (vertex(j) == v2) {
                degree(j) += 1;
                found2 = 1;
            }
            if (found1 && found2) break;
        }
        if (! found1) {
            vertex << v1;
            degree << 1;
        }
        if (! found2) {
            vertex << v2;
            degree << 1;
        }
    }
    return *this;
}

} // namespace regress

} // namespace modules

} // namespace madlib

#endif // defined(MADLIB_MODULES_CENTRALITY_DEGREE_CENTRALITY_IMPL_HPP)
