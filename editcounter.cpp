#include "editcounter.h"
#include "codeelement.h"
#include "interfaces.h"
#include "projectstate.h"

#include <doctest.h>

#include <iostream>
#include <stdexcept>

ConflictCount countElements(const CodeElement& base,
			    const CodeElement& left,
			    const CodeElement& right)
{
    ConflictCount cc;

    const bool leftChanged = left != base;
    const bool rightChanged = right != base;

    cc.baseElements = 1;
    cc.elementChanges += id_t(leftChanged) + id_t(rightChanged);
    cc.elementConflicts += id_t(leftChanged && rightChanged);

    return cc;
}

TEST_CASE("Individual edits are recognized correctly")
{
  SUBCASE("no change")
    {
      const ConflictCount cc1 = countElements(CodeElement(0),
					      CodeElement(0),
					      CodeElement(0));
      CHECK(cc1.baseFiles == 0);
      CHECK(cc1.baseElements == 1);
      CHECK(cc1.elementChanges == 0);
      CHECK(cc1.elementConflicts == 0);
      CHECK(cc1.fileConflicts == 0);
      CHECK(cc1.manualResolutions == 0);
    }
  
  SUBCASE("One change; no conflict")
    {
      const ConflictCount cc2 = countElements(CodeElement(0),
					      CodeElement(0),
					      CodeElement(1));
      CHECK(cc2.baseElements == 1);
      CHECK(cc2.elementChanges == 1);
      CHECK(cc2.elementConflicts == 0);
      
      const ConflictCount cc3 = countElements(CodeElement(0),
					      CodeElement(1),
					      CodeElement(0));
      CHECK(cc3.baseElements == 1);
      CHECK(cc3.elementChanges == 1);
      CHECK(cc3.elementConflicts == 0);
    }
  
  SUBCASE("two changes; conflict")
    {
      const ConflictCount cc4 = countElements(CodeElement(0),
					      CodeElement(1),
					      CodeElement(2));
      CHECK(cc4.baseElements == 1);
      CHECK(cc4.elementChanges == 2);
      CHECK(cc4.elementConflicts == 1);
    }
  
  SUBCASE("identical changes")
    {
      const ConflictCount cc5 = countElements(CodeElement(0),
					      CodeElement(3),
					      CodeElement(3));
      CHECK(cc5.baseElements == 1);
      CHECK(cc5.elementChanges == 2);
      CHECK(cc5.elementConflicts == 1);
    }
}

ConflictCount countFiles(const ProjectFile& base,
			 const ProjectFile& left,
			 const ProjectFile& right)
{
    ConflictCount cc;
    cc.baseFiles = 1;
    cc.maxFileSize = base.size();
    
    bool fileHasConflict = false;
    id_t fileTotalChanges = 0; // All file conflicts will need manual
                               // resolution if any conflict occurs
    // Elements are expected to be in order, so test that
    for (id_t i = 0; i<base.size(); ++i)
      {
	const auto &bElement = base.at(i);
	const auto &lElement = left.at(i);
	const auto &rElement = right.at(i);
	
	const auto eCC = countElements(bElement, lElement, rElement);
	cc += eCC;

	fileTotalChanges += eCC.elementChanges - eCC.elementConflicts;
        fileHasConflict |= (eCC.elementConflicts > 0);
      }
    if (fileHasConflict) {
      ++cc.fileConflicts;
      cc.manualResolutions += fileTotalChanges;
    }

    return cc;
}

