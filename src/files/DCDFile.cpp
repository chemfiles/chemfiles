#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>

#include "chemfiles/File.hpp"
#include "chemfiles/files/DCDFile.hpp"

#include "chemfiles/error_fmt.hpp"

using namespace std;
using namespace chemfiles;

DCDFile::DCDFile(std::string path, File::Mode mode)
{
   const char* openmode;
   if (mode == File::READ) {
      openmode = "r";
      fileStream.exceptions(ifstream::failbit);
      try
      {
         fileStream.open(path, ios::in | ios::binary);
      }
      catch (ifstream::failure& e)
      {
         cerr << "Exception opening/reading file '" << path << "' : " << endl;
         cerr << "Please chech the path of the file and if it exists." << endl;
      }
      //Read dcd file header 
      readHeader();
   }
   else if (mode == File::WRITE) {
      openmode = "w";
      fileStream.exceptions(ofstream::failbit);
      try
      {
         fileStream.open(path, ios::app | ios::binary);
      }
      catch (ofstream::failure& e)
      {
         cerr << "Exception creating file '" << path << "' : " << endl;
         cerr << "Couldn't create the file." << endl;
      }

   }
}

void DCDFile::readHeader()
{
   unsigned int fortcheck1, fortcheck2;
   long initialPos = fileStream.tellg();
   //This is the trick for reading binary data from fortran file, see the method checkFortranIOerror for more details.
   fileStream.read((char*)&fortcheck1, sizeof(unsigned int));               //consistency check 1
   fileStream.read((char*)HDR, sizeof(char) * 4);                       //first data block written by fortran  : a character array of length 4.
   fileStream.read((char*)ICNTRL, sizeof(int) * 20);                    //second data block written by fortran : an integer(4) array of length 20.
   fileStream.read((char*)&fortcheck2, sizeof(unsigned int));         //consistency check 2

   // if the 2 unsigned ints have a different value there was an error
   if (!checkFortranIOerror(__FILE__, __LINE__, fortcheck1, fortcheck2))
   {
      return;
   }

   HDR[4] = '\0';

   if (ICNTRL[10])
   {
      pbc[0] = pbc[1] = pbc[2] = pbc[3] = pbc[4] = pbc[5] = 0.0;
   }

   /* Several "lines" of title of length 80 are written to the dcd file by CHARMM */
   fileStream.read((char*)&fortcheck1, sizeof(unsigned int));
   fileStream.read((char*)&NTITLE, sizeof(int));
   if (NTITLE == 0)
   {
      TITLE = new char[80 + 1];
      TITLE[0] = '\0';
   }
   else
   {
      TITLE = new char[NTITLE * 80 + 1];
      for (int it = 0; it < NTITLE; it++)
      {
         fileStream.read((char*)&TITLE[it * 80], sizeof(char) * 80);
      }
      TITLE[NTITLE * 80] = '\0';
   }
   fileStream.read((char*)&fortcheck2, sizeof(unsigned int));
   if (!checkFortranIOerror(__FILE__, __LINE__, fortcheck1, fortcheck2))
   {
      return;
   }

   // reading number of atoms
   fileStream.read((char*)&fortcheck1, sizeof(unsigned int));
   fileStream.read((char*)&numAtoms, sizeof(int));
   fileStream.read((char*)&fortcheck2, sizeof(unsigned int));
   if (!checkFortranIOerror(__FILE__, __LINE__, fortcheck1, fortcheck2))
   {
      return;
   }

   /* If some atoms of the MD or MC simulation are frozen (i.e. never moving ) it is useless to store their coordinates more than once.
    * In that case a list of Free atoms (moving ones) is written at the end of the header part of the dcd.
    * See ReadDcd::read_firstFrame() for more details.
    */
   numFreeAtoms = numAtoms - ICNTRL[8];
   if (numFreeAtoms != numAtoms)
   {
      freeAtoms = new int[numFreeAtoms];
      fileStream.read((char*)&fortcheck1, sizeof(unsigned int));
      fileStream.read((char*)freeAtoms, sizeof(int) * numFreeAtoms);
      fileStream.read((char*)&fortcheck2, sizeof(unsigned int));
      if (!checkFortranIOerror(__FILE__, __LINE__, fortcheck1, fortcheck2))
      {
         return;
      }
   }

   headByteNum = std::abs(initialPos - fileStream.tellg());
}

