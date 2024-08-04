#ifndef EDITCOUNTER_H
#define EDITCOUNTER_H

#include "interfaces.h"

class EditCounter : public ConflictCounter
{
public:
  ConflictCounter::conflict_t count(const ProjectState& base,
				    const ProjectState& left,
				    const ProjectState& right) const override;
};

#endif//EDITCOUNTER_H
