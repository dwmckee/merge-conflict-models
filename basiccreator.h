#ifndef BASICCREATOR_H
#define BASICCREATOR_H

#include "interfaces.h"

#include <random>
#include <utility>

// Will distribute exactly nElements elements uniformaly among exactly nFiles
// files.
class BasicCreator : public ProjectCreator
{
public:
  BasicCreator(id_t nFiles = 100, id_t nElements = 10000);

  ProjectState create() const override;

  std::pair<double, double> mu_sigma() const;

private:
  id_t _nFiles;
  id_t _nElements;
};

#endif//BASICCREATOR_H
