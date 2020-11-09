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

void Frame::getMinMaxBox(std::vector<Vector3D> pos, int n, float* min, float* max) {

	float x1, x2, y1, y2, z1, z2;

	//Consider first atom's coordinates as min and max.
	x1 = pos[0][0];
	x2 = pos[0][0];
	y1 = pos[0][1];
	y2 = pos[0][1];
	z1 = pos[0][2];
	z2 = pos[0][2];

	for (int i = 1; i < n; i++) {
		if (pos[i][0] < x1) x1 = pos[i][0];
		if (pos[i][0] > x2) x2 = pos[i][0];
		if (pos[i][1] < y1) y1 = pos[i][1];
		if (pos[i][1] > y2) y2 = pos[i][1];
		if (pos[i][2] < z1) z1 = pos[i][2];
		if (pos[i][2] > z2) z2 = pos[i][2];
	}

	min[0] = x1;
	max[0] = x2;
	min[1] = y1;
	max[1] = y2;
	min[2] = z1;
	max[2] = z2;
}

int Frame::createNeighborList(int** nbList, int xCells, int yCells, int zCells) {

	if (nbList == NULL) {
		return -1;
	}

	int xyTotalCells = xCells * yCells;
	int cellIndex = 0;
	for (int zCell = 0; zCell < zCells; zCell++) {
		for (int yCell = 0; yCell < yCells; yCell++) {
			for (int xCell = 0; xCell < xCells; xCell++) {

				//27 neighbors plus a -1 for the end of the list
				int neighbors[28];
				int n = 0;
				neighbors[n++] = cellIndex;

				if (xCell < xCells - 1) {
					neighbors[n++] = cellIndex + 1;
				}
				if (yCell < yCells - 1) {
					neighbors[n++] = cellIndex + xCells;
				}
				if (zCell < zCells - 1) {
					neighbors[n++] = cellIndex + xyTotalCells;
				}
				if (xCell < (xCells - 1) && yCell < (yCells - 1)) {
					neighbors[n++] = cellIndex + xCells + 1;
				}
				if (xCell < (xCells - 1) && zCell < (zCells - 1)) {
					neighbors[n++] = cellIndex + xyTotalCells + 1;
				}
				if (yCell < (yCells - 1) && zCell < (zCells - 1)) {
					neighbors[n++] = cellIndex + xyTotalCells + xCells;
				}
				if (xCell < (xCells - 1) && yCell > 0) {
					neighbors[n++] = cellIndex - xCells + 1;
				}
				if (xCell > 0 && zCell < (zCells - 1)) {
					neighbors[n++] = cellIndex + xyTotalCells - 1;
				}
				if (yCell > 0 && zCell < (zCells - 1)) {
					neighbors[n++] = cellIndex + xyTotalCells - xCells;
				}
				if (xCell < (xCells - 1) && yCell < (yCells - 1) && zCell < (zCells - 1)) {
					neighbors[n++] = cellIndex + xyTotalCells + xCells + 1;
				}
				if (xCell > 0 && yCell < (yCells - 1) && zCell < (zCells - 1)) {
					neighbors[n++] = cellIndex + xyTotalCells + xCells - 1;
				}
				if (xCell < (xCells - 1) && yCell > 0 && zCell < (zCells - 1)) {
					neighbors[n++] = cellIndex + xyTotalCells - xCells + 1;
				}
				if (xCell > 0 && yCell > 0 && zCell < (zCells - 1)) {
					neighbors[n++] = cellIndex + xyTotalCells - xCells - 1;
				}
				if (xCell) {
					neighbors[n++] = cellIndex - 1;
				}
				if (yCell) {
					neighbors[n++] = cellIndex - xCells;
				}
				if (zCell) {
					neighbors[n++] = cellIndex - xyTotalCells;
				}
				if (xCell && yCell) {
					neighbors[n++] = cellIndex - xCells - 1;
				}
				if (xCell && zCell) {
					neighbors[n++] = cellIndex - xyTotalCells - 1;
				}
				if (yCell && zCell) {
					neighbors[n++] = cellIndex - xyTotalCells - xCells;
				}
				if (xCell && yCell < (yCells - 1)) {
					neighbors[n++] = cellIndex + xCells - 1;
				}
				if (xCell < (xCells - 1) && zCell) {
					neighbors[n++] = cellIndex - xyTotalCells + 1;
				}
				if (yCell < (yCells - 1) && zCell) {
					neighbors[n++] = cellIndex - xyTotalCells + xCells;
				}
				if (xCell && yCell && zCell) {
					neighbors[n++] = cellIndex - xyTotalCells - xCells - 1;
				}
				if (xCell < (xCells - 1) && yCell && zCell) {
					neighbors[n++] = cellIndex - xyTotalCells - xCells + 1;
				}
				if (xCell && yCell < (yCells - 1) && zCell) {
					neighbors[n++] = cellIndex - xyTotalCells + xCells - 1;
				}
				if (xCell < (xCells - 1) && yCell < (yCells - 1) && zCell) {
					neighbors[n++] = cellIndex - xyTotalCells + xCells + 1;
				}

				//End of the list
				neighbors[n++] = -1;

				int* lst = (int*)malloc(n * sizeof(int));
				//Check allocation
				if (lst == NULL) {
					return -1;
				}
				memcpy(lst, neighbors, n * sizeof(int));
				nbList[cellIndex] = lst;

				cellIndex++;
			}
		}
	}

	return 0;
}

