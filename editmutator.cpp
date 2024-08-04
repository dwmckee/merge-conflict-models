#include "editmutator.h"

#include "basiccreator.h"
#include "codeelement.h"
#include "editcounter.h"
#include "projectid.h"
#include "projectstate.h"

#include <doctest.h>

#include <algorithm>
#include <numeric>
#include <random>

ProjectState EditMutator::mutate(const ProjectState& base) const
{
  ProjectState ps = base;

  //Randomness for this run
  std::random_device rd;
  std::mt19937 gen(rd());

  // Count elements and build a lookup table of file starts
  id_t elementCount = 0;
  std::vector<id_t> thing{0_id}; // Sentinel for finding element indexes
  for (auto i = 0_id; i < ps.size(); ++i) {
    elementCount += ps[i].size();
    thing.push_back(elementCount);
  }

  // Select elements at random across the project, find their file and index
  // by consulting out table, and replace the selected element.
  std::uniform_int_distribution<id_t> d(0_id,elementCount-1);
  for (auto i = 0_id; i < _nChanges; ++i)
    {
      const auto idx = d(gen);
      const auto it = std::upper_bound(thing.begin(), thing.end(), idx);
      const auto fileNum = it - thing.begin() - 1;
      const auto prev = std::prev(it); // Should be safe because of the sentinel
      const auto elementNum = idx - (*prev);
      ps[fileNum][elementNum] = CodeElement();
    }
	
  return ps;
};

TEST_CASE("Mutation preserves number of file and number of elements")
{
  for (id_t i = 0; i < 10; ++i)
    {
      ProjectState orig = ProjectState();
      for (id_t i = 0; i < 100_id; ++i)
	{
	  auto & pf = orig.addFile(nextId());
	  for (id_t j=0; j<100_id; ++j)
	    pf.appendElement(nextId());
      }
      const auto mutator = EditMutator(5_id);
      const ProjectState changed = mutator.mutate(orig);
		       
      // This mutator should neither change the number of files in a project,
      // nor the number of elements in a file
      CHECK(changed.size() == orig.size());

      // But should result in _nChanges elements differing across the whole
      // project
      id_t changeCount=0;
      for (id_t i=0; i<orig.size(); ++i)
	{
          const auto of = orig[i];
          const auto cf = changed[i];
	  CHECK(cf.id() == of.id());
          CHECK(cf.size() == of.size());
	  changeCount +=
	    std::inner_product(
			       of.cbegin(), of.cend(), cf.cbegin(), 0_id,
			       [](id_t sum, bool mismatch) -> id_t { return sum + static_cast<id_t>(mismatch); },
			       [](CodeElement lhs, CodeElement rhs) -> bool { return lhs != rhs; });
      }
      // This test may fail occasionally, because there is a chance that some
      // elements might get changed more than once, resulting in fewer total
      // changed elements. Odds are about 1/1000 for each pass through the loop
      // or 1% in total.
      CHECK(changeCount == 5_id); 
    }
}
