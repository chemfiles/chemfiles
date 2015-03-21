#include "Chemharp.hpp"
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
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


BOOST_PYTHON_MODULE(chemharp){
    // Removing this line will result in bad stuff appening, like segfaults and
    // your grand mother being kidnaped by aliens. So don't do this!
    np::initialize();

    py::class_<std::vector<size_t>>("Test")
            .def(py::vector_indexing_suite<std::vector<size_t>>() );

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
        .def("__len__", &Frame::natoms)
        .add_property("natoms", &Frame::natoms)
        .add_property("topology",
            py::make_function(
                static_cast<const Topology& (Frame::*)(void) const>(&Frame::topology),
                py::return_value_policy<py::copy_const_reference>()),
            static_cast<void (Frame::*)(const Topology&)>(&Frame::topology))
        .add_property("cell",
            py::make_function(
                static_cast<const UnitCell& (Frame::*)(void) const>(&Frame::cell),
                py::return_value_policy<py::copy_const_reference>()),
            static_cast<void (Frame::*)(const UnitCell&)>(&Frame::cell))
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
        .def("atom_list",
            static_cast<const vector<size_t>& (Topology::*)(void) const>(&Topology::atom_list),
            py::return_value_policy<std_vector_convertor>())
        .def("atom_types",
            static_cast<const vector<Atom>& (Topology::*)(void) const>(&Topology::atom_types),
            py::return_value_policy<std_vector_convertor>())
        .def("append", &Topology::append)
        .def("add_bond", &Topology::add_bond)
        .def("__len__", &Topology::natoms)
        .add_property("natoms", &Topology::natoms)
        .def("clear", &Topology::clear)
        .def("atom", py::make_function(
            static_cast<const Atom& (Topology::*)(size_t) const>(&Topology::atom),
            py::return_value_policy<py::copy_const_reference>()))
        .def("reserve", &Topology::reserve)
/*      TODO
        vector<bond> bonds(void);
        vector<angle> angles(void);
        vector<dihedral> dihedrals(void);
*/
    ;

    py::class_<UnitCell>("UnitCell", py::init<>())
        .def(py::init<double>())
        .def(py::init<double, double, double>())
        .def(py::init<double, double, double, double, double, double>())
        // TODO Wrap CellType enum
        // TODO Constructors with cell type
        // TODO Matrix3D matricial() const;
        .add_property("type",
            py::make_function(
                static_cast<const UnitCell::CellType& (UnitCell::*)(void) const>(&UnitCell::type),
                py::return_value_policy<py::copy_const_reference>()),
            static_cast<void (UnitCell::*)(UnitCell::CellType)>(&UnitCell::type))
        .add_property("a",
            static_cast<double (UnitCell::*)(void) const>(&UnitCell::a),
            static_cast<void (UnitCell::*)(double)>(&UnitCell::a))
        .add_property("b",
            static_cast<double (UnitCell::*)(void) const>(&UnitCell::b),
            static_cast<void (UnitCell::*)(double)>(&UnitCell::b))
        .add_property("c",
            static_cast<double (UnitCell::*)(void) const>(&UnitCell::c),
            static_cast<void (UnitCell::*)(double)>(&UnitCell::c))
        .add_property("alpha",
            static_cast<double (UnitCell::*)(void) const>(&UnitCell::alpha),
            static_cast<void (UnitCell::*)(double)>(&UnitCell::alpha))
        .add_property("beta",
            static_cast<double (UnitCell::*)(void) const>(&UnitCell::beta),
            static_cast<void (UnitCell::*)(double)>(&UnitCell::beta))
        .add_property("gamma",
            static_cast<double (UnitCell::*)(void) const>(&UnitCell::gamma),
            static_cast<void (UnitCell::*)(double)>(&UnitCell::gamma))
        .add_property("periodic_x",
            static_cast<bool (UnitCell::*)(void) const>(&UnitCell::periodic_x),
            static_cast<void (UnitCell::*)(bool)>(&UnitCell::periodic_x))
        .add_property("periodic_y",
            static_cast<bool (UnitCell::*)(void) const>(&UnitCell::periodic_y),
            static_cast<void (UnitCell::*)(bool)>(&UnitCell::periodic_y))
        .add_property("periodic_z",
            static_cast<bool (UnitCell::*)(void) const>(&UnitCell::periodic_z),
            static_cast<void (UnitCell::*)(bool)>(&UnitCell::periodic_z))
        .add_property("full_periodic",
            static_cast<bool (UnitCell::*)(void) const>(&UnitCell::full_periodic),
            static_cast<void (UnitCell::*)(bool)>(&UnitCell::full_periodic))
    ;

    // TODO: Logger class

}