std::vector<int>* Frame::searchBonds(std::vector<Vector3D> positions, int natoms, float cutoff, int maxBonds) {

	//Get box's min and max
	float min[3], max[3];
	getMinMaxBox(positions, natoms, min, max);

	//Box's xyz ranges
	float xRange = max[0] - min[0];
	float yRange = max[1] - min[1];
	float zRange = max[2] - min[2];

	//Box's number of cells in xyz and in total
	const float inversedCutoff = 1.0 / cutoff;
	int xCells = ((int)(xRange * inversedCutoff)) + 1;
	int yCells = ((int)(yRange * inversedCutoff)) + 1;
	int zCells = ((int)(zRange * inversedCutoff)) + 1;
	int xyTotalCells = yCells * xCells;
	int totalCells = xyTotalCells * zCells;

	//Check if total cells is not negative
	if (totalCells <= 0)
	{
		throw error(
			"Negative number of cells"
		);
		return NULL;
	}

	//Limit number of cells in the box (to avoid memory issues)
	if (totalCells > 40000000000)
	{
		throw error(
			"Exceeded maximum cells"
		);
		return NULL;
	}

	//Sort atoms in cells
	int** cells = (int**)calloc(totalCells, sizeof(int*));
	int* numInCell = (int*)calloc(totalCells, sizeof(int));
	int* maxInCell = (int*)calloc(totalCells, sizeof(int));

	for (int i = 0; i < natoms; i++) {
		int atomXCellNum, atomYCellNum, atomZCellNum, atomCellIndex, numOfAtomsInCell;

		//Compute atom's cell index
		Vector3D atomCoord = positions[i];
		atomXCellNum = (int)((atomCoord[0] - min[0]) * inversedCutoff);
		atomYCellNum = (int)((atomCoord[1] - min[1]) * inversedCutoff);
		atomZCellNum = (int)((atomCoord[2] - min[2]) * inversedCutoff);

		atomCellIndex = atomZCellNum * xyTotalCells + atomYCellNum * xCells + atomXCellNum;

		//Grow cell
		numOfAtomsInCell = numInCell[atomCellIndex];
		if (numOfAtomsInCell == maxInCell[atomCellIndex]) {
			cells[atomCellIndex] = (int*)realloc(cells[atomCellIndex], (numOfAtomsInCell + 4) * sizeof(int));
			maxInCell[atomCellIndex] += 4;
		}

		//Store atom index in appropriate cell
		cells[atomCellIndex][numOfAtomsInCell] = i;
		numInCell[atomCellIndex]++;
	}
	free(maxInCell);

	//Create the neighbor list 
	int** nbList = (int**)calloc(totalCells, sizeof(int*));
	createNeighborList(nbList, xCells, yCells, zCells);

	//Setup result
	std::vector<int>* result = new std::vector<int>();

	int bondsCount = generateBonds(result, &positions, totalCells, cells, numInCell, nbList, maxBonds, cutoff);

	if (bondsCount == 0 || bondsCount > maxBonds)
	{
		for (int i = 0; i < totalCells; i++) {
			free(cells[i]);
			free(nbList[i]);
		}
		free(cells);
		free(nbList);
		free(numInCell);

		if (bondsCount == 0)
		{
			throw error(
				"no bonds found"
			);
		}
		else if (bondsCount > maxBonds)
		{
			throw error(
				"exceeded max number of bonds"
			);
		}

		return NULL;
	}

	for (int i = 0; i < totalCells; i++) {
		free(cells[i]);
		free(nbList[i]);
	}
	free(cells);
	free(nbList);
	free(numInCell);

	return result;
}

