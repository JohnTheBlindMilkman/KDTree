#include "testsHeader.hxx"

// =====================================================================================================
// Distance class tests
// =====================================================================================================

TEST_CASE("Distance class tests","[distance]")
{
    Event event1 = {.id = 1, .Xvertex = 0.0, .Yvertex = 0.0, .Zvertex = 0.0};
    Event event2 = {.id = 2, .Xvertex = 1.0, .Yvertex = 1.0, .Zvertex = 1.0};

    Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
    Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};

    SECTION("Distance should return correct value")
    {
        // would be good idea to check more "extreme" values to check what happens with overflow and underflow
        REQUIRE_THAT(SquaredDist::distance(point1,point2), Catch::Matchers::WithinRel(double(3)));
        REQUIRE_THAT(RootSquaredDist::distance(point1,point2), Catch::Matchers::WithinRel(double(1.7320508075688772935274463415059)));
    }

    SECTION("Order should not matter")
    {
        REQUIRE_THAT(SquaredDist::distance(point1,point2), Catch::Matchers::WithinRel(SquaredDist::distance(point2,point1)));
        REQUIRE_THAT(RootSquaredDist::distance(point1,point2), Catch::Matchers::WithinRel(RootSquaredDist::distance(point2,point1)));
    }
}