// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

// !!!! AUTO-GENERATED FILE !!!! Do not edit. See elements.py for the code.
// The data comes from Blue Obelisk's data repository at the svn repository:
// http://svn.code.sf.net/p/bodr/code/trunk/bodr

#ifndef CHEMFILES_PERIODIC_HPP
#define CHEMFILES_PERIODIC_HPP

#include <map>
#include <string>
#include <cstdint>

namespace chemfiles {

//! Storing basic elemental data: mass, colvalent and Van der Waals radii
struct ElementData {
    //! Atomic number
    const uint64_t number;
    //! Full name
    const std::string name;
    //! Mass in atomic units
    const double mass;
    //! Covalent radius in Angstrom
    const double covalent_radius;
    //! Van der Waals radius in Angstrom
    const double vdw_radius;
};

static const std::map<std::string, ElementData> PERIODIC_INFORMATION = {
    {"Xx", ElementData{0, "Dummy", 0.0, 0.0, 0.0} },
    {"H", ElementData{1, "Hydrogen", 1.008, 0.37, 1.2} },
    {"He", ElementData{2, "Helium", 4.002602, 0.32, 1.4} },
    {"Li", ElementData{3, "Lithium", 6.94, 1.34, 2.2} },
    {"Be", ElementData{4, "Beryllium", 9.012182, 0.9, 1.9} },
    {"B", ElementData{5, "Boron", 10.81, 0.82, 1.8} },
    {"C", ElementData{6, "Carbon", 12.011, 0.77, 1.7} },
    {"N", ElementData{7, "Nitrogen", 14.007, 0.75, 1.6} },
    {"O", ElementData{8, "Oxygen", 15.999, 0.73, 1.55} },
    {"F", ElementData{9, "Fluorine", 18.9984032, 0.71, 1.5} },
    {"Ne", ElementData{10, "Neon", 20.1797, 0.69, 1.54} },
    {"Na", ElementData{11, "Sodium", 22.98976928, 1.54, 2.4} },
    {"Mg", ElementData{12, "Magnesium", 24.305, 1.3, 2.2} },
    {"Al", ElementData{13, "Aluminium", 26.9815386, 1.18, 2.1} },
    {"Si", ElementData{14, "Silicon", 28.085, 1.11, 2.1} },
    {"P", ElementData{15, "Phosphorus", 30.973762, 1.06, 1.95} },
    {"S", ElementData{16, "Sulfur", 32.06, 1.02, 1.8} },
    {"Cl", ElementData{17, "Chlorine", 35.45, 0.99, 1.8} },
    {"Ar", ElementData{18, "Argon", 39.948, 0.97, 1.88} },
    {"K", ElementData{19, "Potassium", 39.0983, 1.96, 2.8} },
    {"Ca", ElementData{20, "Calcium", 40.078, 1.74, 2.4} },
    {"Sc", ElementData{21, "Scandium", 44.955912, 1.44, 2.3} },
    {"Ti", ElementData{22, "Titanium", 47.867, 1.36, 2.15} },
    {"V", ElementData{23, "Vanadium", 50.9415, 1.25, 2.05} },
    {"Cr", ElementData{24, "Chromium", 51.9961, 1.27, 2.05} },
    {"Mn", ElementData{25, "Manganese", 54.938045, 1.39, 2.05} },
    {"Fe", ElementData{26, "Iron", 55.845, 1.25, 2.05} },
    {"Co", ElementData{27, "Cobalt", 58.933195, 1.26, 2.0} },
    {"Ni", ElementData{28, "Nickel", 58.6934, 1.21, 2.0} },
    {"Cu", ElementData{29, "Copper", 63.546, 1.38, 2.0} },
    {"Zn", ElementData{30, "Zinc", 65.38, 1.31, 2.1} },
    {"Ga", ElementData{31, "Gallium", 69.723, 1.26, 2.1} },
    {"Ge", ElementData{32, "Germanium", 72.63, 1.22, 2.1} },
    {"As", ElementData{33, "Arsenic", 74.9216, 1.19, 2.05} },
    {"Se", ElementData{34, "Selenium", 78.96, 1.16, 1.9} },
    {"Br", ElementData{35, "Bromine", 79.904, 1.14, 1.9} },
    {"Kr", ElementData{36, "Krypton", 83.798, 1.1, 2.02} },
    {"Rb", ElementData{37, "Rubidium", 85.4678, 2.11, 2.9} },
    {"Sr", ElementData{38, "Strontium", 87.62, 1.92, 2.55} },
    {"Y", ElementData{39, "Yttrium", 88.90585, 1.62, 2.4} },
    {"Zr", ElementData{40, "Zirconium", 91.224, 1.48, 2.3} },
    {"Nb", ElementData{41, "Niobium", 92.90638, 1.37, 2.15} },
    {"Mo", ElementData{42, "Molybdenum", 95.96, 1.45, 2.1} },
    {"Tc", ElementData{43, "Technetium", 97.0, 1.56, 2.05} },
    {"Ru", ElementData{44, "Ruthenium", 101.07, 1.26, 2.05} },
    {"Rh", ElementData{45, "Rhodium", 102.9055, 1.35, 2.0} },
    {"Pd", ElementData{46, "Palladium", 106.42, 1.31, 2.05} },
    {"Ag", ElementData{47, "Silver", 107.8682, 1.53, 2.1} },
    {"Cd", ElementData{48, "Cadmium", 112.411, 1.48, 2.2} },
    {"In", ElementData{49, "Indium", 114.818, 1.44, 2.2} },
    {"Sn", ElementData{50, "Tin", 118.71, 1.41, 2.25} },
    {"Sb", ElementData{51, "Antimony", 121.76, 1.38, 2.2} },
    {"Te", ElementData{52, "Tellurium", 127.6, 1.35, 2.1} },
    {"I", ElementData{53, "Iodine", 126.90447, 1.33, 2.1} },
    {"Xe", ElementData{54, "Xenon", 131.293, 1.3, 2.16} },
    {"Cs", ElementData{55, "Caesium", 132.9054519, 2.25, 3.0} },
    {"Ba", ElementData{56, "Barium", 137.327, 1.98, 2.7} },
    {"La", ElementData{57, "Lanthanum", 138.90547, 1.69, 2.5} },
    {"Ce", ElementData{58, "Cerium", 140.116, 1.69, 2.48} },
    {"Pr", ElementData{59, "Praseodymium", 140.90765, 1.69, 2.47} },
    {"Nd", ElementData{60, "Neodymium", 144.242, 1.69, 2.45} },
    {"Pm", ElementData{61, "Promethium", 145.0, 1.69, 2.43} },
    {"Sm", ElementData{62, "Samarium", 150.36, 1.69, 2.42} },
    {"Eu", ElementData{63, "Europium", 151.964, 1.69, 2.4} },
    {"Gd", ElementData{64, "Gadolinium", 157.25, 1.69, 2.38} },
    {"Tb", ElementData{65, "Terbium", 158.92535, 1.69, 2.37} },
    {"Dy", ElementData{66, "Dysprosium", 162.5, 1.69, 2.35} },
    {"Ho", ElementData{67, "Holmium", 164.93032, 1.69, 2.33} },
    {"Er", ElementData{68, "Erbium", 167.259, 1.69, 2.32} },
    {"Tm", ElementData{69, "Thulium", 168.93421, 1.69, 2.3} },
    {"Yb", ElementData{70, "Ytterbium", 173.054, 1.69, 2.28} },
    {"Lu", ElementData{71, "Lutetium", 174.9668, 1.6, 2.27} },
    {"Hf", ElementData{72, "Hafnium", 178.49, 1.5, 2.25} },
    {"Ta", ElementData{73, "Tantalum", 180.94788, 1.38, 2.2} },
    {"W", ElementData{74, "Tungsten", 183.84, 1.46, 2.1} },
    {"Re", ElementData{75, "Rhenium", 186.207, 1.59, 2.05} },
    {"Os", ElementData{76, "Osmium", 190.23, 1.28, 2.0} },
    {"Ir", ElementData{77, "Iridium", 192.217, 1.37, 2.0} },
    {"Pt", ElementData{78, "Platinum", 195.084, 1.28, 2.05} },
    {"Au", ElementData{79, "Gold", 196.966569, 1.44, 2.1} },
    {"Hg", ElementData{80, "Mercury", 200.592, 1.49, 2.05} },
    {"Tl", ElementData{81, "Thallium", 204.38, 1.48, 2.2} },
    {"Pb", ElementData{82, "Lead", 207.2, 1.47, 2.3} },
    {"Bi", ElementData{83, "Bismuth", 208.9804, 1.46, 2.3} },
    {"Po", ElementData{84, "Polonium", 209.0, 1.46, 2.0} },
    {"At", ElementData{85, "Astatine", 210.0, 1.46, 2.0} },
    {"Rn", ElementData{86, "Radon", 222.0, 1.45, 2.0} },
    {"Fr", ElementData{87, "Francium", 223.0, 1.45, 2.0} },
    {"Ra", ElementData{88, "Radium", 226.0, 1.45, 2.0} },
    {"Ac", ElementData{89, "Actinium", 227.0, 1.45, 2.0} },
    {"Th", ElementData{90, "Thorium", 232.03806, 1.45, 2.4} },
    {"Pa", ElementData{91, "Protactinium", 231.03588, 1.45, 2.0} },
    {"U", ElementData{92, "Uranium", 238.02891, 1.45, 2.3} },
    {"Np", ElementData{93, "Neptunium", 237.0, 1.45, 2.0} },
    {"Pu", ElementData{94, "Plutonium", 244.0, 1.45, 2.0} },
    {"Am", ElementData{95, "Americium", 243.0, 1.45, 2.0} },
    {"Cm", ElementData{96, "Curium", 247.0, 1.45, 2.0} },
    {"Bk", ElementData{97, "Berkelium", 247.0, 1.45, 2.0} },
    {"Cf", ElementData{98, "Californium", 251.0, 1.45, 2.0} },
    {"Es", ElementData{99, "Einsteinium", 252.0, 1.45, 2.0} },
    {"Fm", ElementData{100, "Fermium", 257.0, 1.45, 2.0} },
    {"Md", ElementData{101, "Mendelevium", 258.0, 1.45, 2.0} },
    {"No", ElementData{102, "Nobelium", 259.0, 1.45, 2.0} },
    {"Lr", ElementData{103, "Lawrencium", 262.0, 1.45, 2.0} },
    {"Rf", ElementData{104, "Rutherfordium", 267.0, 1.45, 2.0} },
    {"Db", ElementData{105, "Dubnium", 270.0, 1.45, 2.0} },
    {"Sg", ElementData{106, "Seaborgium", 271.0, 1.45, 2.0} },
    {"Bh", ElementData{107, "Bohrium", 270.0, 1.45, 2.0} },
    {"Hs", ElementData{108, "Hassium", 277.0, 1.45, 2.0} },
    {"Mt", ElementData{109, "Meitnerium", 276.0, 1.45, 2.0} },
    {"Ds", ElementData{110, "Darmstadtium", 281.0, 1.45, 2.0} },
    {"Rg", ElementData{111, "Roentgenium", 282.0, 1.45, 2.0} },
    {"Cn", ElementData{112, "Copernicium", 285.0, 1.45, 2.0} },
    {"Uut", ElementData{113, "Ununtrium", 285.0, 1.45, 2.0} },
    {"Fl", ElementData{114, "Flerovium", 289.0, 1.45, 2.0} },
    {"Uup", ElementData{115, "Ununpentium", 289.0, 1.45, 2.0} },
    {"Lv", ElementData{116, "Livermorium", 293.0, 1.45, 2.0} },
    {"Uus", ElementData{117, "Ununseptium", 294.0, 1.45, 2.0} },
    {"Uuo", ElementData{118, "Ununoctium", 294.0, 1.45, 2.0} },
};

} // namespace chemfiles

#endif
