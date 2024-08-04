//

#include <random>
#include <utility>

// Use distro and gen to draw random values until one lies in the range
// [limits.first, limits.second). It is incumbent on the caller to ensure
// the efficiency of this process.
template <class GEN, class DISTRO, typename T>
T constrained_throw(std::pair<T,T> limits, GEN & gen, DISTRO & distro)
{
  T val;
  do {
    val = distro(gen);
  } while (val < limits.first || val >= limits.second);
  return val;
}
