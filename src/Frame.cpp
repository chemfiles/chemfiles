// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iosfwd>
#include <iterator>
#include <string>
#include <vector>

#include "chemfiles/Frame.hpp"

#include "chemfiles/Atom.hpp"
#include "chemfiles/Configuration.hpp"
#include "chemfiles/Connectivity.hpp"
#include "chemfiles/Topology.hpp"
#include "chemfiles/UnitCell.hpp"
#include "chemfiles/error_fmt.hpp"
#include "chemfiles/external/optional.hpp"
#include "chemfiles/types.hpp"

using namespace chemfiles;

// get radius compatible with VMD bond guessing algorithm
static optional<double> guess_bonds_radius(const Atom& atom);

Frame::Frame(UnitCell cell) : cell_(std::move(cell)) {} // NOLINT: std::move for trivially copiable type

size_t Frame::size() const {
   assert(positions_.size() == topology_.size());
   if (velocities_) {
      assert(positions_.size() == velocities_->size());
   }
   return positions_.size();
}

void Frame::resize(size_t size) {
   topology_.resize(size);
   positions_.resize(size);
   if (velocities_) {
      velocities_->resize(size);
   }
}

void Frame::reserve(size_t size) {
   topology_.reserve(size);
   positions_.reserve(size);
   if (velocities_) {
      velocities_->reserve(size);
   }
}

void Frame::add_velocities() {
   if (!velocities_) {
      velocities_ = std::vector<Vector3D>(size());
   }
}

void Frame::guess_bonds() {
   topology_.clear_bonds();
   // This bond guessing algorithm comes from VMD
   auto cutoff = 0.833;
   for (size_t i = 0; i < size(); i++) {
      auto rad = guess_bonds_radius(topology_[i]).value_or(0);
      cutoff = std::max(cutoff, rad);
   }
   cutoff = 1.2 * cutoff;

   for (size_t i = 0; i < size(); i++) {
      auto i_radius = guess_bonds_radius(topology_[i]);
      if (!i_radius) {
         throw error(
            "missing Van der Waals radius for '{}'", topology_[i].type()
         );
      }
      for (size_t j = i + 1; j < size(); j++) {
         auto j_radius = guess_bonds_radius(topology_[j]);
         if (!j_radius) {
            throw error(
               "missing Van der Waals radius for '{}'", topology_[j].type()
            );
         }
         auto d = distance(i, j);
         auto radii = i_radius.value() + j_radius.value();
         if (0.03 < d && d < 0.6 * radii && d < cutoff) {
            topology_.add_bond(i, j);
         }
      }
   }

   auto bonds = topology().bonds();
   auto to_remove = std::vector<Bond>();
   // We need to remove bonds between hydrogen atoms which are bonded more than
   // once
   for (auto& bond : bonds) {
      auto i = bond[0], j = bond[1];
      if (topology_[i].type() != "H") {
         continue;
      }
      if (topology_[j].type() != "H") {
         continue;
      }

      auto nbonds = std::count_if(
         std::begin(bonds), std::end(bonds), [=](const Bond& b) {
            return b[0] == i || b[0] == j || b[1] == i || b[1] == j;
         });
      assert(nbonds >= 1);

      if (nbonds != 1) {
         to_remove.push_back(bond);
      }
   }

   for (auto& bond : to_remove) {
      topology_.remove_bond(bond[0], bond[1]);
   }
}

void Frame::getMinMaxBox(Vector3D& min, Vector3D& max) {

   float x1, x2, y1, y2, z1, z2;

   //Consider first atom's coordinates as min and max.
   x1 = this->positions_[0][0];
   x2 = this->positions_[0][0];
   y1 = this->positions_[0][1];
   y2 = this->positions_[0][1];
   z1 = this->positions_[0][2];
   z2 = this->positions_[0][2];

   for (const auto& position : this->positions_) {
      if (position[0] < x1) x1 = position[0];
      if (position[0] > x2) x2 = position[0];
      if (position[1] < y1) y1 = position[1];
      if (position[1] > y2) y2 = position[1];
      if (position[2] < z1) z1 = position[2];
      if (position[2] > z2) z2 = position[2];
   }

   assert(x2 > x1);
   assert(y2 > y1);
   assert(z2 > z1);

   min[0] = x1;
   max[0] = x2;
   min[1] = y1;
   max[1] = y2;
   min[2] = z1;
   max[2] = z2;
}

