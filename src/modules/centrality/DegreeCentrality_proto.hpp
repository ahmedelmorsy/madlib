/* ----------------------------------------------------------------------- *//**
 *
 * @file LinearRegression_proto.hpp
 *
 *//* ----------------------------------------------------------------------- */

#ifndef MADLIB_MODULES_CENTRALITY_DEGREE_CENTRALITY_PROTO_HPP
#define MADLIB_MODULES_CENTRALITY_DEGREE_CENTRALITY_PROTO_HPP

namespace madlib {

namespace modules {

namespace centrality {

// Use Eigen
using namespace dbal;
using namespace dbal::eigen_integration;

template <class Container>
class CentralityAccumulator
  : public DynamicStruct<CentralityAccumulator<Container>, Container> {
public:
    typedef DynamicStruct<CentralityAccumulator, Container> Base;
    MADLIB_DYNAMIC_STRUCT_TYPEDEFS;
    typedef std::tuple<int, int> tuple_type;

    CentralityAccumulator(Init_type& inInitialization);
    void bind(ByteStream_type& inStream);
    CentralityAccumulator& operator<<(const tuple_type& inTuple);
    template <class OtherContainer> CentralityAccumulator& operator<<(
        const CentralityAccumulator<OtherContainer>& inOther);
    template <class OtherContainer> CentralityAccumulator& operator=(
        const CentralityAccumulator<OtherContainer>& inOther);

    uint64_type numRows;
    ColumnVector_type data;
};

class DegreeCentrality {
public:
    template <class Container> DegreeCentrality(
        const CentralityAccumulator<Container>& inState);
    template <class Container> DegreeCentrality& compute(
        const CentralityAccumulator<Container>& inState);

    MutableNativeColumnVector vertex;
    MutableNativeColumnVector degree;
};

} // namespace regress

} // namespace modules

} // namespace madlib

#endif // defined(MADLIB_MODULES_CENTRALITY_DEGREE_CENTRALITY_PROTO_HPP)
