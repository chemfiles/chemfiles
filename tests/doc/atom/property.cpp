// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>
#include <chemfiles.hpp>
using namespace chemfiles;

#undef assert
#define assert CHECK

TEST_CASE() {
    // [example]
    auto atom = Atom("C");

    atom.set("first", "this is a carbon");
    atom.set("second", 42.5);

    assert(atom.get("second")->as_double() == 42.5);
    assert(atom.get("first")->as_string() == "this is a carbon");

    assert(!atom.get("non-existant property"));

    // Override the "first" property
    atom.set("first", false);
    assert(atom.get("first")->as_bool() == false);

    // Iterate over all properties
    atom.set("third", "string");
    atom.set("fourth", Vector3D(1., 2., 3.));
    for(auto prop_ite = atom.properties_begin(); prop_ite != atom.properties_end() ; ++prop_ite) {
        auto kind = prop_ite->second.get_kind();
        switch(kind) {
            case Property::BOOL:
                assert(prop_ite->second.as_bool() == false);
                break;
            case Property::DOUBLE:
                assert(prop_ite->second.as_double() == 42.5);
                break;
            case Property::STRING:
                assert(prop_ite->second.as_string() == "string");
                break;
            case Property::VECTOR3D:
                assert(prop_ite->second.as_vector3d() == Vector3D(1., 2., 3.));
                break;
        }
    }
    // [example]
}
