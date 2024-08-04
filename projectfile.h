#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include "codeelement.h"
#include "projectid.h"

#include <vector>

class ProjectFile
{
public:
  using Container = std::vector<CodeElement>;
  using value_type = Container::value_type;
  using const_iterator = Container::const_iterator;
  using iterator = Container::iterator;
  using size_type = Container::size_type;

  ProjectFile(): _id(nextId()), _elements() {}
  explicit ProjectFile(const Container& elements);
  ProjectFile(id_t id, const Container& elements = Container());

  id_t id() const { return _id; }

  size_type size() const { return _elements.size(); }
  value_type& operator[](size_t i);
  const value_type& operator[](size_t i) const;
  value_type& at(size_t i);
  const value_type& at(size_t i) const;

  iterator find(id_t id);
  const_iterator find(id_t id) const;

  void appendElement(id_t id);
  void appendElement(const CodeElement& ce);
  // move elements
  void mutateElement(id_t id);
  void mutateElement(iterator it);
  // remove elements

  iterator begin() { return _elements.begin(); }
  const_iterator cbegin() const { return _elements.cbegin(); }
  iterator end() { return _elements.end(); }
  const_iterator cend() const { return _elements.cend(); }

  private:
  id_t _id;
  Container _elements;
};


#endif//PROJECTFILE_H
