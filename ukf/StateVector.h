/*
Copyright (C) 2016 Thiemar Pty Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef STATEVECTOR_H
#define STATEVECTOR_H

#include <tuple>
#include <cstddef>
#include <utility>
#include <Eigen/Core>
#include "Config.h"

namespace UKF {

    namespace detail {

    /*
    This variable template defines the number of dimensions of a particular
    StateVector field. For a normal Eigen column vector, the default
    implementation returns the number of rows at compile time.

    Template specialisations can be used to define the size of fields which
    don't have a MaxRowsAtCompileTime member, or need a custom size for some
    reason (eg. the Eigen Quaternion classes).
    */
    template <typename Field>
    constexpr std::size_t SigmaPointDimension = Field::MaxRowsAtCompileTime;

    template <>
    constexpr std::size_t SigmaPointDimension<Eigen::Quaternionf> = 4;

    template <>
    constexpr std::size_t SigmaPointDimension<Eigen::Quaterniond> = 4;

    template <>
    constexpr std::size_t SigmaPointDimension<float> = 1;

    template <>
    constexpr std::size_t SigmaPointDimension<double> = 1;

    template<typename T>
    constexpr T Adder(T v) {
        return v;
    }

    template<typename T, typename... Args>
    constexpr T Adder(T first, Args... args) {
        return first + Adder(args...);
    }

    /*
    Get the dimension of the state vector by summing the dimension of all
    fields.
    */
    template <typename... Fields>
    constexpr std::size_t GetStateVectorDimension(const std::tuple<Fields...>& t) {
        return Adder(SigmaPointDimension<Fields>...);
    }

    }

/*
Templated state vector class. A particular UKF implementation should
instantiate this class with a list of fields that make up the state vector.

By default, fields can be Eigen vectors (including Quaternions) or scalar
floating point types (float, double). Support for other classes can be added
by specialising the SigmaPointDimension variable for the desired class.
*/
template <typename IntegratorType, typename... Fields>
class StateVector {
private:
    static IntegratorType integrator;
    static std::tuple<Fields...> field_types;
    static constexpr std::size_t dimension = detail::GetStateVectorDimension(field_types);

    Eigen::Matrix<real_t, dimension, 1> state_vector;

public:
    static constexpr std::size_t GetDimension() { return dimension; }
};

}

#endif