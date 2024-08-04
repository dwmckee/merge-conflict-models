#include "basiccreator.h"

#include "constrained_random.h"
#include "projectid.h"
#include "projectstate.h"

#include <cstdarg>
#include <doctest.h>

#include <algorithm>
#include <iostream>
#include <random>
#include <utility>

BasicCreator::BasicCreator(id_t nFiles, id_t nElements)
    : _nFiles(nFiles)
    , _nElements(nElements)
{}

ProjectState BasicCreator::create() const
{
    ProjectState ps;

    if (_nFiles == 0)
      return ps;
    
    // Create empty files
    for (id_t i=0; i<_nFiles; ++i)
      {
	ps.addFile(nextId());
      }

    std::random_device rd;
    std::mt19937 gen(rd());
    // The distriution of source file sizes is non-trivial. I.Herraiz,
    // D.M.German, and A.E.Hassan "On The Distribution Of Source Code File
    // Sizes" suggests that a "double Pareto distribution" is better than
    // log-normal, but the standard library has log-normal, so start with that.
    const auto [m, s] = mu_sigma();
    auto d = std::lognormal_distribution<double> (m, s);

    // Create remaining elements assigning each to a (uniformly) randomly chosen
    // file
    for (id_t j = 0; j < _nElements; ++j)
      {
	// Choose a file to put the new element in
        const id_t idx = constrained_throw(std::pair(0_id, _nFiles), gen, d);
	ps.at(idx).appendElement(nextId());
      }
    
    return ps;
};

TEST_CASE("Creation number of files and elements")
{
  SUBCASE("no files")
    {
      const BasicCreator c1(0, 0);
      const auto ps1 = c1.create();
      CHECK(ps1.size() == 0);

      const BasicCreator c2(0, 1);
      const auto  ps2 = c2.create();
      CHECK(ps2.size() == 0);
    }

  SUBCASE("one file")
    {
      const BasicCreator c3(1, 10);
      const auto ps3 = c3.create();
      CHECK(ps3.size() == 1);
      CHECK(ps3[0].size() == 10);
    }

  SUBCASE("two files")
    {
      const BasicCreator c4(2, 20);
      const auto ps4 = c4.create();
      CHECK(ps4.size() == 2);
      CHECK(ps4[0].size() + ps4[1].size() == 20);
    }
    
    SUBCASE("many files")
      {
          const BasicCreator c5(15, 75);
          const auto ps5 = c5.create();
          CHECK(ps5.size() == 15);
	  const auto totalElements = std::accumulate(ps5.begin(), ps5.end(), 0_id,
						     [](const auto& tot, const auto& pf) {
	  return tot + pf.size(); });
          CHECK(totalElements == 75);
      }
}

std::pair<double, double> BasicCreator::mu_sigma() const
{
   // Use the shape parameter from the "Overall" row of Table 3 in the paper:
    //
    constexpr double s = 1.49;
    constexpr double mu = log(s); // Often seen in PDF/CDF
    //
    // The second parameter (sigma or mean depending on your POV) is going to
    // depend on the number of files, and we choose it so that
    //
    // CDF(_nFiles) = 0.98
    //
    // Use the form  CDF(x) = [1 + erf(ln(x/s)/(sqrt(2)*sigma))]/2,
    //
    // and extract sigma.
    //
    // ln(nFiles/s)/(sqrt(2)*sigma) = erf^-1(1.98 - 1)
    constexpr double inv_erf_of_CDF = 1.65; // Tabulated value  
    // 
    // sigma = log(nFiles/s) / (sqrt(2) * 1.65)
    const double sigma = log(_nFiles/s) / (sqrt(2) * inv_erf_of_CDF);

    return std::make_pair(mu, sigma);
}
