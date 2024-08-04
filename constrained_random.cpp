#include "constrained_random.h"

#include "projectid.h"

#include <doctest.h>

#include <algorithm>
#include <limits>
#include <ranges>

TEST_CASE("limits enforced")
{
  const auto i_limits = std::make_pair(3_id,7_id);
  const auto d_limits = std::make_pair(3.0,7.0);

  auto gen = std::mt19937(0.0);
  
    SUBCASE("integer")
    {
      auto int_d = std::uniform_int_distribution(0_id, 10_id);
      std::ranges::for_each(std::views::iota(0, 40),
			    [&i_limits, &gen, &int_d](int){
			      const auto val = constrained_throw(i_limits, gen, int_d);
			      CHECK(val >= i_limits.first);
			      CHECK(val < i_limits.second);
			    });
    }


    SUBCASE("one allowed value")
      {
	const auto limits = std::make_pair(1_id,2_id);
	auto d = std::uniform_int_distribution(0_id, 3_id);
	std::ranges::for_each(std::views::iota(0, 10),
			      [&limits, &gen, &d](int){
				const auto val = constrained_throw(limits, gen, d);
				CHECK(val == limits.first);
			    });
      } 
    
    
    SUBCASE("Floating point")
      {
        auto double_d = std::uniform_real_distribution(0.0, 10.0);
	std::ranges::for_each(std::views::iota(0,200),
			      [&d_limits, &gen, &double_d](int){
				const auto val = constrained_throw(d_limits, gen, double_d);
				CHECK(val >= d_limits.first);
				CHECK(val < d_limits.second);
			      });
      }
}