size_t Frame::createNeighborList(std::vector<std::vector<size_t>>& nbList, size_t xCells, 
   size_t yCells, size_t zCells) {

   size_t xyTotalCells = xCells * yCells;
   size_t cellIndex = 0;
   for (size_t zCell = 0; zCell < zCells; zCell++) {
      for (size_t yCell = 0; yCell < yCells; yCell++) {
         for (size_t xCell = 0; xCell < xCells; xCell++) {
            nbList[cellIndex].reserve(14);
            nbList[cellIndex].emplace_back(cellIndex);
            if (xCell < xCells - 1) {
               nbList[cellIndex].emplace_back(cellIndex + 1);
            }
            if (yCell < yCells - 1) {
               nbList[cellIndex].emplace_back(cellIndex + xCells);
            }
            if (zCell < zCells - 1) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells);
            }
            if (xCell < (xCells - 1) && yCell < (yCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xCells + 1);
            }
            if (xCell < (xCells - 1) && zCell < (zCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells + 1);
            }
            if (yCell < (yCells - 1) && zCell < (zCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells + xCells);
            }
            if (xCell < (xCells - 1) && yCell > 0) {
               nbList[cellIndex].emplace_back(cellIndex - xCells + 1);
            }
            if (xCell > 0 && zCell < (zCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells - 1);
            }
            if (yCell > 0 && zCell < (zCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells - xCells);
            }
            if (xCell < (xCells - 1) && yCell < (yCells - 1) && zCell < (zCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells + xCells + 1);
            }
            if (xCell > 0 && yCell < (yCells - 1) && zCell < (zCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells + xCells - 1);
            }
            if (xCell < (xCells - 1) && yCell > 0 && zCell < (zCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells - xCells + 1);
            }
            if (xCell > 0 && yCell > 0 && zCell < (zCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells - xCells - 1);
            }
            nbList[cellIndex].shrink_to_fit();
            cellIndex++;
         }
      }
   }
   return 0;
}

