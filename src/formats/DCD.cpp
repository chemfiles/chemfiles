#include <cassert>
#include <cmath>
#include <cstdint>

#include <array>
#include <map>
#include <string>
#include <vector>

#include "chemfiles/error_fmt.hpp"
#include "chemfiles/external/optional.hpp"
#include "chemfiles/parse.hpp"
#include "chemfiles/string_view.hpp"
#include "chemfiles/types.hpp"
#include "chemfiles/utils.hpp"
#include "chemfiles/warnings.hpp"

#include "chemfiles/Atom.hpp"
#include "chemfiles/File.hpp"
#include "chemfiles/FormatMetadata.hpp"
#include "chemfiles/Frame.hpp"
#include "chemfiles/Property.hpp"
#include "chemfiles/Residue.hpp"
#include "chemfiles/Topology.hpp"
#include "chemfiles/UnitCell.hpp"

#include "chemfiles/files/DCDFile.hpp"
#include "chemfiles/formats/DCD.hpp"

using namespace chemfiles;
 
template<> const FormatMetadata& chemfiles::format_metadata<DCDFormat>() {
   static FormatMetadata metadata;
   metadata.name = "DCD";
   metadata.extension = ".dcd";
   metadata.description = "DCD () single precision binary FORTRAN format";
   metadata.reference = "";

   metadata.read = true;
   metadata.write = true;
   metadata.memory = true;

   metadata.positions = true;
   metadata.velocities = true;
   metadata.unit_cell = true;
   metadata.atoms = true;
   metadata.bonds = false;
   metadata.residues = true;
   return metadata;
}

static void set_positions(const std::vector<float>& x, const std::vector<float>& y,
   const std::vector<float>& z, Frame& frame);
static void get_positions(std::vector<float>& x, std::vector<float>& y,
   std::vector<float>& z, const Frame& frame);
static void set_cell(matrix box, Frame& frame);
static void get_cell(matrix box, const Frame& frame);

DCDFormat::DCDFormat(std::string path, File::Mode mode, File::Compression compression) {
   if (compression != File::DEFAULT)
      throw format_error("DCD format does not support compression");

   file_ = new DCDFile(path, mode);
}

void DCDFormat::read_step(size_t frameNum, Frame& frame)
{
   assert(frameNum < file_->getNumFrames());
   file_->setPos(frameNum);
   step_ = frameNum;
   read(frame);
}

void DCDFormat::read(Frame& frame)
{
   int natoms = file_->getNumAtoms();
   int md_step = step_;
   matrix box;

   std::vector<float> x;
   std::vector<float> y;
   std::vector<float> z;

   file_->readFrame(x, y, z);
   bool has_box = file_->hasCrystal();
   bool has_positions = file_->hasPositions();

   frame.set_step(static_cast<size_t>(md_step));         // actual step of MD Simulation
   frame.resize(static_cast<size_t>(natoms));

   if (has_box) {
      box[0][1] = file_->getPbc()[0];
      box[0][2] = file_->getPbc()[1];
      box[0][3] = file_->getPbc()[2];
      box[1][1] = file_->getPbc()[3];
      box[1][2] = file_->getPbc()[4];
      box[1][3] = file_->getPbc()[5];
      set_cell(box, frame);
   }
   if (has_positions) {
      frame.set("has_positions", true);
      set_positions(x, y, z, frame);
   }

   step_++;
}

