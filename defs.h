#ifndef DEFS_H
#define DEFS_H
//#include <boost/shared_array.hpp>


#define M_VAL 1e16f
typedef double value_type;
//typedef boost::shared_array<value_type> array;
typedef value_type *array;
typedef size_t vertex_type;
const vertex_type School = 0;
const vertex_type Nil = 9000;
const value_type EPSILON = (value_type)0.0001;
#endif