void DCDFile::writeHeader()
{
   if (fileStream.is_open())
   {
      unsigned int icntrlBlockSize = (sizeof(char) * 4) + (sizeof(int) * 20);
      fileStream.write((char*)&icntrlBlockSize, sizeof(unsigned int));
      fileStream.write((char*)&HDR, sizeof(char) * 4);                //first data block written by fortran  : a character array of length 4.
      fileStream.write((char*)ICNTRL, sizeof(int) * 20);                    //second data block written by fortran : an integer(4) array of length 20.
      fileStream.write((char*)&icntrlBlockSize, sizeof(unsigned int));


      /* Several "lines" of title of length 80 are written to the dcd file by CHARMM */
      unsigned int titlesBlockSize = sizeof(int) + (NTITLE * 80) + 1;
      fileStream.write((char*)&titlesBlockSize, sizeof(unsigned int));
      fileStream.write((char*)&NTITLE, sizeof(int));
      for (int it = 0; it < NTITLE; it++)
      {
         fileStream.write((char*)&TITLE[it * 80], sizeof(char) * 80);
      }
      TITLE[NTITLE * 80] = '\0';
      fileStream.write((char*)&titlesBlockSize, sizeof(unsigned int));

      // writing number of atoms
      unsigned int atomsNumBlockSize = numAtoms * sizeof(int);
      fileStream.write((char*)&atomsNumBlockSize, sizeof(unsigned int));
      fileStream.write((char*)&numAtoms, sizeof(int));
      fileStream.write((char*)&atomsNumBlockSize, sizeof(unsigned int));

      /* If some atoms of the MD or MC simulation are frozen (i.e. never moving ) it is useless to store their coordinates more than once.
       * In that case a list of Free atoms (moving ones) is written at the end of the header part of the dcd.
       */
      if (numFreeAtoms != numAtoms)
      {
         unsigned int freeAtomsBlockSize = numFreeAtoms * sizeof(int);
         fileStream.write((char*)&freeAtomsBlockSize, sizeof(unsigned int));
         fileStream.write((char*)freeAtoms, sizeof(int) * numFreeAtoms);
         fileStream.write((char*)&freeAtomsBlockSize, sizeof(unsigned int));
      }
   }
}

void DCDFile::readFrame(std::vector<float>& X, std::vector<float>& Y,
   std::vector<float>& Z, int toFrame)
{
   //dcd files store all the atoms in the first frame (even if there are some frozen atoms)
   //and only store free atoms in the other frames
   int frameAtmNum;
   if (ICNTRL[8] != 0 && frameNumber != 0)
   {
      frameAtmNum = numAtoms - ICNTRL[8];
   }
   else
   {
      frameAtmNum = numAtoms;
   }

   X.reserve(frameAtmNum);
   Y.reserve(frameAtmNum);
   Z.reserve(frameAtmNum);

   unsigned int fortcheck1, fortcheck2;

   float* tmpX = new float[frameAtmNum];
   float* tmpY = new float[frameAtmNum];
   float* tmpZ = new float[frameAtmNum];

   if (ICNTRL[10])
   {
      fileStream.read((char*)&fortcheck1, sizeof(unsigned int));
      fileStream.read((char*)pbc, sizeof(double) * 6);
      fileStream.read((char*)&fortcheck2, sizeof(unsigned int));
      checkFortranIOerror(__FILE__, __LINE__, fortcheck1, fortcheck2);
   }

   // X
   fileStream.read((char*)&fortcheck1, sizeof(unsigned int));
   fileStream.read((char*)tmpX, sizeof(float) * frameAtmNum);
   fileStream.read((char*)&fortcheck2, sizeof(unsigned int));
   checkFortranIOerror(__FILE__, __LINE__, fortcheck1, fortcheck2);

   // Y
   fileStream.read((char*)&fortcheck1, sizeof(unsigned int));
   fileStream.read((char*)tmpY, sizeof(float) * frameAtmNum);
   fileStream.read((char*)&fortcheck2, sizeof(unsigned int));
   checkFortranIOerror(__FILE__, __LINE__, fortcheck1, fortcheck2);

   // Z
   fileStream.read((char*)&fortcheck1, sizeof(unsigned int));
   fileStream.read((char*)tmpZ, sizeof(float) * frameAtmNum);
   fileStream.read((char*)&fortcheck2, sizeof(unsigned int));
   checkFortranIOerror(__FILE__, __LINE__, fortcheck1, fortcheck2);

   for (int it = 0; it < frameAtmNum; it++)
   {
      X.emplace_back(tmpX[it]);
      Y.emplace_back(tmpY[it]);
      Z.emplace_back(tmpZ[it]);
   }

   delete[] tmpX;
   delete[] tmpY;
   delete[] tmpZ;
   frameNumber++;
}