void DCDFormat::write(const Frame& frame) {
   int natoms = frame.size();

   int md_step = static_cast<int>(frame.step());
   float lambda = static_cast<float>(frame.get("trr_lambda").value_or(0.0).as_double());

   matrix box;
   std::vector<float> x;
   std::vector<float> y;
   std::vector<float> z;
   bool has_box = frame.cell().shape() != UnitCell::INFINITE;
   if (has_box) {
      get_cell(box, frame);
   }
   if (frame.get("has_positions").value_or(true).as_bool()) {
      x.resize(natoms);
      y.resize(natoms);
      z.resize(natoms);
      get_positions(x, y, z, frame);
   }

   //write DCD header only at the begining of the trajectory
   if (step_ == 0)
   {
      //only coordinates DCD files are handled for now (velocities will be done later) 
      std::string type = "CORD";
      std::vector<int> properties(20, 0);
      properties[0] = static_cast<int>(frame.get("frames_number").value_or(0).as_double());
      properties[1] = static_cast<int>(frame.get("frames_number_before_start").value_or(0).as_double());
      properties[2] = static_cast<int>(frame.get("writing_frequency").value_or(0).as_double());
      properties[3] = static_cast<int>(frame.get("steps_number").value_or(0).as_double());
      properties[7] = static_cast<int>(frame.get("dof_number").value_or(0).as_double());
      properties[8] = static_cast<int>(frame.get("frozen_atoms").value_or(0).as_double());
      properties[9] = static_cast<int>(frame.get("AKMA_timestep").value_or(0).as_double());
      properties[10] = static_cast<int>(frame.get("crystal").value_or(0).as_double());
      properties[19] = static_cast<int>(frame.get("charmm_version").value_or(0).as_double());
      file_->setProperties(properties);
      file_->setHDR(type);
      file_->setNumTitles(1);
      file_->setTitle(frame.get("title").value_or(" ").as_string());
      file_->setNumAtoms(frame.size());
      file_->setNumFreeAtoms(frame.size() - properties[8]);
      if (properties[8] != 0)
      {
         //TODO: if frozen atoms are present in the trajectory, export only the free atoms
         //in the all the trajectory frames except for the first frame (where frozen and free atoms are exported) 
         //file_->setFreeAtoms();
      }
      file_->writeHeader();
   }

   file_->writeFrame(x, y, z);

   step_++;
}

void set_positions(const std::vector<float>& x, const std::vector<float>& y,
   const std::vector<float>& z, Frame& frame) {
   auto positions = frame.positions();
   assert(x.size() == positions.size());
   assert(y.size() == positions.size());
   assert(z.size() == positions.size());

   for (size_t i = 0; i < frame.size(); i++) {
      positions[i][0] = static_cast<double>(x[i]);
      positions[i][1] = static_cast<double>(y[i]);
      positions[i][2] = static_cast<double>(z[i]);
   }
}

void get_positions(std::vector<float>& x, std::vector<float>& y,
   std::vector<float>& z, const Frame& frame) {
   auto positions = frame.positions();
   assert(x.size() == positions.size());
   assert(y.size() == positions.size());
   assert(z.size() == positions.size());

   for (size_t i = 0; i < frame.size(); i++) {
      x[i] = static_cast<float>(positions[i][0]);
      y[i] = static_cast<float>(positions[i][1]);
      z[i] = static_cast<float>(positions[i][2]);
   }
}

void set_cell(matrix box, Frame& frame) {
   auto a = Vector3D(static_cast<double>(box[0][0]), static_cast<double>(box[0][1]),
      static_cast<double>(box[0][2]));
   auto b = Vector3D(static_cast<double>(box[1][0]), static_cast<double>(box[1][1]),
      static_cast<double>(box[1][2]));
   auto c = Vector3D(static_cast<double>(box[2][0]), static_cast<double>(box[2][1]),
      static_cast<double>(box[2][2]));

   auto angle = [](const Vector3D& u, const Vector3D& v) {
      constexpr double PI = 3.141592653589793238463;
      auto cos = dot(u, v) / (u.norm() * v.norm());
      cos = std::max(-1., std::min(1., cos));
      return acos(cos) * 180.0 / PI;
   };

   double alpha = angle(b, c);
   double beta = angle(a, c);
   double gamma = angle(a, b);

   auto matrix = Matrix3D(
      static_cast<double>(box[0][0]), static_cast<double>(box[1][0]), static_cast<double>(alpha),
      static_cast<double>(box[0][1]), static_cast<double>(box[1][1]), static_cast<double>(beta),
      static_cast<double>(box[0][2]), static_cast<double>(box[1][2]), static_cast<double>(gamma)
   );
   frame.set_cell(UnitCell(matrix));
}

void get_cell(matrix box, const Frame& frame) {
   auto matrix = frame.cell().matrix();
   box[0][0] = static_cast<float>(matrix[0][0]);
   box[0][1] = static_cast<float>(matrix[1][0]);
   box[0][2] = static_cast<float>(matrix[2][0]);
   box[1][0] = static_cast<float>(matrix[0][1]);
   box[1][1] = static_cast<float>(matrix[1][1]);
   box[1][2] = static_cast<float>(matrix[2][1]);
   box[2][0] = static_cast<float>(matrix[0][2]);
   box[2][1] = static_cast<float>(matrix[1][2]);
   box[2][2] = static_cast<float>(matrix[2][2]);
}

size_t DCDFormat::nsteps()
{
   return file_->getNumFrames();
}

