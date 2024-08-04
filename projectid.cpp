#include "projectid.h"
#include <sys/types.h>

#include <doctest.h>

#include <limits>
#include <string>

// Initilaize so that the first nextId will return zero, but we don't
// precompute.
static id_t global_id = std::numeric_limits<id_t>::max();

id_t nextId()
{
  return ++global_id;
}

TEST_CASE("Global ID changes")
{
  const auto initial = nextId();
  CHECK(nextId() == initial + 1);
}

id_t operator""_id (unsigned long long value)
{
  return static_cast<id_t>(value);
}

TEST_CASE("id_t literal values")
{
  CHECK(0_id == 0);
  CHECK(1_id == 1);
  CHECK(32000_id == 32000);

  CHECK(-1_id == ~0_id); // Depends on twos-complement
}
