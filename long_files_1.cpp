#include "basiccreator.h"
#include "codeelement.h"
#include "editcounter.h"
#include "editmutator.h"
#include "interfaces.h"
#include "projectfile.h"
#include "projectstate.h"

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>

std::vector<id_t> parseCommandLine(int argc, char* argv[])
{
    if (argc <= 1)
      return std::vector<id_t> {25, 50, 100, 250, 500, 1000, 2500, 5000, 10000, 20000};

  bool printHelp = false;
  std::vector<id_t> meanFileLength;
  for (int i = 1; i < argc; ++i) {
    id_t num = std::atoi(argv[i]);
    if (num > 0) {
      meanFileLength.push_back(num);
    } else {
      printHelp = true;
      std::cerr << "Error: couldn't parse argument '" << argv[i]
		<< "' as a number." << std::endl;
    }
  }
  
  // Handle early-abort cases (help, version, ....)
  if (printHelp) {
      std::cout << "Usage: " << argv[0] << " [number of files in project]+"
                << std::endl;
      exit(EXIT_FAILURE);
  }

  return meanFileLength;
}

int main(int argc, char *argv[])
{
  // Configuration variables and defaults
  double fraction = 0.02; // Fraction of elements changed during an edit
  id_t nElements = 200000;
  id_t nRounds = 45;
  int verbose=1;

  std::vector<id_t> meanFileLength = parseCommandLine(argc, argv);

  // Initialize the behaviors we're going to test
  const auto mutator = std::make_unique<EditMutator>(nElements*fraction);
  const auto counter = std::make_unique<EditCounter>();

  // Main results: mean file length vs mean number of manual merge actions
  std::map<double, double> results;

  for (const auto length : meanFileLength)
    {
      // Creation behavior depend on the number of project
      const auto creator = std::make_unique<BasicCreator>(nElements / length,
							  nElements);
    
      // Global bookkeeping storage
      ConflictCount total;

      // Core simulation loop
      std::cerr << "Length" << std::setw(6) << length << ":";
      for (id_t i = 0; i < nRounds; ++i)
	{
	std::cerr << ".";
	  
	const auto base = creator->create();
	const auto left = mutator->mutate(base);
	const auto right = mutator->mutate(base);
		    
        if (verbose > 1)
	  {
	    std::cerr << "Iteration " << i << ":" << std::endl;
            std::cerr << "\tBase  " << base.size() << " files." << std::endl;
            std::cerr << "\t\t[" << base.fileSizeLimits().first
                      << "," << base.fileSizeLimits().second << "]"
		      << std::endl;
	    std::cerr << "\tLeft " << left.size() << " files." << std::endl;
	    std::cerr << "\tRight " << right.size() << " files." << std::endl;
	  }
	
	// score and accumulate
        ConflictCount cc = counter->count(base, left, right);
        if (verbose > 1)
	  {
            std::cerr << "\t" << "Counted: " << cc.elementChanges
		      << " total changes." << std::endl;
            std::cerr << "\t" << "         " << cc.elementConflicts
		      << " conflicted changes." << std::endl;
            std::cerr << "\t"
                      << "         " << cc.fileConflicts
		      << " out of " << cc.baseFiles
		      << " files with conflicts." << std::endl;
            std::cerr << "\t" << "         " << cc.manualResolutions
		      << " manual merge actions required." << std::endl;
	  }
	total += cc;
      }
      std::cout << std::endl;
      
      if (verbose > 0)
	  {
            std::cerr << "\t" << "Counted: " << total.elementChanges
		      << " total changes." << std::endl;
            std::cerr << "\t" << "         " << total.elementConflicts
		      << " conflicted changes." << std::endl;
            std::cerr << "\t"
                      << "         " << total.fileConflicts
		      << " out of " << total.baseFiles
		      << " files with conflicts." << std::endl;
            std::cerr << "\t" << "         " << total.manualResolutions
		      << " manual merge actions required." << std::endl;
            std::cerr << "\t" << "         " << total.maxFileSize
		      << " maximum file size." << std::endl;
	  }
      std::cerr << std::endl;

      results[length]
	= total.manualResolutions/static_cast<double>(total.elementChanges);
    }

  std::cout << "# Length" << "\t" << "% manual" << std::endl;
  for (const auto& [length, rate] : results)
    {
      std::cout << std::setw(8) << std::fixed << std::setprecision(1)
                << length
                << "\t"
                << std::setw(8) << std::fixed << std::setprecision(2)
		<< rate*100
		<< std::endl;
    }
  
  return EXIT_SUCCESS;
}
