#include "testsHeader.hxx"

// =====================================================================================================
// Node class tests
// =====================================================================================================

TEST_CASE("Node class tests","[node][point][distance]")
{
    Event event1 = {.id = 1, .Xvertex = 0.0, .Yvertex = 0.0, .Zvertex = 0.0};
    Event event2 = {.id = 2, .Xvertex = 1.0, .Yvertex = 1.0, .Zvertex = 1.0};
    Event event3 = {.id = 3, .Xvertex = -1.0, .Yvertex = -1.0, .Zvertex = -1.0};
    Event event4 = {.id = 4, .Xvertex = -2.0, .Yvertex = -2.0, .Zvertex = -1.0};
    Event event5 = {.id = 5, .Xvertex = 2.0, .Yvertex = 2.0, .Zvertex = 2.0};

    SECTION("Empty node state")
    {
        Node<Event,double,3,SquaredDist> node({},0,32);

        REQUIRE_THAT(node.GetMedian(), Catch::Matchers::WithinRel(double())); // GetMedian should be equal to T() [T = double]
        REQUIRE(node.IsSplit() == false); // node is not split
        REQUIRE(node.GetLeftNode() == nullptr); // no children
        REQUIRE(node.GetRightNode() == nullptr); // ibid.
        REQUIRE(node.GetData().size() == 0); // passed data was empty
    }

    SECTION("Adding point ot the node where stored data <= bucket size")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        
        Node<Event,double,3,SquaredDist> node({},0,1);

        REQUIRE(node.AddPoint(std::move(point1)) == true); // point was added successfuly
        REQUIRE(node.IsSplit() == false); // node is not split
        REQUIRE(node.GetLeftNode() == nullptr); // no children
        REQUIRE(node.GetRightNode() == nullptr); // ibid.
        REQUIRE(node.GetData().size() == 1); // stored data has one element
    }

    SECTION("Node size and data size should be equal")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};

        Node<Event,double,3,SquaredDist> node({},0,32);

        REQUIRE(node.GetData().size() == 0); // passed data was empty
        REQUIRE(node.size() == 0); // node is empty
        CHECK(node.AddPoint(std::move(point1)) == true);
        REQUIRE(node.GetData().size() == 1); // data has 1 elem
        REQUIRE(node.size() == 1); // node has 1 elem
    }

    SECTION("Adding point ot the node where stored data > bucket size")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};
        
        Node<Event,double,3,SquaredDist> node({},0,1);

        CHECK(node.AddPoint(std::move(point1)) == true); // point was added successfuly
        REQUIRE(node.IsSplit() == false); // node is not split

        CHECK(node.AddPoint(std::move(point2)) == true); // point was added successfuly
        REQUIRE(node.IsSplit() == true); // node is split

        REQUIRE_THAT(node.GetMedian(),Catch::Matchers::WithinRel(double(0.5))); // median should be equal to 0+1/2

        CHECK_FALSE(node.GetLeftNode() == nullptr); // children exist
        CHECK(node.GetLeftNode()->size() == 1); // half is passed to left
        REQUIRE(node.GetLeftNode()->IsSplit() == false); // children are not split

        CHECK_FALSE(node.GetRightNode() == nullptr); // children exist
        CHECK(node.GetRightNode()->size() == 1); // half is passed to right
        REQUIRE(node.GetRightNode()->IsSplit() == false); // children are not split

        REQUIRE(node.size() == 0); // stored data has no elements
    }

    SECTION("Split node should guide new point to correct child node")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};
        Point<Event,double,3> point3 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point4 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};
        
        Node<Event,double,3,SquaredDist> node({point1,point2},0,1);
        REQUIRE(node.IsSplit() == true); // testing again, this time data is passed in ctor

        REQUIRE(node.GetChild(point3) == node.GetLeftNode()); // point3 == point1 and point3 < median therefore it will point to left node
        REQUIRE(node.GetChild(point4) == node.GetRightNode()); // point4 == point1 and point4 > median therefore it will point to right node
    }

    SECTION("Adding points to split node should be impossible")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};
        Point<Event,double,3> point3 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        
        Node<Event,double,3,SquaredDist> node({point1,point2},0,1);
        REQUIRE(node.AddPoint(std::move(point3)) == false); // node is split, AddPoint should return false
        REQUIRE(node.size() == 0); // data should still be empty
    }

    SECTION("Removing point from node")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};
        Point<Event,double,3> point3 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        
        Node<Event,double,3,SquaredDist> node({point1},0,1);

        CHECK(node.size() == 1);
        REQUIRE(node.RemovePoint(point2)== false); // removing point which is not in tree shoudl eval to false
        CHECK(node.size() == 1);
        REQUIRE(node.RemovePoint(point3) == true); // removing point with same ID should be possible (Leaf class impelmentation dependent)
        CHECK(node.size() == 0);
        REQUIRE(node.GetData().size() == 0); //check again if it was removed from data
    }

    SECTION("Nearest point in node which has points")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};
        Point<Event,double,3> point3 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        
        Node<Event,double,3,SquaredDist> node({point1,point2},0,2);

        Point<Event,double,3> nearestPoint = node.FindNearest(point3);
        CHECK(node.size() == 2);
        CHECK(point1 == point3);
        REQUIRE(nearestPoint == point1); // nearest point should be the one which has the same value as point we use to search
    }

    SECTION("Nearest point in node which has split")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};
        Point<Event,double,3> point3 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        
        Node<Event,double,3,SquaredDist> node({point1,point2},0,1);

        Point<Event,double,3> nearestPoint = node.FindNearest(point3);
        CHECK(node.size() == 0);

        Point<Event,double,3> emptyPoint{};
        REQUIRE(nearestPoint == emptyPoint); // nearest point should be equal a default-construced one
    }

    SECTION("Nearest point in empty node")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        
        Node<Event,double,3,SquaredDist> node({},0,1);

        Point<Event,double,3> nearestPoint = node.FindNearest(point1);
        Point<Event,double,3> emptyPoint{};
        REQUIRE(nearestPoint == emptyPoint); // returned point should be equal a default-construced one
    }

    SECTION("N nearest points where N = 1 and N <= size")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};
        Point<Event,double,3> point3 = {event3,{event3.Xvertex,event3.Yvertex,event3.Zvertex}};
        Point<Event,double,3> point4 = {event4,{event4.Xvertex,event4.Yvertex,event4.Zvertex}};
        Point<Event,double,3> point5 = {event5,{event5.Xvertex,event5.Yvertex,event5.Zvertex}};
        Point<Event,double,3> point6 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        
        Node<Event,double,3,SquaredDist> node({point1,point2,point3,point4,point5},0,5);

        CHECK(node.size() == 5);

        Point<Event,double,3> nearestPoint = node.FindNearest(point6);
        std::vector<Point<Event,double,3> > nearestPointVec = node.FindNNearest(point6,1);
        CHECK(nearestPointVec.size() == 1); // make sure that vector has the same size as number of requested points
        REQUIRE(nearestPoint == nearestPointVec.front()); // with N=1 both methods should give the same result
    }

    SECTION("N nearest points where N > 1 and N <= size")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};
        Point<Event,double,3> point3 = {event3,{event3.Xvertex,event3.Yvertex,event3.Zvertex}};
        Point<Event,double,3> point4 = {event4,{event4.Xvertex,event4.Yvertex,event4.Zvertex}};
        Point<Event,double,3> point5 = {event5,{event5.Xvertex,event5.Yvertex,event5.Zvertex}};
        Point<Event,double,3> point6 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        
        Node<Event,double,3,SquaredDist> node({point1,point2,point3,point4,point5},0,5);

        CHECK(node.size() == 5);

        std::vector<Point<Event,double,3> > nearestPointVec = node.FindNNearest(point6,3);
        CHECK(nearestPointVec.size() == 3); // make sure that vector has the same size as number of requested points
        REQUIRE(nearestPointVec.at(0) == point1); // same position
        REQUIRE(nearestPointVec.at(1) == point2); // 2nd closest position
        REQUIRE(nearestPointVec.at(2) == point3); // 2nd closest position
    }

    SECTION("N nearest points where N > size")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};
        Point<Event,double,3> point3 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        
        Node<Event,double,3,SquaredDist> node({point1,point2},0,5);

        CHECK(node.size() == 2);

        std::vector<Point<Event,double,3> > nearestPointVec = node.FindNNearest(point3,3);
        CHECK(nearestPointVec.size() == 2); // make sure that vector has the same size node
        REQUIRE(nearestPointVec.at(0) == point1); // same position
        REQUIRE(nearestPointVec.at(1) == point2); // 2nd closest position
    }

    SECTION("N nearest points in split node")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};
        Point<Event,double,3> point3 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        
        Node<Event,double,3,SquaredDist> node({point1,point2},0,1);
        CHECK(node.size() == 0);

        std::vector<Point<Event,double,3> > nearestPointVec = node.FindNNearest(point3,2);
        REQUIRE(nearestPointVec.empty() == true);
        REQUIRE(nearestPointVec.size() == 0);
    }

    SECTION("N nearest points in empty node")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        
        Node<Event,double,3,SquaredDist> node({},0,1);
        CHECK(node.size() == 0);

        std::vector<Point<Event,double,3> > nearestPointVec = node.FindNNearest(point1,2);
        REQUIRE(nearestPointVec.empty() == true);
        REQUIRE(nearestPointVec.size() == 0);
    }

    SECTION("Within distance where all points fulfill the condition")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};
        Point<Event,double,3> point3 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        
        Node<Event,double,3,SquaredDist> node({point1,point2},0,5);

        std::vector<Point<Event,double,3> > nearestPointVec = node.FindWithinDistance(point3,3);
        CHECK(nearestPointVec.size() == 2);
        REQUIRE(nearestPointVec.at(0) == point1);
        REQUIRE(nearestPointVec.at(1) == point2);
    }

    SECTION("Within distance where some points fulfill the condition")
    {
        Point<Event,double,3> point1 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        Point<Event,double,3> point2 = {event2,{event2.Xvertex,event2.Yvertex,event2.Zvertex}};
        Point<Event,double,3> point3 = {event3,{event3.Xvertex,event3.Yvertex,event3.Zvertex}};
        Point<Event,double,3> point4 = {event4,{event4.Xvertex,event4.Yvertex,event4.Zvertex}};
        Point<Event,double,3> point5 = {event5,{event5.Xvertex,event5.Yvertex,event5.Zvertex}};
        Point<Event,double,3> point6 = {event1,{event1.Xvertex,event1.Yvertex,event1.Zvertex}};
        
        Node<Event,double,3,SquaredDist> node({point1,point2,point3,point4,point5},0,5);

        std::vector<Point<Event,double,3> > nearestPointVec = node.FindWithinDistance(point6,3);
        CHECK(nearestPointVec.size() == 3);
        REQUIRE(nearestPointVec.at(0) == point1);
        REQUIRE(nearestPointVec.at(1) == point2);
        REQUIRE(nearestPointVec.at(2) == point3);
    }
}