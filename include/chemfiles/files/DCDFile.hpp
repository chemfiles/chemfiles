#ifndef CHEMFILES_DCD_FILE_HPP
#define CHEMFILES_DCD_FILE_HPP

#include <cstdint>
#include <string>

#include "chemfiles/File.hpp"

namespace chemfiles {

   /// Handle the creation and destruction of the file as needed.
   /// Reads the file header and stores the offsets for individual frames.
   class DCDFile {
   public:

      DCDFile(std::string path, File::Mode mode);
      ~DCDFile();

      void                          readHeader();
      void                          readFrame(std::vector<float>& X, std::vector<float>& Y,
                                                std::vector<float>& Z, int toFrame = NULL);
      void                          writeHeader();
      void                          writeFrame(std::vector<float>& x, std::vector<float>& y, 
                                                std::vector<float>& z);

      void                          setHDR(std::string hdr);
      void                          setProperties(std::vector<int>& properties);

      int                           getNumFrames() const;
      
      //Information on the periodic boundary conditions of the system
      bool                          hasCrystal() const;
      const double*                 getPbc() const;
      void                          setPbc(double* pbc);

      //
      bool                          hasFrozenAtoms();
      int                           getNumFreeAtoms() const;
      void                          setNumFreeAtoms(int freeAtomsNum);
      void                          setFreeAtoms(std::vector<int>& freeAtomsIndexes);
      
      //Number of atoms in the system 
      int                           getNumAtoms() const;
      void                          setNumAtoms(size_t atomNum);

      bool                          hasPositions() const;
      
      //Navigate in the file stream
      virtual size_t                getPos();
      void                          setPos(size_t frameNum);

      //Read title information 
      int                           getNumTitles();
      void                          setNumTitles(int titlesNumber);
      char*                         getTitle();
      void                          setTitle(std::string title);
      
   protected:
      
      std::fstream                  fileStream;    //File stream for opening and reading dcd file
      
      //content of ICNTRL : non detailed ones are 0
      //ICNTRL(0)  number of frames in this dcd
      //ICNTRL(1)  if restart, total number of frames before first print
      //ICNTRL(2)  frequency of writting dcd
      //ICNTRL(3)  number of steps ; note that NSTEP/NSAVC = numFrames
      //ICNTRL(7)  number of degrees of freedom
      //ICNTRL(8)  is numAtoms - NumberOfFreeAtoms : it is the number of frozen (i.e. not moving atoms)
      //ICNTRL(9) timestep in AKMA units but stored as a 32 bits integer !!!
      //ICNTRL(10) is 1 if CRYSTAL used
      //ICNTRL(19) is charmm version
      int                           ICNTRL[20];
      char                          HDR[5];    //CORD (coordinates) or VEL (velocities). Velocities not supported yet
      
      int                           NTITLE;        //Number of "title lines" in the dcd file
      char*                         TITLE;         //Each "title line" is 80 char long.
      double                        pbc[6];        //6 real matrix defining the periodic boundary conditions : only useful if QCRYS is not 0

      int                           numAtoms;      // Number of atoms
      int                           numFreeAtoms;  // Number of free (moving) atoms
      int*                          freeAtoms;     // Array storing indexes of moving atoms
      long                          headByteNum;   //Bytes number of the DCD header
      int                           frameNumber = 0;
      int                           frameNumAtoms;

   private:
      bool                          checkFortranIOerror(const char file[], const int line,
                                             const size_t fortcheck1, const size_t fortcheck2) const;
   };
} // namespace chemfiles
#endif