size_t Frame::createNeighborListSym(std::vector<std::vector<size_t>>& nbList, size_t xCells,
   size_t yCells, size_t zCells) {

   size_t xyTotalCells = xCells * yCells;
   size_t cellIndex = 0;
   for (size_t zCell = 0; zCell < zCells; zCell++) {
      for (size_t yCell = 0; yCell < yCells; yCell++) {
         for (size_t xCell = 0; xCell < xCells; xCell++) {
            nbList[cellIndex].reserve(27);
            nbList[cellIndex].emplace_back(cellIndex);
            if (xCell < xCells - 1) {
               nbList[cellIndex].emplace_back(cellIndex + 1);
            }
            if (yCell < yCells - 1) {
               nbList[cellIndex].emplace_back(cellIndex + xCells);
            }
            if (zCell < zCells - 1) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells);
            }
            if (xCell < (xCells - 1) && yCell < (yCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xCells + 1);
            }
            if (xCell < (xCells - 1) && zCell < (zCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells + 1);
            }
            if (yCell < (yCells - 1) && zCell < (zCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells + xCells);
            }
            if (xCell < (xCells - 1) && yCell > 0) {
               nbList[cellIndex].emplace_back(cellIndex - xCells + 1);
            }
            if (xCell > 0 && zCell < (zCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells - 1);
            }
            if (yCell > 0 && zCell < (zCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells - xCells);
            }
            if (xCell < (xCells - 1) && yCell < (yCells - 1) && zCell < (zCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells + xCells + 1);
            }
            if (xCell > 0 && yCell < (yCells - 1) && zCell < (zCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells + xCells - 1);
            }
            if (xCell < (xCells - 1) && yCell > 0 && zCell < (zCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells - xCells + 1);
            }
            if (xCell > 0 && yCell > 0 && zCell < (zCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xyTotalCells - xCells - 1);
            }
            if (xCell) {
               nbList[cellIndex].emplace_back(cellIndex - 1);
            }
            if (yCell) {
               nbList[cellIndex].emplace_back(cellIndex - xCells);
            }
            if (zCell) {
               nbList[cellIndex].emplace_back(cellIndex - xyTotalCells);
            }
            if (xCell && yCell) {
               nbList[cellIndex].emplace_back(cellIndex - xCells - 1);
            }
            if (xCell && zCell) {
               nbList[cellIndex].emplace_back(cellIndex - xyTotalCells - 1);
            }
            if (yCell && zCell) {
               nbList[cellIndex].emplace_back(cellIndex - xyTotalCells - xCells);
            }
            if (xCell && yCell < (yCells - 1)) {
               nbList[cellIndex].emplace_back(cellIndex + xCells - 1);
            }
            if (xCell < (xCells - 1) && zCell) {
               nbList[cellIndex].emplace_back(cellIndex - xyTotalCells + 1);
            }
            if (yCell < (yCells - 1) && zCell) {
               nbList[cellIndex].emplace_back(cellIndex - xyTotalCells + xCells);
            }
            if (xCell && yCell && zCell) {
               nbList[cellIndex].emplace_back(cellIndex - xyTotalCells - xCells - 1);
            }
            if (xCell < (xCells - 1) && yCell && zCell) {
               nbList[cellIndex].emplace_back(cellIndex - xyTotalCells - xCells + 1);
            }
            if (xCell && yCell < (yCells - 1) && zCell) {
               nbList[cellIndex].emplace_back(cellIndex - xyTotalCells + xCells - 1);
            }
            if (xCell < (xCells - 1) && yCell < (yCells - 1) && zCell) {
               nbList[cellIndex].emplace_back(cellIndex - xyTotalCells + xCells + 1);
            }
            nbList[cellIndex].shrink_to_fit();
            cellIndex++;
         }
      }
   }
   return 0;
}

void Frame::guess_bonds_cls() {

   size_t numAtoms = size();

   topology_.clear_bonds();

   auto cutoff = 0.833;
   for (size_t i = 0; i < size(); i++) {
      auto rad = guess_bonds_radius(topology_[i]).value_or(0);
      cutoff = std::max(cutoff, rad);
   }
   cutoff = 1.2 * cutoff;

   if (numAtoms == 0 || cutoff == 0.0) {
      return;
   }

   //Get box's min and max
   Vector3D min, max;
   getMinMaxBox(min, max);

   //Box's xyz ranges
   float xRange = max[0] - min[0];
   float yRange = max[1] - min[1];
   float zRange = max[2] - min[2];

   //Box's number of cells in each dim, in x and y dimensions, and in total (in x, y and z dimensions)
   const float inversedCutoff = 1.0 / cutoff;
   size_t xCells = std::ceil(xRange * inversedCutoff);
   size_t yCells = std::ceil(yRange * inversedCutoff);
   size_t zCells = std::ceil(zRange * inversedCutoff);
   size_t xyTotalCells = yCells * xCells;
   size_t totalCells = xyTotalCells * zCells;

   //Limit number of cells in the box (to avoid memory issues)
   if (totalCells > 40000000000)
   {
      throw error(
         "Exceeded maximum cells"
      );
      return;
   }

   //Sort atoms in cells
   std::vector<std::vector<size_t>> cells;
   cells.resize(totalCells);

   for (size_t atomIndex = 0; atomIndex < numAtoms; atomIndex++) {
      size_t atomXCellNum, atomYCellNum, atomZCellNum, atomCellIndex;

      //Compute atom's cell index
      Vector3D atomCoord = positions_[atomIndex];
      atomXCellNum = std::floor((atomCoord[0] - min[0]) * inversedCutoff);
      atomYCellNum = std::floor((atomCoord[1] - min[1]) * inversedCutoff);
      atomZCellNum = std::floor((atomCoord[2] - min[2]) * inversedCutoff);

      atomCellIndex = atomZCellNum * xyTotalCells + atomYCellNum * xCells + atomXCellNum;

      if (cells[atomCellIndex].capacity() == 0)
      {
         cells[atomCellIndex].reserve(8);
      }
      else if (cells[atomCellIndex].capacity() == cells[atomCellIndex].size())
      {
         cells[atomCellIndex].reserve(cells[atomCellIndex].capacity() + 4);
      }
      
      //Store atom index in appropriate cell
      cells[atomCellIndex].emplace_back(atomIndex);
   }

   for (size_t cell = 0; cell < cells.size(); cell++)
   {
      cells[cell].shrink_to_fit();
   }

   //Create the neighbor vector
   std::vector<std::vector<size_t>> nbList;
   nbList.resize(totalCells);
   createNeighborList(nbList, xCells, yCells, zCells);

   //Max number of 12 bonds per atom
   size_t maxNumBonds = numAtoms * 12;
   createBonds(cells, nbList, maxNumBonds, cutoff);
}

