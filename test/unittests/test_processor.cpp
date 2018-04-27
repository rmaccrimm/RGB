#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "functions.h"

TEST_CASE("Half carries for addition", "[half_carry_add]")
{
    REQUIRE(half_carry_add(0x8, 0x8) == true);
    REQUIRE(half_carry_add(0xf, 0x1) == true);
    REQUIRE(half_carry_add(0x1, 0xf) == true);
    REQUIRE(half_carry_add(0xf, 0xf) == true);
}

