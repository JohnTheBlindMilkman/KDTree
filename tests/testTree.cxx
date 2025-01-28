#include "testsHeader.hxx"

// =====================================================================================================
// KDTree class tests
// =====================================================================================================

TEST_CASE("KDTree class test","[kdtree][node][point][distance]")
{
    Event event1 = {.id = 1, .Xvertex = 0.0, .Yvertex = 0.0, .Zvertex = 0.0};
    Event event2 = {.id = 2, .Xvertex = 1.0, .Yvertex = 1.0, .Zvertex = 1.0};
    Event event3 = {.id = 3, .Xvertex = -1.0, .Yvertex = -1.0, .Zvertex = -1.0};
    Event event4 = {.id = 4, .Xvertex = -2.0, .Yvertex = -2.0, .Zvertex = -1.0};
    Event event5 = {.id = 5, .Xvertex = 2.0, .Yvertex = 2.0, .Zvertex = 2.0};
    Event event6 = {.id = 6, .Xvertex = -2.1, .Yvertex = 2.0, .Zvertex = 2.0};
    Event event7 = {.id = 7, .Xvertex = 3.5, .Yvertex = 3.5, .Zvertex = -3.5};
    Event event8 = {.id = 8, .Xvertex = -4., .Yvertex = -4., .Zvertex = 4.};

    SECTION("State of empty tree")
    {
        KDTree<Event,3,double,SquaredDist> tree(32);

        REQUIRE(tree.GetBucketSize() == 32);
        REQUIRE(tree.GetMaxSizeBeforeSplit() == 10000);
        REQUIRE(tree.IsSplit() == false);
        REQUIRE(tree.size() == 0);
    }

    SECTION("State of not empty and not split tree")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};

        KDTree<Event,3,double,SquaredDist> tree(32,10000,{point1,point2});

        CHECK(tree.IsSplit() == false);
        REQUIRE(tree.size() == 2);
    }

    SECTION("State of not empty and split (by default) tree")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};
        Point<Event,double,3> point3 = {event3,{event3.Xvertex,event3.Yvertex,event3.Zvertex}};

        KDTree<Event,3,double,SquaredDist> tree(32,2,{point1,point2,point3});

        REQUIRE(tree.IsSplit() == true);
        REQUIRE(tree.size() == 3);
    }

    SECTION("State of not empty and split (explicitly) tree")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};
        Point<Event,double,3> point3 = {event3,{event3.Xvertex,event3.Yvertex,event3.Zvertex}};

        KDTree<Event,3,double,SquaredDist> tree(32,4,{point1,point2,point3});

        CHECK(tree.IsSplit() == false);
        CHECK(tree.size() == 3);

        tree.SplitTree();

        REQUIRE(tree.IsSplit() == true);
        REQUIRE(tree.size() == 3); // here I also require instead of check, because we switch to a different evaluation of size
    }

    SECTION("Adding point to empty tree")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};

        KDTree<Event,3,double,SquaredDist> tree(32);

        CHECK(tree.IsSplit() == false);
        CHECK(tree.size() == 0);

        tree.AddPoint(std::move(point1));

        CHECK(tree.IsSplit() == false);
        REQUIRE(tree.size() == 1);
    }

    SECTION("Adding points to a tree and splitting it")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};
        Point<Event,double,3> point3 = {event3,{event3.Xvertex,event3.Yvertex,event3.Zvertex}};

        KDTree<Event,3,double,SquaredDist> tree(32,2,{point1,point2});

        CHECK(tree.IsSplit() == false);
        CHECK(tree.size() == 2);

        tree.AddPoint(std::move(point3));

        CHECK(tree.IsSplit() == true);
        REQUIRE(tree.size() == 3);
    }

    SECTION("Removing point from tree with entries (not split)")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};
        Point<Event,double,3> point3 = {event3,{event3.Xvertex,event3.Yvertex,event3.Zvertex}};

        KDTree<Event,3,double,SquaredDist> tree(32,10000,{point1,point2,point3});

        CHECK(tree.IsSplit() == false);
        CHECK(tree.size() == 3);

        REQUIRE(tree.RemovePoint(point3) == true);

        CHECK(tree.IsSplit() == false);
        REQUIRE(tree.size() == 2);
    }

    SECTION("Removing point from an empty tree")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};

        KDTree<Event,3,double,SquaredDist> tree(32);

        CHECK(tree.IsSplit() == false);
        CHECK(tree.size() == 0);

        REQUIRE(tree.RemovePoint(point1) == false);

        CHECK(tree.IsSplit() == false);
        CHECK(tree.size() == 0);
    }

    // remove from split tree

    // find nearest

    // find n nearest

    // find within distance
}