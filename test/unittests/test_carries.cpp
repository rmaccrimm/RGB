#include "catch.hpp"
#include "utils.h"

TEST_CASE("Half carries for addition", "[half_carry_add]")
{
    REQUIRE(utils::half_carry_add(0x8, 0x8) == true);
    REQUIRE(utils::half_carry_add(0xf, 0x1) == true);
    REQUIRE(utils::half_carry_add(0xf, 0xf) == true);
    REQUIRE(utils::half_carry_add(0xff, 0x1) == true);
    REQUIRE(utils::half_carry_add(0xf0, 0x1) == false);
    REQUIRE(utils::half_carry_add(0xa, 0x1) == false);
    REQUIRE(utils::half_carry_add(0x0, 0x0) == false);
    REQUIRE(utils::half_carry_add(0xf, 0x0) == false);
}

TEST_CASE("Half carries for subtraction", "[half_carry_sub]")
{
    REQUIRE(utils::half_carry_sub(0x10, 0x1) == true);
    REQUIRE(utils::half_carry_sub(0x10, 0xf) == true);
    REQUIRE(utils::half_carry_sub(0xf1, 0xa) == true);
    REQUIRE(utils::half_carry_sub(0xf7, 0x8) == true);
    REQUIRE(utils::half_carry_sub(0x0, 0xf) == true);
    REQUIRE(utils::half_carry_sub(0x18, 0x1) == false);
    REQUIRE(utils::half_carry_sub(0x1f, 0xf) == false);
    REQUIRE(utils::half_carry_sub(0xf, 0x2) == false);
}

TEST_CASE("Full carries for addition", "[full_carry_add]")
{
    REQUIRE(utils::full_carry_add(0xff, 0x1) == true);
    REQUIRE(utils::full_carry_add(0xf8, 0xf0) == true);
    REQUIRE(utils::full_carry_add(0x80, 0x80) == true);
    REQUIRE(utils::full_carry_add(0x8f, 0xf0) == true);
    REQUIRE(utils::full_carry_add(0xff00, 0x1) == false);
    REQUIRE(utils::full_carry_add(0xff, 0x0) == false);
    REQUIRE(utils::full_carry_add(0xff00, 0xff) == false);
    REQUIRE(utils::full_carry_add(0x0f, 0xf0) == false);
}

TEST_CASE("Full carries for subtraction", "[full_carry_sub]")
{
    REQUIRE(utils::full_carry_sub(0xff00, 0x1) == true);
    REQUIRE(utils::full_carry_sub(0x0100, 0xffff) == true);
    REQUIRE(utils::full_carry_sub(0x1, 0x1080) == true);
    REQUIRE(utils::full_carry_sub(0xff80, 0xff) == true);
    REQUIRE(utils::full_carry_sub(0x0, 0x0100) == false);
    REQUIRE(utils::full_carry_sub(0xff01, 0x1) == false);
    REQUIRE(utils::full_carry_sub(0xff80, 0x7f) == false);
    REQUIRE(utils::full_carry_sub(0xffff, 0x0) == false);
}