void Frame::set_topology(Topology topology) {
   if (topology.size() != size()) {
      throw error(
         "the topology contains {} atoms, but the frame contains {} atoms",
         topology.size(), size()
      );
   }
   topology_ = std::move(topology);
}

void Frame::add_atom(Atom atom, Vector3D position, Vector3D velocity) {
   topology_.add_atom(std::move(atom));
   positions_.push_back(position);
   if (velocities_) {
      velocities_->push_back(velocity);
   }
   assert(size() == topology_.size());
}

void Frame::remove(size_t i) {
   if (i >= size()) {
      throw out_of_bounds(
         "out of bounds atomic index in `Frame::remove`: we have {} atoms, "
         "but the index is {}",
         size(), i
      );
   }
   topology_.remove(i);
   positions_.erase(positions_.begin() + static_cast<std::ptrdiff_t>(i));
   if (velocities_) {
      velocities_->erase(velocities_->begin() + static_cast<std::ptrdiff_t>(i));
   }
   assert(size() == topology_.size());
}

double Frame::distance(size_t i, size_t j) const {
   if (i >= size() || j >= size()) {
      throw out_of_bounds(
         "out of bounds atomic index in `Frame::distance`: we have {} "
         "atoms, but the index are {} and {}",
         size(), i, j
      );
   }

   auto rij = positions_[i] - positions_[j];
   return cell_.wrap(rij).norm();
}

double Frame::angle(size_t i, size_t j, size_t k) const {
   if (i >= size() || j >= size() || k >= size()) {
      throw out_of_bounds(
         "out of bounds atomic index in `Frame::angle`: we have {} atoms, "
         "but the index are {}, {}, and {}",
         size(), i, j, k
      );
   }

   auto rij = cell_.wrap(positions_[i] - positions_[j]);
   auto rkj = cell_.wrap(positions_[k] - positions_[j]);

   auto cos = dot(rij, rkj) / (rij.norm() * rkj.norm());
   cos = std::max(-1.0, std::min(1.0, cos));
   return acos(cos);
}

double Frame::dihedral(size_t i, size_t j, size_t k, size_t m) const {
   if (i >= size() || j >= size() || k >= size() || m >= size()) {
      throw out_of_bounds(
         "out of bounds atomic index in `Frame::dihedral`: we have {} "
         "atoms, but the index are {}, {}, {}, and {}",
         size(), i, j, k, m
      );
   }

   auto rij = cell_.wrap(positions_[i] - positions_[j]);
   auto rjk = cell_.wrap(positions_[j] - positions_[k]);
   auto rkm = cell_.wrap(positions_[k] - positions_[m]);

   auto a = cross(rij, rjk);
   auto b = cross(rjk, rkm);
   return atan2(rjk.norm() * dot(b, rij), dot(a, b));
}

