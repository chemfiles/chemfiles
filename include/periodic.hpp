/*
 * Chemharp, an efficient IO library for chemistry file formats
 * Copyright (C) 2015 Guillaume Fraux
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/
 *
 * !!!! AUTO-GENERATED FILE !!!! Do not edit. See elements.py for the code.
 * The data comes from Blue Obelisk's data repository at the svn repository:
 * http://svn.code.sf.net/p/bodr/code/trunk/bodr
 */

#ifndef HARP_PERIODIC_H
#define HARP_PERIODIC_H

#include <map>
#include <string>

//! Storing basic elemental data: mass, colvalent and Van der Waals radii
struct ElementData {
    //! Atomic number
    const int number;
    //! Full name
    const char* name;
    //! Mass in atomic units
    const float mass;
    //! Covalent radius in Angstrom
    const float colvalent_radius;
    //! Van der Waals radius in Angstrom
    const float vdw_radius;
};

static const std::map<std::string, ElementData> PERIODIC_INFORMATION = {
    {"Xx", ElementData{0, "Dummy", 0.0f, 0.0f, 0.0f} },
    {"H", ElementData{1, "Hydrogen", 1.008f, 0.37f, 1.2f} },
    {"He", ElementData{2, "Helium", 4.002602f, 0.32f, 1.4f} },
    {"Li", ElementData{3, "Lithium", 6.94f, 1.34f, 2.2f} },
    {"Be", ElementData{4, "Beryllium", 9.012182f, 0.9f, 1.9f} },
    {"B", ElementData{5, "Boron", 10.81f, 0.82f, 1.8f} },
    {"C", ElementData{6, "Carbon", 12.011f, 0.77f, 1.7f} },
    {"N", ElementData{7, "Nitrogen", 14.007f, 0.75f, 1.6f} },
    {"O", ElementData{8, "Oxygen", 15.999f, 0.73f, 1.55f} },
    {"F", ElementData{9, "Fluorine", 18.9984032f, 0.71f, 1.5f} },
    {"Ne", ElementData{10, "Neon", 20.1797f, 0.69f, 1.54f} },
    {"Na", ElementData{11, "Sodium", 22.98976928f, 1.54f, 2.4f} },
    {"Mg", ElementData{12, "Magnesium", 24.305f, 1.3f, 2.2f} },
    {"Al", ElementData{13, "Aluminium", 26.9815386f, 1.18f, 2.1f} },
    {"Si", ElementData{14, "Silicon", 28.085f, 1.11f, 2.1f} },
    {"P", ElementData{15, "Phosphorus", 30.973762f, 1.06f, 1.95f} },
    {"S", ElementData{16, "Sulfur", 32.06f, 1.02f, 1.8f} },
    {"Cl", ElementData{17, "Chlorine", 35.45f, 0.99f, 1.8f} },
    {"Ar", ElementData{18, "Argon", 39.948f, 0.97f, 1.88f} },
    {"K", ElementData{19, "Potassium", 39.0983f, 1.96f, 2.8f} },
    {"Ca", ElementData{20, "Calcium", 40.078f, 1.74f, 2.4f} },
    {"Sc", ElementData{21, "Scandium", 44.955912f, 1.44f, 2.3f} },
    {"Ti", ElementData{22, "Titanium", 47.867f, 1.36f, 2.15f} },
    {"V", ElementData{23, "Vanadium", 50.9415f, 1.25f, 2.05f} },
    {"Cr", ElementData{24, "Chromium", 51.9961f, 1.27f, 2.05f} },
    {"Mn", ElementData{25, "Manganese", 54.938045f, 1.39f, 2.05f} },
    {"Fe", ElementData{26, "Iron", 55.845f, 1.25f, 2.05f} },
    {"Co", ElementData{27, "Cobalt", 58.933195f, 1.26f, 2.0f} },
    {"Ni", ElementData{28, "Nickel", 58.6934f, 1.21f, 2.0f} },
    {"Cu", ElementData{29, "Copper", 63.546f, 1.38f, 2.0f} },
    {"Zn", ElementData{30, "Zinc", 65.38f, 1.31f, 2.1f} },
    {"Ga", ElementData{31, "Gallium", 69.723f, 1.26f, 2.1f} },
    {"Ge", ElementData{32, "Germanium", 72.63f, 1.22f, 2.1f} },
    {"As", ElementData{33, "Arsenic", 74.9216f, 1.19f, 2.05f} },
    {"Se", ElementData{34, "Selenium", 78.96f, 1.16f, 1.9f} },
    {"Br", ElementData{35, "Bromine", 79.904f, 1.14f, 1.9f} },
    {"Kr", ElementData{36, "Krypton", 83.798f, 1.1f, 2.02f} },
    {"Rb", ElementData{37, "Rubidium", 85.4678f, 2.11f, 2.9f} },
    {"Sr", ElementData{38, "Strontium", 87.62f, 1.92f, 2.55f} },
    {"Y", ElementData{39, "Yttrium", 88.90585f, 1.62f, 2.4f} },
    {"Zr", ElementData{40, "Zirconium", 91.224f, 1.48f, 2.3f} },
    {"Nb", ElementData{41, "Niobium", 92.90638f, 1.37f, 2.15f} },
    {"Mo", ElementData{42, "Molybdenum", 95.96f, 1.45f, 2.1f} },
    {"Tc", ElementData{43, "Technetium", 97.0f, 1.56f, 2.05f} },
    {"Ru", ElementData{44, "Ruthenium", 101.07f, 1.26f, 2.05f} },
    {"Rh", ElementData{45, "Rhodium", 102.9055f, 1.35f, 2.0f} },
    {"Pd", ElementData{46, "Palladium", 106.42f, 1.31f, 2.05f} },
    {"Ag", ElementData{47, "Silver", 107.8682f, 1.53f, 2.1f} },
    {"Cd", ElementData{48, "Cadmium", 112.411f, 1.48f, 2.2f} },
    {"In", ElementData{49, "Indium", 114.818f, 1.44f, 2.2f} },
    {"Sn", ElementData{50, "Tin", 118.71f, 1.41f, 2.25f} },
    {"Sb", ElementData{51, "Antimony", 121.76f, 1.38f, 2.2f} },
    {"Te", ElementData{52, "Tellurium", 127.6f, 1.35f, 2.1f} },
    {"I", ElementData{53, "Iodine", 126.90447f, 1.33f, 2.1f} },
    {"Xe", ElementData{54, "Xenon", 131.293f, 1.3f, 2.16f} },
    {"Cs", ElementData{55, "Caesium", 132.9054519f, 2.25f, 3.0f} },
    {"Ba", ElementData{56, "Barium", 137.327f, 1.98f, 2.7f} },
    {"La", ElementData{57, "Lanthanum", 138.90547f, 1.69f, 2.5f} },
    {"Ce", ElementData{58, "Cerium", 140.116f, 1.69f, 2.48f} },
    {"Pr", ElementData{59, "Praseodymium", 140.90765f, 1.69f, 2.47f} },
    {"Nd", ElementData{60, "Neodymium", 144.242f, 1.69f, 2.45f} },
    {"Pm", ElementData{61, "Promethium", 145.0f, 1.69f, 2.43f} },
    {"Sm", ElementData{62, "Samarium", 150.36f, 1.69f, 2.42f} },
    {"Eu", ElementData{63, "Europium", 151.964f, 1.69f, 2.4f} },
    {"Gd", ElementData{64, "Gadolinium", 157.25f, 1.69f, 2.38f} },
    {"Tb", ElementData{65, "Terbium", 158.92535f, 1.69f, 2.37f} },
    {"Dy", ElementData{66, "Dysprosium", 162.5f, 1.69f, 2.35f} },
    {"Ho", ElementData{67, "Holmium", 164.93032f, 1.69f, 2.33f} },
    {"Er", ElementData{68, "Erbium", 167.259f, 1.69f, 2.32f} },
    {"Tm", ElementData{69, "Thulium", 168.93421f, 1.69f, 2.3f} },
    {"Yb", ElementData{70, "Ytterbium", 173.054f, 1.69f, 2.28f} },
    {"Lu", ElementData{71, "Lutetium", 174.9668f, 1.6f, 2.27f} },
    {"Hf", ElementData{72, "Hafnium", 178.49f, 1.5f, 2.25f} },
    {"Ta", ElementData{73, "Tantalum", 180.94788f, 1.38f, 2.2f} },
    {"W", ElementData{74, "Tungsten", 183.84f, 1.46f, 2.1f} },
    {"Re", ElementData{75, "Rhenium", 186.207f, 1.59f, 2.05f} },
    {"Os", ElementData{76, "Osmium", 190.23f, 1.28f, 2.0f} },
    {"Ir", ElementData{77, "Iridium", 192.217f, 1.37f, 2.0f} },
    {"Pt", ElementData{78, "Platinum", 195.084f, 1.28f, 2.05f} },
    {"Au", ElementData{79, "Gold", 196.966569f, 1.44f, 2.1f} },
    {"Hg", ElementData{80, "Mercury", 200.592f, 1.49f, 2.05f} },
    {"Tl", ElementData{81, "Thallium", 204.38f, 1.48f, 2.2f} },
    {"Pb", ElementData{82, "Lead", 207.2f, 1.47f, 2.3f} },
    {"Bi", ElementData{83, "Bismuth", 208.9804f, 1.46f, 2.3f} },
    {"Po", ElementData{84, "Polonium", 209.0f, 1.46f, 2.0f} },
    {"At", ElementData{85, "Astatine", 210.0f, 1.46f, 2.0f} },
    {"Rn", ElementData{86, "Radon", 222.0f, 1.45f, 2.0f} },
    {"Fr", ElementData{87, "Francium", 223.0f, 1.45f, 2.0f} },
    {"Ra", ElementData{88, "Radium", 226.0f, 1.45f, 2.0f} },
    {"Ac", ElementData{89, "Actinium", 227.0f, 1.45f, 2.0f} },
    {"Th", ElementData{90, "Thorium", 232.03806f, 1.45f, 2.4f} },
    {"Pa", ElementData{91, "Protactinium", 231.03588f, 1.45f, 2.0f} },
    {"U", ElementData{92, "Uranium", 238.02891f, 1.45f, 2.3f} },
    {"Np", ElementData{93, "Neptunium", 237.0f, 1.45f, 2.0f} },
    {"Pu", ElementData{94, "Plutonium", 244.0f, 1.45f, 2.0f} },
    {"Am", ElementData{95, "Americium", 243.0f, 1.45f, 2.0f} },
    {"Cm", ElementData{96, "Curium", 247.0f, 1.45f, 2.0f} },
    {"Bk", ElementData{97, "Berkelium", 247.0f, 1.45f, 2.0f} },
    {"Cf", ElementData{98, "Californium", 251.0f, 1.45f, 2.0f} },
    {"Es", ElementData{99, "Einsteinium", 252.0f, 1.45f, 2.0f} },
    {"Fm", ElementData{100, "Fermium", 257.0f, 1.45f, 2.0f} },
    {"Md", ElementData{101, "Mendelevium", 258.0f, 1.45f, 2.0f} },
    {"No", ElementData{102, "Nobelium", 259.0f, 1.45f, 2.0f} },
    {"Lr", ElementData{103, "Lawrencium", 262.0f, 1.45f, 2.0f} },
    {"Rf", ElementData{104, "Rutherfordium", 267.0f, 1.45f, 2.0f} },
    {"Db", ElementData{105, "Dubnium", 270.0f, 1.45f, 2.0f} },
    {"Sg", ElementData{106, "Seaborgium", 271.0f, 1.45f, 2.0f} },
    {"Bh", ElementData{107, "Bohrium", 270.0f, 1.45f, 2.0f} },
    {"Hs", ElementData{108, "Hassium", 277.0f, 1.45f, 2.0f} },
    {"Mt", ElementData{109, "Meitnerium", 276.0f, 1.45f, 2.0f} },
    {"Ds", ElementData{110, "Darmstadtium", 281.0f, 1.45f, 2.0f} },
    {"Rg", ElementData{111, "Roentgenium", 282.0f, 1.45f, 2.0f} },
    {"Cn", ElementData{112, "Copernicium", 285.0f, 1.45f, 2.0f} },
    {"Uut", ElementData{113, "Ununtrium", 285.0f, 1.45f, 2.0f} },
    {"Fl", ElementData{114, "Flerovium", 289.0f, 1.45f, 2.0f} },
    {"Uup", ElementData{115, "Ununpentium", 289.0f, 1.45f, 2.0f} },
    {"Lv", ElementData{116, "Livermorium", 293.0f, 1.45f, 2.0f} },
    {"Uus", ElementData{117, "Ununseptium", 294.0f, 1.45f, 2.0f} },
    {"Uuo", ElementData{118, "Ununoctium", 294.0f, 1.45f, 2.0f} },
};

#endif
