#ifndef CHEMFILES_FORMAT_DCD_HPP
#define CHEMFILES_FORMAT_DCD_HPP

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "chemfiles/File.hpp"
#include "chemfiles/Format.hpp"
#include "chemfiles/files/DCDFile.hpp"

using matrix = float[3][3];

namespace chemfiles {
   class Frame;

   /// [DCD][DCD] file format reader and writer.
   ///
   /// [DCD]: 
   class DCDFormat final : public Format {
   public:
      // public methods
      DCDFormat(std::string path, File::Mode mode, File::Compression compression);

      void read_step(size_t step, Frame& frame) override;
      void read(Frame& frame) override;
      void write(const Frame& frame) override;
      size_t nsteps() override;
  
   private:
      /// Associated dcd file
      DCDFile* file_;
      /// The next step to read
      size_t step_ = 0;
   };

   template<> FormatInfo format_information<DCDFormat>();
} // namespace chemfiles

#endif
