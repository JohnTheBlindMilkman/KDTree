#include "testsHeader.hxx"

// =====================================================================================================
// Point class test
// =====================================================================================================

TEST_CASE("Point class test","[point]")
{
    Event event = {.id = 1, .Xvertex = 0.0, .Yvertex = 0.0, .Zvertex = 0.0};

    SECTION("Point should be constructable by setting its components by hand")
    {
        Point<Event,double,3> point;
        point.object = event;
        point.coords = {event.Xvertex,event.Yvertex,event.Zvertex};

        REQUIRE(point.object == event);
        REQUIRE(point.coords.at(0) == event.Xvertex);
        REQUIRE(point.coords.at(1) == event.Yvertex);
        REQUIRE(point.coords.at(2) == event.Zvertex);
    }

    SECTION("Point should be constructible by designated initialiser")
    {
        Point<Event,double,3> point = {event,{event.Xvertex,event.Yvertex,event.Zvertex}};

        REQUIRE(point.object == event);
        REQUIRE(point.coords.at(0) == event.Xvertex);
        REQUIRE(point.coords.at(1) == event.Yvertex);
        REQUIRE(point.coords.at(2) == event.Zvertex);
    }

    SECTION("Points with equal objects should be considered equal")
    {
        Point<Event,double,3> point1 = {event,{event.Xvertex,event.Yvertex,event.Zvertex}};
        Point<Event,double,3> point2 = {event,{event.Xvertex,event.Yvertex,event.Zvertex}};

        REQUIRE(point1 == point2);
    }
}