TEST_CASE("File edits total correctly")
{
  SUBCASE("empty files")
    {  
      ConflictCount cc1 = countFiles(ProjectFile(), ProjectFile(), ProjectFile());
      CHECK(cc1.baseFiles == 1);
      CHECK(cc1.baseElements == 0);
      CHECK(cc1.elementChanges == 0);
      CHECK(cc1.elementConflicts == 0);
      CHECK(cc1.fileConflicts == 0);
      CHECK(cc1.manualResolutions == 0);
      CHECK(cc1.maxFileSize == 0);
      
      ConflictCount cc2 = countFiles(ProjectFile({}),
				     ProjectFile({}),
				     ProjectFile({}));
      CHECK(cc2.baseFiles == 1);
      CHECK(cc2.baseElements == 0);
      CHECK(cc2.elementChanges == 0);
      CHECK(cc2.elementConflicts == 0);
      CHECK(cc2.fileConflicts == 0);
      CHECK(cc2.manualResolutions == 0);
      CHECK(cc2.maxFileSize == 0);
    }
    
  SUBCASE("single element")
    {  
      ConflictCount cc3 = countFiles(ProjectFile({CodeElement(0)}),
				     ProjectFile({CodeElement(0)}),
				     ProjectFile({CodeElement(0)}));
      CHECK(cc3.baseFiles == 1);
      CHECK(cc3.baseElements == 1);
      CHECK(cc3.elementChanges == 0);
      CHECK(cc3.elementConflicts == 0);
      CHECK(cc3.fileConflicts == 0);
      CHECK(cc3.manualResolutions == 0);
      CHECK(cc3.maxFileSize == 1);

      ConflictCount cc4 = countFiles(ProjectFile({CodeElement(0)}),
				     ProjectFile({CodeElement(1)}),
				     ProjectFile({CodeElement(0)}));
      CHECK(cc4.baseFiles == 1);
      CHECK(cc4.baseElements == 1);
      CHECK(cc4.elementChanges == 1);
      CHECK(cc4.elementConflicts == 0);
      CHECK(cc4.fileConflicts == 0);
      CHECK(cc4.manualResolutions == 0);

      ConflictCount cc5 = countFiles(ProjectFile({CodeElement(0)}),
				     ProjectFile({CodeElement(1)}),
				     ProjectFile({CodeElement(2)}));
      CHECK(cc5.baseFiles == 1);
      CHECK(cc5.baseElements == 1);
      CHECK(cc5.elementChanges == 2);
      CHECK(cc5.elementConflicts == 1);
      CHECK(cc5.fileConflicts == 1);
      CHECK(cc5.manualResolutions == 1);
    }


  SUBCASE("two elements no conflicts")
    {
      // no changes
      ConflictCount cc0 = countFiles(ProjectFile({CodeElement(0), CodeElement(3)}),
				     ProjectFile({CodeElement(0), CodeElement(3)}),
				     ProjectFile({CodeElement(0), CodeElement(3)}));
      CHECK(cc0.baseFiles == 1);
      CHECK(cc0.baseElements == 2);
      CHECK(cc0.elementChanges == 0);
      CHECK(cc0.elementConflicts == 0);
      CHECK(cc0.fileConflicts == 0);
      CHECK(cc0.manualResolutions == 0);
      CHECK(cc0.maxFileSize == 2);

      // one single change
      ConflictCount cc1 = countFiles(ProjectFile({CodeElement(0), CodeElement(3)}),
				     ProjectFile({CodeElement(1), CodeElement(3)}),
				     ProjectFile({CodeElement(0), CodeElement(3)}));
      CHECK(cc1.baseFiles == 1);
      CHECK(cc1.baseElements == 2);
      CHECK(cc1.elementChanges == 1);
      CHECK(cc1.elementConflicts == 0);
      CHECK(cc1.fileConflicts == 0);
      CHECK(cc1.manualResolutions == 0);

      ConflictCount cc2 = countFiles(ProjectFile({CodeElement(0), CodeElement(3)}),
				     ProjectFile({CodeElement(0), CodeElement(3)}),
				     ProjectFile({CodeElement(0), CodeElement(4)}));
      CHECK(cc2.baseFiles == 1);
      CHECK(cc2.baseElements == 2);
      CHECK(cc2.elementChanges == 1);
      CHECK(cc2.elementConflicts == 0);
      CHECK(cc2.fileConflicts == 0);
      CHECK(cc2.manualResolutions == 0);

          // two single changes
      ConflictCount cc5 = countFiles(ProjectFile({ CodeElement(0), CodeElement(3)}),
				     ProjectFile({ CodeElement(1), CodeElement(3)}),
				     ProjectFile({ CodeElement(0), CodeElement(4)}));
      CHECK(cc5.baseFiles == 1);
      CHECK(cc5.baseElements == 2);
      CHECK(cc5.elementChanges == 2);
      CHECK(cc5.elementConflicts == 0);
      CHECK(cc5.fileConflicts == 0);
      CHECK(cc5.manualResolutions == 0);
    }

  SUBCASE("two elements with conflicts")
    {
      // one conflict, one unchanged
      ConflictCount cc3 = countFiles(ProjectFile({CodeElement(0), CodeElement(3)}),
				     ProjectFile({CodeElement(1), CodeElement(3)}),
				     ProjectFile({CodeElement(2), CodeElement(3)}));
      CHECK(cc3.baseFiles == 1);
      CHECK(cc3.baseElements == 2);
      CHECK(cc3.elementChanges == 2);
      CHECK(cc3.elementConflicts == 1);
      CHECK(cc3.fileConflicts == 1);
      CHECK(cc3.manualResolutions == 1);

      // one conflict, one single change
      ConflictCount cc4 = countFiles(ProjectFile({CodeElement(0), CodeElement(3)}),
				     ProjectFile({CodeElement(1), CodeElement(4)}),
				     ProjectFile({CodeElement(2), CodeElement(3)}));
      CHECK(cc4.baseFiles == 1);
      CHECK(cc4.baseElements == 2);
      CHECK(cc4.elementChanges == 3);
      CHECK(cc4.elementConflicts == 1);
      CHECK(cc4.fileConflicts == 1);
      CHECK(cc4.manualResolutions == 2);

      // two conflicts
      ConflictCount cc6 = countFiles(ProjectFile({CodeElement(0), CodeElement(3)}),
				     ProjectFile({CodeElement(1), CodeElement(4)}),
				     ProjectFile({CodeElement(2), CodeElement(5)}));
      CHECK(cc6.baseFiles == 1);
      CHECK(cc6.baseElements == 2);
      CHECK(cc6.elementChanges == 4);
      CHECK(cc6.elementConflicts == 2);
      CHECK(cc6.fileConflicts == 1);
      CHECK(cc6.manualResolutions == 2);
    }
}

ConflictCounter::conflict_t EditCounter::count(const ProjectState& base,
					       const ProjectState& left,
					       const ProjectState& right) const
{
    ConflictCount cc;

    if (base.size() == 0)
      return cc;

    if (left.size() != base.size() || right.size() != base.size())
      {
        std::cerr << "What? Size mismatch: " << left.size() << ":"
                  << base.size() << ":" << right.size() << "!" << std::endl;
	throw std::runtime_error("Size mismatch");
      }
    
     for (const auto &bFile : base) {
         ++cc.baseFiles;
	 cc.baseElements += base.size();
	const auto &lFile = *(left.find(bFile.id()));
        const auto &rFile = *(right.find(bFile.id()));

	const auto fileCount = countFiles(bFile, lFile, rFile);

	cc += fileCount;
     }
     return cc;
}

