#include "projectfile.h"
#include "codeelement.h"
#include "projectid.h"

#include <doctest.h>

#include <algorithm>
#include <stdexcept>

ProjectFile::ProjectFile(const Container& elements)
  : _id(nextId())
    , _elements(elements)
{
}

ProjectFile::ProjectFile(id_t id, const Container& elements)
    : _id(id)
    , _elements(elements)
{
}

auto ProjectFile::operator[](size_t i) -> value_type&
{
  return _elements[i];
}

auto ProjectFile::operator[](size_t i) const -> const value_type&
{
  return _elements[i];
}

auto ProjectFile::at(size_t i) -> value_type&
{
  return _elements.at(i);
}

auto ProjectFile::at(size_t i) const -> const value_type&
{
  return _elements.at(i);
}

auto ProjectFile::find(id_t id) -> iterator
{
  return std::find_if(begin(), end(),
		      [&id](const auto &ce){ return ce.id() == id; });
}

auto ProjectFile::find(id_t id) const -> const_iterator
{
  return std::find_if(cbegin(),cend(),
		      [&id](const auto &ce){ return ce.id() == id; });
}

void ProjectFile::appendElement(id_t id)
{
  _elements.emplace_back(id);
}

  void ProjectFile::appendElement(const CodeElement& ce)
{
  _elements.push_back(ce);
}

void ProjectFile::mutateElement(id_t id)
{
    iterator it = find(id);
    if (it == _elements.end())
        throw std::out_of_range("Element not found");
    return mutateElement(it);
}

void ProjectFile::mutateElement(iterator it)
{
  *it = CodeElement(); // As simple as getting a new id
}


TEST_CASE("ProjectFile")
{
    ProjectFile pf;
    CHECK(pf.size() == 0);

    SUBCASE("Appending increases size")
    {
        for (int i = 0; i < 3; ++i)
	  {
	    const auto iSize = pf.size();
	    pf.appendElement(CodeElement());
	    CHECK(pf.size() == iSize + 1);
        }
    }

    SUBCASE("Mutate leaves size unchanged, and changes elements")
    {
      const auto iSize = pf.size();
      for (auto it = pf.begin(); it != pf.end(); ++it)
	{
	  const auto ce = *it;
	  pf.mutateElement(it);
          CHECK(pf.size() == iSize);
	  CHECK(*it != ce);
      }
    }
    
}
