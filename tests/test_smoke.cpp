#include <catch2/catch_test_macros.hpp>

// a very small starter test.
// its only purpose right now is to verify that catch 2 is wired up correctly
// and that the test executable can build and run successfully.

// here we create a test named "Smoke test passes"
// we tag it with "[smoke]" because using tags will be useful later when we want to run
// only certain groups of tests.
TEST_CASE("Smoke test passes", "[smoke]") {
    REQUIRE(1 + 1 == 2);
}


// the #include for catch 2 test macros give us access to:
// TEST_CASE and REQUIRE
// these are the basic pieces we use to write tests.