#include "Chemharp.hpp"
#include <boost/python.hpp>
#include <boost/numpy.hpp>

namespace py = boost::python;
namespace np = boost::numpy;
using namespace harp;

void translate_Error(Error const& e) {
    auto err = std::string("Chemharp error: ") + e.what();
    PyErr_SetString(PyExc_UserWarning, err.c_str());
}

void translate_FileError(FileError const& e) {
    auto err = std::string("Chemharp file error: ") + e.what();
    PyErr_SetString(PyExc_UserWarning, err.c_str());
}

void translate_MemoryError(MemoryError const& e) {
    auto err = std::string("Chemharp memory error: ") + e.what();
    PyErr_SetString(PyExc_UserWarning, err.c_str());
}

void translate_FormatError(FormatError const& e) {
    auto err = std::string("Chemharp format error: ") + e.what();
    PyErr_SetString(PyExc_UserWarning, err.c_str());
}

// ResultConverterGenerator used to transform Array3D to numpy ndarray.
struct Array3D_convertor {
    template <class T> struct apply {
        struct type {
            // Convert Array3D to ndarray.
            PyObject* operator()(const Array3D& A) const {
                py::tuple shape = py::make_tuple(3, A.size());
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

BOOST_PYTHON_MODULE(_chemharp){
    // Removing this line will result in bad stuff appening, like segfaults and
    // your grand mother being kidnaped by aliens. So don't do this!
    np::initialize();

    /* Exception management ***************************************************/
    py::register_exception_translator<Error>(&translate_Error);
    py::register_exception_translator<FileError>(&translate_FileError);
    py::register_exception_translator<MemoryError>(&translate_MemoryError);
    py::register_exception_translator<FormatError>(&translate_FormatError);

    /* Trajectory class *******************************************************/
    py::class_<Trajectory, boost::noncopyable>("Trajectory",
            py::init<std::string, py::optional<std::string, std::string>>())
        .def("read_next_step", &Trajectory::read_next_step,
                                    py::return_internal_reference<1,
                                        py::with_custodian_and_ward_postcall<0, 1> >())
        .def("read_at_step", &Trajectory::read_at_step,
                                    py::return_internal_reference<1,
                                        py::with_custodian_and_ward_postcall<0, 1> >())
        .def("write_step", &Trajectory::write_step)
    ;

    /* Frame class ************************************************************/
    py::class_<Frame>("Frame")
        .def("positions",
            static_cast<const Array3D& (Frame::*)(void) const>(&Frame::positions),
            py::return_value_policy<Array3D_convertor>())
        .def("velocities",
            static_cast<const Array3D& (Frame::*)(void) const>(&Frame::velocities),
            py::return_value_policy<Array3D_convertor>())
        .def("has_velocities", &Frame::has_velocities)
        .def("natoms", &Frame::natoms)
        .def("topology",
            static_cast<const Topology& (Frame::*)(void) const>(&Frame::topology),
            py::return_value_policy<py::copy_const_reference>())
        .def("cell",
            static_cast<const UnitCell& (Frame::*)(void) const>(&Frame::cell),
            py::return_value_policy<py::copy_const_reference>())
        .add_property("step",
                static_cast<size_t (Frame::*)(void) const>(&Frame::step),
                static_cast<void (Frame::*)(size_t)>(&Frame::step))
    ;

    /* Atom class *************************************************************/
    py::class_<Atom>("Atom", py::init<std::string>())
        .add_property("name",
                py::make_function(
                    static_cast<const string& (Atom::*)(void) const>(&Atom::name),
                    py::return_value_policy<py::copy_const_reference>()),
                static_cast<void (Atom::*)(const string&)>(&Atom::name))
        .add_property("mass",
                py::make_function(
                    static_cast<const float& (Atom::*)(void) const>(&Atom::mass),
                    py::return_value_policy<py::copy_const_reference>()),
                static_cast<void (Atom::*)(float)>(&Atom::mass))
        .add_property("charge",
                py::make_function(
                    static_cast<const float& (Atom::*)(void) const>(&Atom::charge),
                    py::return_value_policy<py::copy_const_reference>()),
                static_cast<void (Atom::*)(float)>(&Atom::charge))
        // TODO: wrap the Atom::AtomType enum to Python
        .add_property("type",
                py::make_function(
                    static_cast<const Atom::AtomType& (Atom::*)(void) const>(&Atom::type),
                    py::return_value_policy<py::copy_const_reference>()),
                static_cast<void (Atom::*)(Atom::AtomType)>(&Atom::type))
    ;


    py::class_<Topology>("Topology")
        // TODO: class members
    ;

    py::class_<UnitCell>("UnitCell")
        // TODO: class members
    ;

}
