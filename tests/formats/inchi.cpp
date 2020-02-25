// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>

#include "catch.hpp"
#include "chemfiles.hpp"
#include "helpers.hpp"
using namespace chemfiles;

TEST_CASE("Read files in InChI format") {
    SECTION("Number of compounds") {
        Trajectory traj("data/inchi/rdkit.inchi");
        CHECK(traj.nsteps() == 4);

        // Don't die, like RDKit
        auto frame = traj.read();

        CHECK(frame.size() == 14);
        CHECK(frame.topology().bonds().size() == 15);
        CHECK(frame.topology().bond_orders()[0] == Bond::SINGLE);
        CHECK(frame.topology().bond_orders()[1] == Bond::DOUBLE);

        auto positions = frame.positions();
        CHECK(positions[0] == Vector3D{0.0, 0.0, 0.0});

        frame = traj.read();
        CHECK(frame.topology().bond_order(3, 4) == Bond::EVEN_RECTANGLE);

        traj.read();
        traj.read();
    }

    SECTION("Tetrahedral Chirality") {
        // https://www.inchi-trust.org/technical-faq-2/#8.6
        // Technically, this is the only standard way to do things
        char unknown_stereo[] = "InChI=1S/C6H12O/c1-5-3-4-7-6(5)2/h5-6H,3-4H2,1-2H3/t5?,6-/m0/s1\n";
        auto traj_u = Trajectory::memory_reader(unknown_stereo, 65, "InChI");
        auto frame_u = traj_u.read();

        CHECK(frame_u[4].get("chirality")->as_string() == "tetrahedron_unknown");
        CHECK(frame_u[5].get("chirality")->as_string() == "tetrahedron_odd");
    }

    SECTION("Rectangular Chirality") {
        // https://pubchem.ncbi.nlm.nih.gov/compound/6324998 Note: It is missing the /b tag
        char cumulene[] = "InChI=1S/C10H10O2/c1-2-3-4-5-6-9-7-8-10(11)12-9/h4,7-8H,2-3H2,1H3/b9-4-\n";
        auto traj = Trajectory::memory_reader(cumulene, 72, "InChI");
        auto frame = traj.read();

        CHECK(frame.size() == 13);
        CHECK(frame.topology().bond_order(4, 5) == Bond::ODD_RECTANGLE);

        CHECK(*(frame[frame.size() - 1].atomic_number()) == 1);
    }

    SECTION("Anti-rectangular Chirality") {
        // https://pubchem.ncbi.nlm.nih.gov/compound/101094264 Note: It is missing the /t tag
        char allene[] = "InChI=1/C27H21OP/c28-29(25-17-9-3-10-18-25,26-19-11-4-12-20-26)27"
                        "(24-15-7-2-8-16-24)22-21-23-13-5-1-6-14-23/h1-21H/t22-/s2\n";
        auto traj = Trajectory::memory_reader(allene, 124, "InChI");
        auto frame = traj.read();

        CHECK(frame[21].get("chirality")->as_string() == "antirectangle_odd");

        // Note: the following generates a warning - this is not fixed on purpose to test
        // warning handling. The warning is a result of the /h tag
        char allene2[] = "InChI=1/C27H21OP/c28-29(25-17-9-3-10-18-25,26-19-11-4-12-20-26)27"
                        "(24-15-7-2-8-16-24)22-21-23-13-5-1-6-14-23/h1-21H/t22+/s2\n";
        auto traj2 = Trajectory::memory_reader(allene2, 124, "InChI");
        auto frame2 = traj2.read();

        CHECK(frame2[21].get("chirality")->as_string() == "antirectangle_even");

        char allene3[] = "InChI=1/C27H21OP/c28-29(25-17-9-3-10-18-25,26-19-11-4-12-20-26)27"
                        "(24-15-7-2-8-16-24)22-21-23-13-5-1-6-14-23/h1-21H/s2\n";
        auto traj3 = Trajectory::memory_reader(allene3, 119, "InChI");
        auto frame3 = traj3.read();

        CHECK(frame3[21].get("chirality") == nullopt);
    }
}