void DCDFile::writeFrame(std::vector<float>& x, std::vector<float>& y,
   std::vector<float>& z)
{
   if (fileStream.is_open())
   {
      assert((x.size() == y.size()) && (y.size() == z.size()));

      float* tmpX = new float[x.size()];
      float* tmpY = new float[y.size()];
      float* tmpZ = new float[z.size()];

      //fill tmp pointer
      for (size_t coord = 0; coord < x.size(); coord++)
      {
         tmpX[coord] = x[coord];
         tmpY[coord] = y[coord];
         tmpZ[coord] = z[coord];
      }

      if (ICNTRL[10])
      {
         unsigned int blockSize = sizeof(double) * 6;
         fileStream.write((char*)&blockSize, sizeof(unsigned int));
         fileStream.write((char*)pbc, sizeof(double) * 6);
         fileStream.write((char*)&blockSize, sizeof(unsigned int));
      }

      // X
      unsigned int xCoorBlock = sizeof(float) * x.size();
      fileStream.write((char*)&xCoorBlock, sizeof(unsigned int));
      fileStream.write((char*)tmpX, sizeof(float) * x.size());
      fileStream.write((char*)&xCoorBlock, sizeof(unsigned int));

      // Y
      unsigned int yCoorBlock = sizeof(float) * y.size();
      fileStream.write((char*)&yCoorBlock, sizeof(unsigned int));
      fileStream.write((char*)tmpY, sizeof(float) * y.size());
      fileStream.write((char*)&yCoorBlock, sizeof(unsigned int));

      // Z
      unsigned int zCoorBlock = sizeof(float) * z.size();
      fileStream.write((char*)&zCoorBlock, sizeof(unsigned int));
      fileStream.write((char*)tmpZ, sizeof(float) * z.size());
      fileStream.write((char*)&zCoorBlock, sizeof(unsigned int));

      delete[] tmpX;
      delete[] tmpY;
      delete[] tmpZ;
   }
}

void DCDFile::setHDR(std::string hdr)
{
   if (hdr.size() == 4)
   {
      char* hdrChar = const_cast<char*>(hdr.c_str());
      for (size_t ch = 0; ch < hdr.size(); ch++)
      {
         HDR[ch] = hdrChar[ch];
      }
      HDR[4] = '\0';
   }
}

void DCDFile::setProperties(std::vector<int>& properties) {
   if (properties.size() == 20)
   {
      for (size_t prop = 0; prop < properties.size(); prop++)
      {
         ICNTRL[prop] = properties[prop];
      }
   }
}

int DCDFile::getNumFrames() const {
   return ICNTRL[0];
}

bool DCDFile::hasCrystal() const
{
   if (ICNTRL[10])
      return true;
   return false;
}

const double* DCDFile::getPbc() const
{
   return pbc;
}

void DCDFile::setPbc(double* periodicBoundaryConditions)
{
   pbc[0] = periodicBoundaryConditions[0];
   pbc[1] = periodicBoundaryConditions[1];
   pbc[2] = periodicBoundaryConditions[2];
   pbc[3] = periodicBoundaryConditions[3];
   pbc[4] = periodicBoundaryConditions[4];
   pbc[5] = periodicBoundaryConditions[5];
}