void Frame::guess_bonds_cls() {

	int numAtoms = size();

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

	//Generate pointer to bonds vector
	int maxBondsPerAtom = numAtoms * 12;
	std::vector<int>* bondResults = searchBonds(positions_, numAtoms, cutoff, maxBondsPerAtom);

	// Go through the bond results vector to add bonds to connectivity
	for (int bond = 0; bond < bondResults->size() / 2; bond++) {

		int atom1Index = bondResults->at(bond * 2);
		int atom2Index = bondResults->at((bond * 2) + 1);

		Atom atom1 = topology_[atom1Index];
		Atom atom2 = topology_[atom2Index];

		//Prevent hydrogen atoms from bonding with each other
		std::string atom1Type = atom1.type();
		std::string atom2Type = atom2.type();
		if (atom1Type != "H" || atom2Type != "H") {
			// Add a bond, bondorder defaults to 1, bond type to -1
			add_bond(atom1Index, atom2Index);
		}
	}
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

int Frame::generateBonds(std::vector<int>* result, std::vector<Vector3D>* positions, int totalCells, int** cells,
	int* numInCell, int** neighborList, int maxBonds, float cutoff)
{
	int i, j, cellIndex;
	int bondsCount = 0;

	float cutoffPow2 = cutoff * cutoff;

	for (cellIndex = 0; (cellIndex < totalCells) && (bondsCount < maxBonds); cellIndex += 1) {

		const int* tmpCell = cells[cellIndex];
		int atomsInCell = numInCell[cellIndex];

		if (((cellIndex & 255) == 0)) {
			char tmpbuf[128];
			sprintf(tmpbuf, "%6.2f", (100.0f * cellIndex) / (float)totalCells);
			printf("Generating bonds...: %s %% complete\n", tmpbuf);
		}

		for (const int* neighborCellsIndexes = neighborList[cellIndex]; (*neighborCellsIndexes != -1) && (bondsCount < maxBonds); neighborCellsIndexes++) {

			int neighborCellIndex = *neighborCellsIndexes;
			const int* neighborCell = cells[neighborCellIndex];
			int atomNumInCell = numInCell[neighborCellIndex];
			bool selfCell;
			if (cellIndex == neighborCellIndex)
				selfCell = true;
			else
				selfCell = false;

			for (i = 0; (i < atomsInCell) && (bondsCount < maxBonds); i++) {
				int atom1 = tmpCell[i];
				Vector3D posAtom1 = positions->at(atom1);

				//Skip over self and already-tested atoms
				int firstNeighborCell = (selfCell) ? i + 1 : 0;

				for (j = firstNeighborCell; (j < atomNumInCell) && (bondsCount < maxBonds); j++) {
					int atom2 = neighborCell[j];
					Vector3D posAtom2 = positions->at(atom2);
					float distanceX = posAtom1[0] - posAtom2[0];
					float distanceY = posAtom1[1] - posAtom2[1];
					float distanceZ = posAtom1[2] - posAtom2[2];
					float interatomicDistance = distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ;

					//Perform distance test and ignore atoms with almost the same coordinates
					if ((interatomicDistance > cutoffPow2) || (interatomicDistance < 0.001)) {
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
					if (0.03 < interatomicDistance && interatomicDistance < radii && interatomicDistance < cutoffPow2) {
						result->push_back(atom1);
						result->push_back(atom2);
						bondsCount++;
					}
				}
			}
		}
	}

	return result->size();
}
