#ifndef PROJECTSTATE_H
#define PROJECTSTATE_H

#include "projectfile.h"

#include <utility>

class ProjectState
{
public:
  using Container = std::vector<ProjectFile>;
  using value_type = Container::value_type;
  using iterator = Container::iterator;
  using const_iterator = Container::const_iterator;
  using size_type = Container::size_type;
  
  ProjectState();

    size_type size() const { return _files.size(); }
  value_type& operator[](size_t i);
  const value_type& operator[](size_t i) const;
  value_type& at(size_t i);
  const value_type& at(size_t i) const;

  iterator find(id_t id);
  const_iterator find(id_t id) const;

  ProjectFile& addFile(id_t id);

  std::pair<id_t, id_t> fileSizeLimits() const;
  
  iterator begin() { return _files.begin(); }
  const_iterator begin() const { return _files.cbegin(); }
  const_iterator cbegin() const { return _files.cbegin(); }
  iterator end() { return _files.end(); }
  const_iterator end() const { return _files.cend(); }
  const_iterator cend() const { return _files.cend(); }
  
private:
  Container _files;
};

#endif//PROJECTSTATE_H