size_t DCDFile::getPos()
{
   return fileStream.tellg();
}

void DCDFile::setPos(size_t frameNum)
{
   long frameOffset;
   if (frameNum != 0)
   {
      if (hasFrozenAtoms())
         frameOffset = (frameNum - 1) * (((sizeof(unsigned int)) + (sizeof(float) * numFreeAtoms) + (sizeof(unsigned int))) * 3);
      else
         frameOffset = (frameNum - 1) * (((sizeof(unsigned int)) + (sizeof(float) * numAtoms) + (sizeof(unsigned int))) * 3);

      if (hasCrystal())
         frameOffset += (frameNum - 1) * ((sizeof(unsigned int)) + (sizeof(double) * 6) + (sizeof(unsigned int)));

      fileStream.seekg(headByteNum + frameOffset);
   }
   else
   {
      fileStream.seekg(headByteNum);
   }
}

bool DCDFile::hasFrozenAtoms()
{
   if (numAtoms != numFreeAtoms)
      return true;
   return false;
}

int DCDFile::getNumAtoms() const
{
   return numAtoms;
}

void DCDFile::setNumAtoms(size_t atomNum)
{
   numAtoms = atomNum;
}

int DCDFile::getNumFreeAtoms() const
{
   return numFreeAtoms;
}

void DCDFile::setNumFreeAtoms(int freeAtomsNum)
{
   numFreeAtoms = freeAtomsNum;
}

void DCDFile::setFreeAtoms(std::vector<int>& freeAtomsIndexes)
{
   assert(numFreeAtoms == freeAtomsIndexes.size());
   freeAtoms = new int[numFreeAtoms];
   for (size_t atmIdx = 0; atmIdx < freeAtomsIndexes.size(); atmIdx++)
   {
      freeAtoms[atmIdx] = freeAtomsIndexes[atmIdx];
   }
}

bool DCDFile::hasPositions() const
{
   if (getNumAtoms() != 0)
   {
      return true;
   }

   return false;
}

int DCDFile::getNumTitles() {
   return NTITLE;
}

void DCDFile::setNumTitles(int titlesNumber) {
   NTITLE = titlesNumber;
}

char* DCDFile::getTitle()
{
   return TITLE;
}

void DCDFile::setTitle(std::string title)
{
   TITLE = new char[NTITLE * 80 + 1];
   char* allTitle = const_cast<char*>(title.c_str());
   for (size_t ch = 0; ch < title.size(); ch++)
   {
      TITLE[ch] = allTitle[ch];
   }
   TITLE[NTITLE * 80] = '\0';
}

/*
 * When writing a block of binary data to a file, Fortran adds 2 unsigned integer before and after the block : each one contains the total number of bytes of the block.
 *
 * For example for a fortran array of real(8) of size 10, 80 bytes of a data have to be written : so 3 things are written to the binary file :
 *  1) An unsigned integer (4 bytes) , its value is 80
 *  2) The fortran array (80 bytes)
 *  3) A second unsigned integer (4 bytes), same value of 80.
 * This corresponds to the fortran statement "write(file_descriptor) fortran_array"
 *
 * Now when writing several things at the same tine, i.e. "write(file_descriptor) fortran_array,an_integer"
 * things are written as following:
 * 1) unsigned int size
 * 2) fortran array
 * 3) an_integer
 * 4) unsigned int size
 * The value of the 2 unsigned ints is 84 : 80 for the array, 4 for the integer(4).
 *
 * The following method checkFortranIOerror check that the 2 unsigned ints have the same value: if not there was a probem when reading the binary fortran file.
 */
bool DCDFile::checkFortranIOerror(const char file[], const int line, const size_t fortcheck1, const size_t fortcheck2) const
{
   if (fortcheck1 != fortcheck2)
   {
      cout << "Error when reading data from dcd : quantities do not match." << endl;
      cout << "fortcheck1 = " << fortcheck1 << " and fortcheck2 = " << fortcheck2 << endl;
      cout << "in File " << file << " at Line " << line << endl;
      return false;
   }
   return true;
}