TEST_CASE("Write files in InChI format") {

    SECTION("Tetrahedral stereochemistry") {
        auto expected_result =
R"(InChI=1S/CH2ClF/c2-1-3/h1H2
AuxInfo=1/0/N:1,3,2/rA:3nCFCl/rB:s1;s1;/rC:;;;
InChI=1S/CHBrClF/c2-1(3)4/h1H/t1-/m0/s1
AuxInfo=1/0/N:1,4,3,2/it:im/rA:4nC.eFClBr/rB:s1;s1;s1;/rC:;;;;
InChI=1S/CBrClFI/c2-1(3,4)5/t1-/m0/s1
AuxInfo=1/0/N:1,4,3,2,5/it:im/rA:5nC.eFClBrI/rB:s1;s1;s1;s1;/rC:;;;;;
)";

        auto traj = Trajectory::memory_writer("InChI");

        Frame frame;
        frame.add_atom(Atom("C"), {0.0, 0.0, 0.0});
        frame.add_atom(Atom("F"), {0.0, 0.0, 0.0});
        frame.add_atom(Atom("Cl"), {0.0, 0.0, 0.0});

        frame.add_bond(0, 1);
        frame.add_bond(0, 2);

        // Generates a warning
        frame[0].set("chirality", "tetrahedron_even");
        traj.write(frame);

        frame.add_atom(Atom("Br"), {0.0, 0.0, 0.0});
        frame.add_bond(0, 3);
        traj.write(frame);

        frame.add_atom(Atom("I"), {0.0, 0.0, 0.0});
        frame.add_bond(0, 4);
        traj.write(frame);

        auto result = *(traj.memory_buffer());
        auto result_str = std::string(result.data(), result.size());

        CHECK(result_str == expected_result);
    }

    SECTION("Starting with 3D information - Odd stereochemistry") {
        Trajectory traj("data/inchi/github3_3d.mol2");
        auto frame = traj.read();
        
        auto tmpfile = NamedTempPath(".inchi");
        Trajectory out(tmpfile, 'w');

        out.write(frame);
        out.close();

        std::ifstream tmp1(tmpfile);
        std::string line;
        std::getline(tmp1, line);
        CHECK(line == "InChI=1S/C7H17NO5/c1-8-2-4(10)6(12)7(13)5(11)3-9/"
                      "h4-13H,2-3H2,1H3/t4-,5+,6+,7+/m0/s1");
        std::getline(tmp1, line);
        CHECK(line ==
"AuxInfo=1/0/N:7,2,11,3,6,4,5,1,12,8,13,9,10/it:im/rA:"
"30nNCCCCCCOOOCOOHHHHHHHHHHHHHHHHH/rB:s1;s2;s3;s4;s5;s1;s3;s4;s5;s6;s11;s6;s1;"
"s2;s2;s3;s4;s5;s6;s7;s7;s7;s8;s9;s10;s11;s11;s12;s13;/rC:-.1369,.0468,.1815;"
"1.3367,.0949,.0577;1.9205,-1.3437,.0435;3.4793,-1.4157,-.041;4.1081,-.7875,-"
"1.3294;5.67,-.7919,-1.358;-.804,1.3617,.1242;1.4868,-1.9999,1.2355;3.8589,-2."
"7918,.0467;3.6141,-1.4701,-2.4839;6.2688,-.0347,-2.5717;5.967,-.6886,-3.8012;"
"6.1969,-.1782,-.1809;-.6479,-.8168,.2962;1.5377,.5392,-.9172;1.6943,.5842,."
"9637;1.509,-1.8837,-.8092;3.9028,-.8955,.8181;3.764,.2434,-1.4119;5.9996,-1."
"8309,-1.3518;-.9863,1.721,1.137;-.1655,2.0704,-.4033;-1.7529,1.265,-.4033;1."
"5318,-2.9508,1.1118;3.9291,-3.0462,.9697;3.6736,-2.4182,-2.3449;5.7691,.9315,"
"-2.6415;7.3548,-.0821,-2.4917;5.9983,-1.6396,-3.674;6.4738,-.8579,.4378;"
        );

        Trajectory traj_in(tmpfile);
        auto frame_in = traj_in.read();
        CHECK(frame_in.size() == 17);
        CHECK(frame_in[3].get("chirality")->as_string() == "tetrahedron_odd");
        CHECK(frame_in[4].get("chirality")->as_string() == "tetrahedron_even");
        CHECK(frame_in[5].get("chirality")->as_string() == "tetrahedron_even");
        CHECK(frame_in[6].get("chirality")->as_string() == "tetrahedron_even");

        auto tmpfile2 = NamedTempPath(".inchi");
        Trajectory traj_out2(tmpfile2, 'w');
        traj_out2.write(frame_in);
        traj_out2.close();

        std::ifstream tmp2(tmpfile2);

        std::getline(tmp2, line);
        CHECK(line == "InChI=1S/C7H17NO5/c1-8-2-4(10)6(12)7(13)5(11)3-9/"
                      "h4-13H,2-3H2,1H3/t4-,5+,6+,7+/m0/s1");
    }

    SECTION("Starting with 3D information - Odd sulfur stereochemistry") {
        Trajectory traj("data/inchi/github296_3d.mol2");
        auto frame = traj.read();

        auto tmpfile = NamedTempPath(".inchi");
        Trajectory out(tmpfile, 'w');

        out.write(frame);
        out.close();

        std::ifstream tmp1(tmpfile);
        std::string line;
        std::getline(tmp1, line);
        CHECK(line == "InChI=1S/C5H12OS/c1-5(2,3)7(4)6/h1-4H3/t7-/m1/s1");
        std::getline(tmp1, line);
        CHECK(line ==
"AuxInfo=1/0/N:1,2,3,5,4,6,7/E:(1,2,3)/it:im/rA:19nCCCCCOSHHHHHHHHHHHH/rB:;;"
"s1s2s3;;;s4s5d6;s1;s1;s1;s2;s2;s2;s3;s3;s3;s5;s5;s5;/rC:2.9656,-.7748,.1474;."
"6001,-.4304,1.0573;1.0316,-.317,-1.4491;1.6231,-.0014,-.0394;2.623,2.1877,1."
"7026;3.0131,2.1824,-.9927;1.871,1.8254,.0741;3.1716,-.8935,1.2112;2.8872,-1."
"7568,-.3192;3.7758,-.2145,-.3192;1.0281,-.2509,2.0436;-.3155,.1507,.9474;."
"3728,-1.4908,.9474;.9743,-1.3969,-1.5856;.033,.1127,-1.5286;1.6743,.1127,-2."
"2174;2.8576,1.2523,2.2106;3.5377,2.7633,1.5605;1.9217,2.7633,2.3067;"
        );

        Trajectory traj_in(tmpfile);
        auto frame_in = traj_in.read();
        CHECK(frame_in.size() == frame.size() - 12);
        CHECK(frame_in[6].get("chirality")->as_string() == "tetrahedron_even"); // chirality is set for us via 3D

        auto tmpfile2 = NamedTempPath(".inchi");
        Trajectory traj_out2(tmpfile2, 'w');
        traj_out2.write(frame_in);

        frame_in[6].set("chirality", "tetrahedron_odd");
        traj_out2.write(frame_in);

        traj_out2.close();
        std::ifstream tmp2(tmpfile2);

        std::getline(tmp2, line);
        CHECK(line == "InChI=1S/C5H12OS/c1-5(2,3)7(4)6/h1-4H3/t7-/m1/s1");

        std::getline(tmp2, line);
        std::getline(tmp2, line);
        CHECK(line == "InChI=1S/C5H12OS/c1-5(2,3)7(4)6/h1-4H3/t7-/m0/s1");
    }
}
