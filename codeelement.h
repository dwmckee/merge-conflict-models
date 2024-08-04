#ifndef CODEELEMENT_H
#define CODEELEMENT_H

#include "projectid.h"

#include <cstdint>

class CodeElement
{
public:
  CodeElement();
  explicit CodeElement(const id_t id);

  bool operator==(const CodeElement &that) const = default;

  friend bool operator<(const CodeElement &lhs, const CodeElement &rhs);
  
  id_t id() const { return _id; }
  
private:
    id_t _id;
};

#endif//CODEELEMENT_H


