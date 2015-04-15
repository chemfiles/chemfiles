/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
*/

#include "Chemharp.hpp"
#include <boost/python.hpp>
#include <boost/numpy.hpp>
namespace py = boost::python;
namespace np = boost::numpy;
using namespace harp;
using std::string;

#ifndef CHRP_PYTHON_CONVERTORS_HPP
#define CHRP_PYTHON_CONVERTORS_HPP

// ResultConverterGenerator used to transform Array3D to numpy ndarray.
struct Array3D_convertor {
    template <class T> struct apply {
        struct type {
            // Convert Array3D to ndarray.
            PyObject* operator()(const Array3D& A) const {
                py::tuple shape = py::make_tuple(A.size(), 3);
                np::dtype dtype = np::dtype::get_builtin<float>();
                np::ndarray res = np::empty(shape, dtype);

                auto c_arr = reinterpret_cast<float (*)[3]>(res.get_data());
                for (size_t i=0; i<A.size(); i++)
                    for (size_t j=0; j<3; j++)
                        c_arr[i][j] = A[i][j];

                return py::incref(res.ptr());
            }

            // Used for documentation.
            const PyTypeObject* get_pytype() const { return 0; }
        };
    };
};

// ResultConverterGenerator used to transform Matrix3D to numpy ndarray.
struct Matrix3D_convertor {
    template <class T> struct apply {
        struct type {
            // Convert Matrix3D to ndarray.
            PyObject* operator()(const Matrix3D& A) const {
                py::tuple shape = py::make_tuple(3, 3);
                np::dtype dtype = np::dtype::get_builtin<double>();
                np::ndarray res = np::empty(shape, dtype);

                auto c_arr = reinterpret_cast<double (*)[3]>(res.get_data());
                for (size_t i=0; i<3; i++)
                    for (size_t j=0; j<3; j++)
                        c_arr[i][j] = A[i][j];

                return py::incref(res.ptr());
            }

            // Used for documentation.
            const PyTypeObject* get_pytype() const { return 0; }
        };
    };
};

struct std_vector_convertor {
    template <class T> struct apply {
        struct type {
            // Convert any std::vector to python list.
            template <class S>
            PyObject* operator()(const std::vector<S>& A) const {
                py::list res;
                for (auto val : A)
                    res.append(val);
                return py::incref(res.ptr());
            }

            // Used for documentation.
            const PyTypeObject* get_pytype() const { return 0; }
        };
    };
};

#endif
