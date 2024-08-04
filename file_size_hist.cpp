// Simple histogram printer for filesizes set by BasicCreator

#include "basiccreator.h"
#include "projectstate.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <ranges>

template <typename T>
std::ostream& stream_histogram(std::ostream& os, const std::vector<T>& hist,
    std::string name = "", double scale = 1.0)
{
  if (!name.empty())
    os << name << ":" << std::endl;

  // Print results
  const auto underflow = 0_id;
  const auto min = underflow + 1;
  const auto overflow = hist.size()-1;
  os << std::setw(5) << "UNDER" << "|";
  if (hist[underflow] > 0)
    os << std::setw(hist[underflow]*scale) << std::setfill('*') << '*'
       << std::setfill(' ');
  os << std::endl;
  for (size_t i = min; i < overflow - 1; ++i)
    {
      os << std::setw(5) << i << "|";
      if (hist[i] > 0)
	os << std::setw(hist[i]*scale) << std::setfill('*') << '*'
	   << std::setfill(' ');
      os << std::endl;
    }
  os << std::setw(5) << "OVER" << "|";
  if (hist[overflow] > 0)
    os << std::setw(hist[overflow]*scale) << std::setfill('*') << '*'
       << std::setfill(' ');
  os << std::endl;

  return os;
}

int main(int argc, char *argv[])
{
  constexpr id_t nRounds = 1;

  id_t nFiles = 15;
  if (argc > 1)
    {
      nFiles = std::min(std::max(2, std::atoi(argv[1])), 50);
      std::cerr << "Project size is " << nFiles << " files.)" << std::endl;
    }
    // Because we're interested here is the shape and extent of the
    // distribution, force the average length to be about that in the
    // paper.
  constexpr double x_bar = 4.13; // From Table 3 of paper
  constexpr id_t mean_file_length = std::exp(x_bar);
  const id_t nElements = mean_file_length * nFiles;

  constexpr id_t hist_bins = mean_file_length/4;
  constexpr id_t hist_bin_width = 10;
  constexpr id_t histmax = hist_bin_width * hist_bins;
  std::vector<double> hist(hist_bins + 1, 1); // [0,histmax] and overflow
  
  BasicCreator creator(nFiles, nElements);
  std::cout << "Distribution parameters (mu, sigma): "
            << creator.mu_sigma().first
            << ", "
            << creator.mu_sigma().second
	    << std::endl;
  
  for (id_t i = 0; i < nRounds; ++i)
    {
      const auto ps = creator.create();
      assert(ps.size() == nFiles);
      for (const auto & pf : ps)
	{
          const auto clampped_size = std::clamp<id_t>(pf.size(), 0, histmax);
	  const auto size_bin = clampped_size/hist_bin_width;
	  hist[size_bin]++;
	}
    }

  std::vector<double> cumm; // Cummulative distribution
  std::transform(hist.begin(), hist.end(), std::back_inserter(cumm),
		 [](double d){ static double sum=0.0; sum += d; return sum; });

  id_t maxLength = 0;
  double mean = 0.0;
  double weight = 0.0;
  for (const auto idx : std::ranges::iota_view { 0ul, hist.size() - 2ul })
    {
      weight += hist[idx];
      mean += idx * hist[idx];
      maxLength = hist[idx] ? idx : maxLength;
    }
  mean /= weight;
  
  const auto max = std::max_element(hist.begin(), hist.end());
  [[maybe_unused]] const auto h_scale = std::min(75.0 / *max, 1.0);

  stream_histogram(std::cout, hist, "File sizes", h_scale);
  std::cout << "\t\t" << "Total files: " << nRounds*nFiles << std::endl;
  std::cout << "\t\t" << "Empty files: " << hist[0] << std::endl;
  std::cout << "\t\t"
            << "Mean length: " << mean
	    << "    (" <<  mean_file_length << ")" << std::endl;
  std::cout << "\t\t" << "Max length:  " << maxLength << std::endl;
  std::cout << "\t\t"
            << "Overflows: " << hist.back() << std::endl;
  stream_histogram(std::cout, cumm, "Cumulative", std::min(75 / cumm.back(), 1.0));
  }