double Frame::out_of_plane(size_t i, size_t j, size_t k, size_t m) const {
   if (i >= size() || j >= size() || k >= size() || m >= size()) {
      throw out_of_bounds(
         "out of bounds atomic index in `Frame::out_of_plane`: we have {} "
         "atoms, but the index are {}, {}, {}, and {}",
         size(), i, j, k, m
      );
   }

   auto rji = cell_.wrap(positions_[j] - positions_[i]);
   auto rik = cell_.wrap(positions_[i] - positions_[k]);
   auto rim = cell_.wrap(positions_[i] - positions_[m]);

   auto n = cross(rik, rim);
   auto n_norm = n.norm();
   if (n_norm < 1e-12) {
      // if i, k, and m are colinear, then j is always inside the plane
      return 0;
   }
   else {
      return dot(rji, n) / n_norm;
   }
}

const std::unordered_map<std::string, double> BOND_GUESSING_RADII = {
    {"H", 1.0},
    {"C", 1.5},
    {"O", 1.3},
    {"N", 1.4},
    {"S", 1.9},
    {"F", 1.2},
};

optional<double> guess_bonds_radius(const Atom& atom) {
   const auto& type = atom.type();
   auto it = BOND_GUESSING_RADII.find(type);
   if (it != BOND_GUESSING_RADII.end()) {
      // allow configuration file to override data from BOND_GUESSING_RADII
      auto user_config = Configuration::atom_data(type);
      if (user_config && user_config->vdw_radius) {
         return user_config->vdw_radius.value();
      }
      else {
         return it->second;
      }
   }
   else {
      // default to chemfiles provided atom type
      return atom.vdw_radius();
   }
}

size_t Frame::createBonds(std::vector<std::vector<size_t>>& cells,
   std::vector<std::vector<size_t>>& neighborList, size_t maxBonds, float cutoff)
{
   size_t bondsCount = 0;
   float cutoffPow2 = cutoff * cutoff;

   for (size_t cellIndex = 0; (cellIndex < cells.size()) && (bondsCount < maxBonds); cellIndex++) {
      size_t atomsInCell = cells[cellIndex].size();

      for (size_t nghb = 0; nghb < neighborList[cellIndex].size() && bondsCount < maxBonds; nghb++) {
         size_t neighborCellIndex = neighborList[cellIndex][nghb];
         size_t atomNumInCell = cells[neighborCellIndex].size();

         bool selfCell;
         if (cellIndex == neighborCellIndex)
            selfCell = true;
         else
            selfCell = false;

         for (size_t i = 0; i < atomsInCell && bondsCount < maxBonds; i++) {
            size_t atom1 = cells[cellIndex][i];
            //Skip over self and already-tested atoms
            size_t firstNeighborCell = (selfCell) ? i + 1 : 0;

            for (size_t j = firstNeighborCell; (j < atomNumInCell) && (bondsCount < maxBonds); j++) {
               size_t atom2 = cells[neighborCellIndex][j];

               float interAtomicDist = this->distance(atom1, atom2);
               interAtomicDist *= interAtomicDist;

               //Perform distance test and ignore atoms with almost the same coordinates
               if ((interAtomicDist > cutoffPow2) || (interAtomicDist < 0.001)) {
                  continue;
               }

               auto atom1Radius = guess_bonds_radius(topology_[atom1]);
               if (!atom1Radius) {
                  throw error(
                     "missing Van der Waals radius for '{}'", topology_[atom1].type()
                  );
               }
               auto atom2Radius = guess_bonds_radius(topology_[atom2]);
               if (!atom2Radius) {
                  throw error(
                     "missing Van der Waals radius for '{}'", topology_[atom2].type()
                  );
               }
               auto radii = atom1Radius.value() + atom2Radius.value();
               if (interAtomicDist > 0.03 && interAtomicDist < radii) {
                  const auto& atm1 = topology_[atom1];
                  const auto& atm2 = topology_[atom2];
                  //Prevent hydrogen atoms from bonding with each other
                  if (atm1.type() != "H" || atm2.type() != "H") {
                     // Add a bond
                     add_bond(atom1, atom2);
                     bondsCount++;
                  }
               }
            }
         }
      }
   }

   return bondsCount;
}
