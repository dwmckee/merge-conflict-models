#include "codeelement.h"

#include "projectid.h"

#include <doctest.h>

#include <functional>
#include <set>


CodeElement::CodeElement() : _id(nextId()) {}
CodeElement::CodeElement(const id_t id) : _id(id) {}

bool operator<(const CodeElement &lhs, const CodeElement &rhs)
{
  return lhs._id < rhs._id;
}


TEST_CASE("CodeElement")
{

  const CodeElement ce1;
  const CodeElement ce2(42u);

  SUBCASE("Equality")
    {
       CHECK(ce1 == ce1);
       CHECK(ce1 != ce2);
       CHECK(ce2 == ce2);
    }

  SUBCASE("Assignment and Copy Construction")
    {
      const CodeElement ce3;
      CodeElement ce4;
      const CodeElement ce5(ce4);
      CHECK(ce4 != ce3);
      CHECK(ce4 == ce5);
      ce4 = ce3;
      CHECK(ce4 == ce3);
      CHECK(ce4 != ce5);
    }

  SUBCASE("Inequality")
    {
      const CodeElement ce6;
      const CodeElement ce7(ce6.id()+1);
      CHECK(!(ce6 < ce6));
      CHECK(ce6 < ce7);
      CHECK(!(ce7 < ce6));
    }

  std::set<CodeElement> s;
  CHECK(s.size() == 0);
  s.insert(ce1);
  s.insert(ce2);
  CHECK(s.size() == 2);

}
