/* ----------------------------------------------------------------------- *//**
 *
 * @file linear.cpp
 *
 * @brief Linear-regression functions
 *
 *//* ----------------------------------------------------------------------- */

#include <dbconnector/dbconnector.hpp>

#include "DegreeCentrality_proto.hpp"
#include "DegreeCentrality_impl.hpp"
#include "degree.hpp"

namespace madlib {

namespace modules {

namespace centrality {
/////////////////////////////////////////////////////
// class DegreeStatus {
// private:
//     int vertex;
//     int degree;
// public:
//     DegreeStatus(int);
//     DegreeStatus(int,int);
//     int getVertex();
//     int getDegree();
//     void incrementDegree();
//     void incrementDegree(int);
// };

// DegreeStatus::DegreeStatus(int vertex) {
//     this->vertex = vertex;
//     this->degree = 1;
// }

// DegreeStatus::DegreeStatus(int vertex, int degree) {
//     this->vertex = vertex;
//     this->degree = degree;
// }

// int DegreeStatus::getVertex() {
//     return this->vertex;
// }

// int DegreeStatus::getDegree() {
//     return this->degree;
// }

// void DegreeStatus::incrementDegree() {
//     this->degree++;
// }

// void DegreeStatus::incrementDegree(int inc) {
//     this->degree += inc;
// }
//////////////////////////////////////////////////////////

typedef CentralityAccumulator<RootContainer> DegreeState;
typedef CentralityAccumulator<MutableRootContainer> MutableDegreeState;

AnyType
degcent_transition::run(AnyType& args) {
    elog(INFO, "transition info1");
    MutableDegreeState state = args[0].getAs<MutableByteString>();
    elog(INFO, "transition info2");

    // char* statusStr = args[0].getAs<char*>();
    // std::vector<DegreeStatus> status;
    // char *p = strtok(statusStr, ",");
    // while (p) {
    //     //printf ("Token: %s\n", p);
    //     int v = atoi(p);
    //     p = strtok(NULL, ",");
    //     int d = atoi(p);
    //     DegreeStatus st(v,d);
    //     status.push_back(st);
    // }


    // int v1 = args[1].getAs<int>();
    // int v2 = args[2].getAs<int>();

    // state << MutableDegreeState::tuple_type(v1, v2);
    return state.storage();

    //update value for vertices
    // uint16_t i = 0;
    // uint16_t len = state.numFields();
    // int found1 = 0, found2 = 0;
    // for (std::vector<DegreeStatus>::iterator it = status.begin(); 
    //     it != status.end(); ++it) {
    //     DegreeStatus st = *it;
    //     if (st.getVertex() == v1) {
    //         st.incrementDegree();
    //         found1 = 1;
    //         if (found2) break;
    //     }
    //     if (st.getVertex() == v2) {
    //         st.incrementDegree();
    //         found2 = 1;
    //         if (found1) break;
    //     }
    // }
    // if (! found1) {
    //     DegreeStatus tuple(v1);
    //     status.push_back(tuple);
    // }
    // if (! found2) {
    //     DegreeStatus tuple(v2);
    //     status.push_back(tuple);
    // }
    // for (i = 0; i < len; i++) {
    //     if (state[i][0].getAs<int>() == v1) {
    //         state[i][1] = state[i][1].getAs<int>()+1;
    //         found1 = 1;
    //         if (found2) break;
    //     }
    //     if (state[i][0].getAs<int>() == v2) {
    //         state[i][1] = state[i][1].getAs<int>()+1;
    //         found2 = 1;
    //         if (found1) break;
    //     }
    // }
    // if (! found1) {
    //     AnyType tuple;
    //     tuple << v1 << 1;
    //     state << tuple;
    // }
    // if (! found2) {
    //     AnyType tuple;
    //     tuple << v2 << 1;
    //     state << tuple;
    // }
    // std::string result;
    // for (std::vector<DegreeStatus>::iterator it = status.begin(); 
    //     it != status.end(); ++it) {
    //     DegreeStatus st = *it;
    //     char vstr[21]; // enough to hold all numbers up to 64-bits
    //     sprintf(vstr, "%d", st.getVertex());
    //     char dstr[21]; // enough to hold all numbers up to 64-bits
    //     sprintf(dstr, "%d", st.getDegree());
    //     result += vstr; 
    //     result += ",";
    //     result += dstr;
    //     result += ",";
    // }
    // char * cstr = new char [result.length()+1];
    // std::strcpy (cstr, result.c_str());
    // AnyType res(cstr);
    // return res;
}

AnyType
degcent_merge_states::run(AnyType& args) {
    // char* stateLeft = args[0].getAs<char*>();
    // char* stateRight = args[1].getAs<char*>();
    elog(INFO, "merge info1");
    MutableDegreeState stateLeft = args[0].getAs<MutableByteString>();
    DegreeState stateRight = args[1].getAs<ByteString>();

    stateLeft << stateRight;
    return stateLeft.storage();

    // std::vector<DegreeStatus> statusL;
    // char *p = strtok(stateLeft, ",");
    // while (p) {
    //     //printf ("Token: %s\n", p);
    //     int v = atoi(p);
    //     p = strtok(NULL, ",");
    //     int d = atoi(p);
    //     DegreeStatus st(v,d);
    //     statusL.push_back(st);
    // }
    // std::vector<DegreeStatus> statusR;
    // p = strtok(stateRight, ",");
    // while (p) {
    //     //printf ("Token: %s\n", p);
    //     int v = atoi(p);
    //     p = strtok(NULL, ",");
    //     int d = atoi(p);
    //     DegreeStatus st(v,d);
    //     statusR.push_back(st);
    // }

    // int found = 0;
    // for (std::vector<DegreeStatus>::iterator itR = statusR.begin(); 
    //     itR != statusR.end(); ++itR) {
    //     DegreeStatus stR = *itR;
    //     found = 0;
    //     for (std::vector<DegreeStatus>::iterator itL = statusL.begin(); 
    //     itL != statusL.end(); ++itL) {
    //         DegreeStatus stL = *itL;
    //         if (stL.getVertex() == stR.getVertex()) {
    //             stL.incrementDegree(stR.getDegree());
    //             found = 1;
    //             break;
    //         }
    //     }
    //     if (! found) {
    //         DegreeStatus tuple(stR.getVertex(), stR.getDegree());
    //         statusL.push_back(tuple);        
    //     }
    // }

    // uint16_t lenR = stateRight.numFields(),lenL = stateLeft.numFields(),i,j, found;
    // for (i = 0; i < lenR; i++) {
    //     int v = stateRight[i][0].getAs<int>();
    //     found = 0;
    //     for (j = 0; j < lenL; j++) {
    //         if (stateLeft[j][0].getAs<int>() == v) {
    //             stateLeft[j][1] = stateLeft[j][1].getAs<int>() + stateRight[i][1].getAs<int>();
    //             found = 1;
    //             break;
    //         }
    //     }
    //     if (! found) {
    //         stateLeft << stateRight[i];
    //     }
    // }


    // std::string result;
    // for (std::vector<DegreeStatus>::iterator it = statusL.begin(); 
    //     it != statusL.end(); ++it) {
    //     DegreeStatus st = *it;
    //     char vstr[21]; // enough to hold all numbers up to 64-bits
    //     sprintf(vstr, "%d", st.getVertex());
    //     char dstr[21]; // enough to hold all numbers up to 64-bits
    //     sprintf(dstr, "%d", st.getDegree());
    //     result += vstr; 
    //     result += ",";
    //     result += dstr;
    //     result += ",";
    // }
    // char * cstr = new char [result.length()+1];
    // std::strcpy (cstr, result.c_str());
    // return cstr;
}

AnyType
degcent_final::run(AnyType& args) {
    // char* state = args[0].getAs<char*>();
    // return state;
    DegreeState state = args[0].getAs<ByteString>();

    // If we haven't seen any data, just return Null. This is the standard
    // behavior of aggregate function on empty data sets (compare, e.g.,
    // how PostgreSQL handles sum or avg on empty inputs)
    if (state.numRows == 0)
        return Null();

    AnyType tuple;
    DegreeCentrality result(state);
    for (unsigned int i = 0; i < state.numRows; i++) {
        tuple << result.vertex(i) << result.degree(i);
    }
    return tuple;

    // AnyType tuple;

    // for (iter = stateRight.begin(); iter != stateRight.end(); iter++) {
    //     tuple << iter.first << iter.second;
    // }

    // LinearRegression result(state);
    // tuple << result.coef << result.r2 << result.stdErr << result.tStats
    //     << (state.numRows > state.widthOfX
    //         ? result.pValues
    //         : Null())
    //     << result.conditionNo;
    // return tuple;
    //return state;
}

} // namespace regress

} // namespace modules

} // namespace madlib
