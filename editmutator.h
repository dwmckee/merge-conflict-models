#ifndef EDITMUTATOR_H
#define EDITMUTATOR_H

#include "interfaces.h"
#include "projectid.h"

class ProjectState;

class EditMutator : public ProjectMutator
{
public:
 EditMutator(id_t nChanges) : _nChanges(nChanges) {}
  ~EditMutator() = default;

  ProjectState mutate(const ProjectState& base) const override;
  
 private:
  id_t _nChanges;
};

#endif//EDITMUTATOR_H
