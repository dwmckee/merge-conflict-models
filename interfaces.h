#ifndef INTERFACES_H
#define INTERFACES_H

#include <memory>

class ProjectFile;
class ProjectState;

class ProjectCreator
{
public:
  virtual ProjectState create() const = 0;
};


class ProjectMutator
{
public:
  virtual ProjectState mutate(const ProjectState &ps) const = 0;
};

struct ConflictCount {
  id_t baseFiles = 0; 
  id_t baseElements = 0;
  id_t elementChanges = 0; // should be baseFile * 2 * changeFraction
  id_t elementConflicts = 0;
  id_t fileConflicts = 0;
  id_t manualResolutions = 0;
  id_t maxFileSize = 0;
  
  ConflictCount () = default;
  ConflictCount (const ConflictCount &that) = default;
  ~ConflictCount () = default;
  ConflictCount & operator=(const ConflictCount &that) = default;
  ConflictCount operator+(const ConflictCount &that) const;
  ConflictCount & operator+=(const ConflictCount &that);
};

class ConflictCounter
{
public:
  using conflict_t = ConflictCount;
  virtual conflict_t count(const ProjectState& base,
			   const ProjectState& left,
			   const ProjectState& right) const = 0;
};



#endif//INTERFACES_H
