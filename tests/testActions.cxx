#include "testsHeader.hxx"

// =====================================================================================================
// Action classes tests
// =====================================================================================================

OneDim objOneDim1{1,1};
OneDim objOneDim2{2,4};
OneDim objOneDim3{3,7};
OneDim objOneDim4{4,8};
OneDim objOneDim5{5,12};
OneDim objOneDim6{6,14};
OneDim objOneDim7{7,17};
OneDim objOneDim8{8,18};

TEST_CASE("Inserter class tests","[node][point][distance][action]")
{
    JJDataStruct::KDTree::Inserter<OneDim,double,1,SquaredDist> inserter;

    SECTION("Inserter throws an exception if the node is a null pointer")
    {
        Point<OneDim,double,1> newPointOneDim = {objOneDim1,objOneDim1.x};
        REQUIRE_THROWS(inserter.Insert(nullptr,std::move(newPointOneDim))); // passing a nullptr should throw std::runtime_error
    }

    SECTION("Inserter can add point to empty node")
    {
        std::shared_ptr<Node<OneDim,double,1,SquaredDist> > node = std::make_shared<Node<OneDim,double,1,SquaredDist> >(std::vector<Point<OneDim,double,1> >(),nullptr,0,1);
        CHECK(node->IsEmpty());
        CHECK(node->size() == 0); // making sure the node is empty

        Point<OneDim,double,1> point1 = {objOneDim1,objOneDim1.x};
        REQUIRE(inserter.Insert(node,std::move(point1))); // insert x = 1

        REQUIRE_FALSE(node->IsEmpty()); // has elements
        REQUIRE(node->size() == 1); // has one element
        REQUIRE(node->GetData().at(0) == point1); // the point is point with x = 1
    }

    SECTION("Inserter can add point to a full node")
    {
        Point<OneDim,double,1> point1 = {objOneDim1,objOneDim1.x};
        std::shared_ptr<Node<OneDim,double,1,SquaredDist> > node = std::make_shared<Node<OneDim,double,1,SquaredDist> >(std::vector<Point<OneDim,double,1> >{point1},nullptr,0,1);
        CHECK_FALSE(node->IsEmpty());
        CHECK(node->size() == 1); // making sure the node is not empty
        CHECK_FALSE(node->IsSplit()); // making sure the node was not split beforehand

        Point<OneDim,double,1> point2 = {objOneDim2,objOneDim2.x};
        REQUIRE(inserter.Insert(node,std::move(point2))); // insert x = 4

        REQUIRE(node->IsEmpty()); // has no emtries
        REQUIRE(node->size() == 0); // ibid.
        REQUIRE(node->IsSplit()); // has split
        REQUIRE_THAT(node->GetMedian(),Catch::Matchers::WithinRel(2.5)); // median is (1 + 4) / 2 = 2.5
        REQUIRE(node->GetLeftNode()->GetData().at(0) == point1); // point x = 1 went left
        REQUIRE(node->GetRightNode()->GetData().at(0) == point2); // point x = 4 went right
    }

    SECTION("Inserter can add point the correct node in a tree")
    {
        Point<OneDim,double,1> point1 = {objOneDim1,objOneDim1.x};
        Point<OneDim,double,1> point2 = {objOneDim2,objOneDim2.x};
        Point<OneDim,double,1> point3 = {objOneDim3,objOneDim3.x};
        std::shared_ptr<Node<OneDim,double,1,SquaredDist> > node = std::make_shared<Node<OneDim,double,1,SquaredDist> >(std::vector<Point<OneDim,double,1> >{point1,point2,point3},nullptr,0,2);
        CHECK(node->IsEmpty());
        CHECK(node->size() == 0); // making sure the node is empty
        CHECK(node->IsSplit()); // making sure it is split
        CHECK_THAT(node->GetMedian(),Catch::Matchers::WithinRel(4.)); // median is 4

        CHECK(node->GetLeftNode()->size() == 1); // left node has 1 point
        CHECK(node->GetLeftNode()->GetData().at(0) == point1); // x = 1

        CHECK(node->GetRightNode()->size() == 2); // right node has 2 points
        CHECK(node->GetRightNode()->GetData().at(0) == point2); // x = 4
        CHECK(node->GetRightNode()->GetData().at(1) == point3); // x = 5

        Point<OneDim,double,1> newPoint = {OneDim{9,2},2};
        REQUIRE(inserter.Insert(node,std::move(newPoint))); // insert x = 2

        CHECK(node->GetRightNode()->size() == 2); // right node has 2 points
        CHECK(node->GetRightNode()->GetData().at(0) == point2); // x = 4
        CHECK(node->GetRightNode()->GetData().at(1) == point3); // x = 5

        REQUIRE(node->GetLeftNode()->size() == 2); // right node has 2 points
        REQUIRE(node->GetLeftNode()->GetData().at(1) == newPoint); // x = 2
    }
}

TEST_CASE("Deleter class tests","[node][point][distance][action]")
{
    JJDataStruct::KDTree::Deleter<OneDim,double,1,SquaredDist> deleter;

    SECTION("Deleter returns std::nullopt if the node is a null pointer")
    {
        Point<OneDim,double,1> newPointOneDim = {objOneDim1,objOneDim1.x};
        REQUIRE(deleter.Remove(nullptr,newPointOneDim) == std::nullopt);
    }

    SECTION("Deleter tries to delete an element in an empty node")
    {
        std::shared_ptr<Node<OneDim,double,1,SquaredDist> > node = std::make_shared<Node<OneDim,double,1,SquaredDist> >(std::vector<Point<OneDim,double,1> >(),nullptr,0,1);
        CHECK(node->IsEmpty());
        CHECK(node->size() == 0); // making sure the node is empty

        Point<OneDim,double,1> point1 = {objOneDim1,objOneDim1.x};
        REQUIRE_FALSE(deleter.Remove(node,point1).has_value()); // try to remove x = 1
    }

    SECTION("Deleter tries to delete an element which doesn't exist in the tree")
    {
        Point<OneDim,double,1> point1 = {objOneDim1,objOneDim1.x};
        Point<OneDim,double,1> point2 = {objOneDim2,objOneDim2.x};
        std::shared_ptr<Node<OneDim,double,1,SquaredDist> > node = std::make_shared<Node<OneDim,double,1,SquaredDist> >(std::vector<Point<OneDim,double,1> >{point1,point2},nullptr,0,1);
        CHECK(node->IsEmpty());
        CHECK(node->size() == 0); // making sure the node is empty
        CHECK(node->IsSplit()); // making sure it is split
        CHECK_THAT(node->GetMedian(),Catch::Matchers::WithinRel(2.5)); // median is 2.5
        CHECK_FALSE(node->GetLeftNode()->IsEmpty());
        CHECK(node->GetLeftNode()->size() == 1);
        CHECK(node->GetLeftNode()->GetData().at(0) == point1); // left node has x = 1
        CHECK_FALSE(node->GetRightNode()->IsEmpty());
        CHECK(node->GetRightNode()->size() == 1);
        CHECK(node->GetRightNode()->GetData().at(0) == point2); // right node has x = 4

        Point<OneDim,double,1> point3 = {objOneDim3,objOneDim3.x};
        REQUIRE_FALSE(deleter.Remove(node,point3).has_value()); // try remove x = 5
        
        CHECK_FALSE(node->GetLeftNode()->IsEmpty());
        CHECK(node->GetLeftNode()->size() == 1);
        CHECK(node->GetLeftNode()->GetData().at(0) == point1); // left node still has x = 1
        CHECK_FALSE(node->GetRightNode()->IsEmpty());
        CHECK(node->GetRightNode()->size() == 1);
        CHECK(node->GetRightNode()->GetData().at(0) == point2); // right node still has x = 4
    }

    SECTION("Deleter deletes an element which exists in the left sub-tree")
    {
        Point<OneDim,double,1> point1 = {objOneDim1,objOneDim1.x};
        Point<OneDim,double,1> point2 = {objOneDim2,objOneDim2.x};
        Point<OneDim,double,1> point3 = {objOneDim3,objOneDim3.x};
        Point<OneDim,double,1> point4 = {objOneDim4,objOneDim4.x};
        std::shared_ptr<Node<OneDim,double,1,SquaredDist> > node = std::make_shared<Node<OneDim,double,1,SquaredDist> >(std::vector<Point<OneDim,double,1> >{point1,point2,point3,point4},nullptr,0,2);
        
        CHECK(node->GetLeftNode()->size() == 2); // node has two points
        auto removed_point1 = deleter.Remove(node,point1);
        REQUIRE(removed_point1.has_value()); // remove x = 1
        REQUIRE(removed_point1.value() == point1);
        REQUIRE(node->GetLeftNode()->size() == 1); // node has one point left
    }

    SECTION("Deleter deletes an element which exists in the right sub-tree")
    {
        Point<OneDim,double,1> point1 = {objOneDim1,objOneDim1.x};
        Point<OneDim,double,1> point2 = {objOneDim2,objOneDim2.x};
        Point<OneDim,double,1> point3 = {objOneDim3,objOneDim3.x};
        Point<OneDim,double,1> point4 = {objOneDim4,objOneDim4.x};
        std::shared_ptr<Node<OneDim,double,1,SquaredDist> > node = std::make_shared<Node<OneDim,double,1,SquaredDist> >(std::vector<Point<OneDim,double,1> >{point1,point2,point3,point4},nullptr,0,2);
        
        CHECK(node->GetRightNode()->size() == 2); // node has two points
        auto remove_point2 = deleter.Remove(node,point3);
        REQUIRE(remove_point2.has_value()); // remove x = 7
        REQUIRE(remove_point2.value() == point3);
        REQUIRE(node->GetRightNode()->size() == 1); // node has one point left
    }

    SECTION("Deleter deletes an element and causes the parent node to join")
    {
        Point<OneDim,double,1> point1 = {objOneDim1,objOneDim1.x};
        Point<OneDim,double,1> point2 = {objOneDim2,objOneDim2.x};
        std::shared_ptr<Node<OneDim,double,1,SquaredDist> > node = std::make_shared<Node<OneDim,double,1,SquaredDist> >(std::vector<Point<OneDim,double,1> >{point1,point2},nullptr,0,1);
        
        CHECK(node->IsSplit());

        auto removed_point1 = deleter.Remove(node,point1);
        CHECK(removed_point1.has_value()); // remove x = 1
        CHECK(removed_point1.value() == point1);

        REQUIRE_FALSE(node->IsSplit());
    }
}

TEST_CASE("NearestFinder class tests","[node][point][distance][action]")
{
    Point<OneDim,double,1> point1 = {objOneDim1,objOneDim1.x};
    Point<OneDim,double,1> point2 = {objOneDim2,objOneDim2.x};
    Point<OneDim,double,1> point3 = {objOneDim3,objOneDim3.x};
    Point<OneDim,double,1> point4 = {objOneDim4,objOneDim4.x};
    Point<OneDim,double,1> point5 = {objOneDim5,objOneDim5.x};
    Point<OneDim,double,1> point6 = {objOneDim6,objOneDim6.x};
    Point<OneDim,double,1> point7 = {objOneDim7,objOneDim7.x};
    Point<OneDim,double,1> point8 = {objOneDim8,objOneDim8.x};

    std::shared_ptr<Node<OneDim,double,1,SquaredDist> > node = std::make_shared<Node<OneDim,double,1,SquaredDist> >(
        std::vector<Point<OneDim,double,1> >{point1,point2,point3,point4,point5,point6,point7,point8},
        nullptr,
        0,
        1);

    REQUIRE(node->IsEmpty());
    REQUIRE(node->IsSplit());
    REQUIRE(node->GetLeftNode()->GetLeftNode()->GetLeftNode()->GetData().at(0) == point1);
    REQUIRE(node->GetLeftNode()->GetLeftNode()->GetRightNode()->GetData().at(0) == point2);
    REQUIRE(node->GetLeftNode()->GetRightNode()->GetLeftNode()->GetData().at(0) == point3);
    REQUIRE(node->GetLeftNode()->GetRightNode()->GetRightNode()->GetData().at(0) == point4);
    REQUIRE(node->GetRightNode()->GetLeftNode()->GetLeftNode()->GetData().at(0) == point5);
    REQUIRE(node->GetRightNode()->GetLeftNode()->GetRightNode()->GetData().at(0) == point6);
    REQUIRE(node->GetRightNode()->GetRightNode()->GetLeftNode()->GetData().at(0) == point7);
    REQUIRE(node->GetRightNode()->GetRightNode()->GetRightNode()->GetData().at(0) == point8);

    JJDataStruct::KDTree::NearestFinder<OneDim,double,1,SquaredDist> finder;

    SECTION("Finder throws an exception if the node is a null pointer")
    {
        Point<OneDim,double,1> newPointOneDim = {objOneDim1,objOneDim1.x};
        REQUIRE_THROWS(finder.Find(nullptr,newPointOneDim));
    }

    SECTION("Finder goes to a node with the closest point")
    {
        OneDim newObjOneDim{9,5};
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto closestPoint = finder.Find(node,newPointOneDim);
        CHECK(closestPoint.has_value()); // make sure that we found a point

        REQUIRE(closestPoint.value() == point2); // the point we found is x = 4
    }

    SECTION("Finder goes to a node where the closest point should be, but it has been removed")
    {
        CHECK(node->GetLeftNode()->GetLeftNode()->GetRightNode()->RemovePoint(point2).has_value()); // we remove the closest point
        CHECK(node->GetLeftNode()->GetLeftNode()->GetRightNode()->IsEmpty());

        OneDim newObjOneDim{9,5};
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto closestPoint = finder.Find(node,newPointOneDim);
        CHECK(closestPoint.has_value()); // make sure that we found a point

        REQUIRE(closestPoint.value() == point3); // the point we found is x = 7
    }

    SECTION("Two points are within the same distance")
    {
        OneDim newObjOneDim{9,5.5};
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto closestPoint = finder.Find(node,newPointOneDim);
        CHECK(closestPoint.has_value()); // make sure that we found a point

        CHECK_THAT(std::abs(SquaredDist::distance(newPointOneDim,point2) - SquaredDist::distance(newPointOneDim,point3)), Catch::Matchers::WithinRel(double())); // check if the two points are approx. within the same distance
        REQUIRE(closestPoint.value() == point2); // the point we found is x = 4
        REQUIRE_FALSE(closestPoint.value() == point4); // the point we found is not x = 7
    }

    SECTION("Returns actual closest point because Cond is true")
    {
        auto closestPoint = finder.FindIf(node,point2,[](const Point<OneDim,double,1> &lhs, const Point<OneDim,double,1> &rhs){return lhs == rhs;});
        CHECK(closestPoint.has_value()); // make sure that we found a point

        REQUIRE(closestPoint.value() == point2); // the point we found is x = 4
    }

    SECTION("Returns second closest point because Cond is false")
    {
        auto closestPoint = finder.FindIf(node,point2,[](const Point<OneDim,double,1> &lhs, const Point<OneDim,double,1> &rhs){return lhs != rhs;});
        CHECK(closestPoint.has_value()); // make sure that we found a point

        REQUIRE(closestPoint.value() == point1); // the point we found is x = 1
    }
}

TEST_CASE("NNearestFinder class tests","[node][point][distance][action]")
{
    Point<OneDim,double,1> point1 = {objOneDim1,objOneDim1.x};
    Point<OneDim,double,1> point2 = {objOneDim2,objOneDim2.x};
    Point<OneDim,double,1> point3 = {objOneDim3,objOneDim3.x};
    Point<OneDim,double,1> point4 = {objOneDim4,objOneDim4.x};
    Point<OneDim,double,1> point5 = {objOneDim5,objOneDim5.x};
    Point<OneDim,double,1> point6 = {objOneDim6,objOneDim6.x};
    Point<OneDim,double,1> point7 = {objOneDim7,objOneDim7.x};
    Point<OneDim,double,1> point8 = {objOneDim8,objOneDim8.x};

    std::shared_ptr<Node<OneDim,double,1,SquaredDist> > node = std::make_shared<Node<OneDim,double,1,SquaredDist> >(
        std::vector<Point<OneDim,double,1> >{point1,point2,point3,point4,point5,point6,point7,point8},
        nullptr,
        0,
        1);

    REQUIRE(node->IsEmpty());
    REQUIRE(node->IsSplit());
    REQUIRE(node->GetLeftNode()->GetLeftNode()->GetLeftNode()->GetData().at(0) == point1); // x = 1
    REQUIRE(node->GetLeftNode()->GetLeftNode()->GetRightNode()->GetData().at(0) == point2); // x = 4
    REQUIRE(node->GetLeftNode()->GetRightNode()->GetLeftNode()->GetData().at(0) == point3); // x = 7
    REQUIRE(node->GetLeftNode()->GetRightNode()->GetRightNode()->GetData().at(0) == point4); // x = 8
    REQUIRE(node->GetRightNode()->GetLeftNode()->GetLeftNode()->GetData().at(0) == point5); // x = 12
    REQUIRE(node->GetRightNode()->GetLeftNode()->GetRightNode()->GetData().at(0) == point6); // x = 14
    REQUIRE(node->GetRightNode()->GetRightNode()->GetLeftNode()->GetData().at(0) == point7); // x = 17
    REQUIRE(node->GetRightNode()->GetRightNode()->GetRightNode()->GetData().at(0) == point8); // x = 18

    JJDataStruct::KDTree::NearestFinder<OneDim,double,1,SquaredDist> finder;
    JJDataStruct::KDTree::NNearestFinder<OneDim,double,1,SquaredDist> nFinder;

    SECTION("Finder throws an exception if the node is a null pointer")
    {
        Point<OneDim,double,1> newPointOneDim = {objOneDim1,objOneDim1.x};
        REQUIRE_THROWS(nFinder.Find(nullptr,newPointOneDim,1));
    }

    SECTION("Finder goes to a node with the closest point")
    {
        OneDim newObjOneDim{9,5};
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto closestPoint = finder.Find(node,newPointOneDim);
        auto nClosestPoints = nFinder.Find(node,newPointOneDim,1);

        CHECK(closestPoint.has_value()); // make sure that we found a point
        CHECK(nClosestPoints.size() == 1);

        REQUIRE(closestPoint.value() == point2); // the point we found is x = 4
        REQUIRE(nClosestPoints.at(0) == closestPoint.value()); // finders are consistent
    }

    SECTION("FindIf for 1 point works the same as NearestFinder")
    {
        auto closestPoint = finder.FindIf(node,point2,[](const Point<OneDim,double,1> &lhs, const Point<OneDim,double,1> &rhs){return lhs == rhs;});
        auto nClosestPoints = nFinder.FindIf(node,point2,1,[](const Point<OneDim,double,1> &lhs, const Point<OneDim,double,1> &rhs){return lhs == rhs;});

        CHECK(closestPoint.has_value()); // make sure that we found a point
        CHECK(nClosestPoints.size() == 1);

        REQUIRE(closestPoint.value() == point2); // the point we found is x = 4
        REQUIRE(nClosestPoints.at(0) == closestPoint.value()); // finders are consistent
    }

    SECTION("Finder goes to a node where the closest point should be, but it has been removed")
    {
        CHECK(node->GetLeftNode()->GetLeftNode()->GetRightNode()->RemovePoint(point2).has_value()); // we remove the closest point
        CHECK(node->GetLeftNode()->GetLeftNode()->GetRightNode()->IsEmpty());

        OneDim newObjOneDim{9,5};
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto closestPoint = finder.Find(node,newPointOneDim);
        auto nClosestPoints = nFinder.Find(node,newPointOneDim,1);

        CHECK(closestPoint.has_value()); // make sure that we found a point
        CHECK(nClosestPoints.size() == 1);

        REQUIRE(closestPoint.value() == point3); // the point we found is x = 7
        REQUIRE(nClosestPoints.at(0) == closestPoint.value()); // finders are consistent
    }

    SECTION("Get one point where two points are within the same distance")
    {
        OneDim newObjOneDim{9,5.5};
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto closestPoint = finder.Find(node,newPointOneDim);
        auto nClosestPoints = nFinder.Find(node,newPointOneDim,1);

        CHECK(closestPoint.has_value()); // make sure that we found a point
        CHECK(nClosestPoints.size() == 1);

        CHECK_THAT(std::abs(SquaredDist::distance(newPointOneDim,point2) - SquaredDist::distance(newPointOneDim,point3)), Catch::Matchers::WithinRel(double())); // check if the two points are approx. within the same distance
        
        REQUIRE(closestPoint.value() == point2); // the point we found is x = 4
        REQUIRE(nClosestPoints.at(0) == closestPoint.value()); // finders are consistent
    }

    SECTION("Get two points where two points are within the same distance")
    {
        OneDim newObjOneDim{9,5.5};
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto nClosestPoints = nFinder.Find(node,newPointOneDim,2);
        CHECK(nClosestPoints.size() == 2); // make sure that we found two points

        REQUIRE(nClosestPoints.at(0) == point2); // the first point we found is x = 4
        REQUIRE(nClosestPoints.at(1) == point3); // the second point we found is x = 7
    }

    SECTION("Get three points where two points are within the same distance")
    {
        OneDim newObjOneDim{9,5.5};
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto nClosestPoints = nFinder.Find(node,newPointOneDim,3);
        CHECK(nClosestPoints.size() == 3); // make sure that we found three points

        REQUIRE(nClosestPoints.at(0) == point2); // the first point we found is x = 4
        REQUIRE(nClosestPoints.at(1) == point3); // the second point we found is x = 7
        REQUIRE(nClosestPoints.at(2) == point4); // the third point we found is x = 8
    }

    SECTION("Returns actual closest points because Cond is true")
    {
        OneDim newObjOneDim{9,5.5}; // has the same ID as point2, in order to remove it
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto nClosestPoints = nFinder.FindIf(node,newPointOneDim,3,[](const Point<OneDim,double,1> &lhs, const Point<OneDim,double,1> &rhs){return true;});
        CHECK(nClosestPoints.size() == 3); // make sure that we found three points

        REQUIRE(nClosestPoints.at(0) == point2); // the first point we found is x = 4
        REQUIRE(nClosestPoints.at(1) == point3); // the second point we found is x = 7
        REQUIRE(nClosestPoints.at(2) == point4); // the third point we found is x = 8
    }

    SECTION("Returns some other closest points because Cond is false for one of them")
    {
        OneDim newObjOneDim{objOneDim2.id,5.5}; // has the same ID as point2, in order to remove it
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto nClosestPoints = nFinder.FindIf(node,newPointOneDim,3,[](const Point<OneDim,double,1> &lhs, const Point<OneDim,double,1> &rhs){return lhs != rhs;});
        CHECK(nClosestPoints.size() == 3); // make sure that we found three points

        REQUIRE(nClosestPoints.at(0) == point3); // the first point we found is x = 7 and not x = 4 as before
        REQUIRE(nClosestPoints.at(1) == point4); // the second point we found is x = 8
        REQUIRE(nClosestPoints.at(2) == point1); // the third point we found is x = 1
    }
}

TEST_CASE("DistanceFinder class tests","[node][point][distance][action]")
{
    Point<OneDim,double,1> point1 = {objOneDim1,objOneDim1.x};
    Point<OneDim,double,1> point2 = {objOneDim2,objOneDim2.x};
    Point<OneDim,double,1> point3 = {objOneDim3,objOneDim3.x};
    Point<OneDim,double,1> point4 = {objOneDim4,objOneDim4.x};
    Point<OneDim,double,1> point5 = {objOneDim5,objOneDim5.x};
    Point<OneDim,double,1> point6 = {objOneDim6,objOneDim6.x};
    Point<OneDim,double,1> point7 = {objOneDim7,objOneDim7.x};
    Point<OneDim,double,1> point8 = {objOneDim8,objOneDim8.x};

    std::shared_ptr<Node<OneDim,double,1,SquaredDist> > node = std::make_shared<Node<OneDim,double,1,SquaredDist> >(
        std::vector<Point<OneDim,double,1> >{point1,point2,point3,point4,point5,point6,point7,point8},
        nullptr,
        0,
        1);

    REQUIRE(node->IsEmpty());
    REQUIRE(node->IsSplit());
    REQUIRE(node->GetLeftNode()->GetLeftNode()->GetLeftNode()->GetData().at(0) == point1); // x = 1
    REQUIRE(node->GetLeftNode()->GetLeftNode()->GetRightNode()->GetData().at(0) == point2); // x = 4
    REQUIRE(node->GetLeftNode()->GetRightNode()->GetLeftNode()->GetData().at(0) == point3); // x = 7
    REQUIRE(node->GetLeftNode()->GetRightNode()->GetRightNode()->GetData().at(0) == point4); // x = 8
    REQUIRE(node->GetRightNode()->GetLeftNode()->GetLeftNode()->GetData().at(0) == point5); // x = 12
    REQUIRE(node->GetRightNode()->GetLeftNode()->GetRightNode()->GetData().at(0) == point6); // x = 14
    REQUIRE(node->GetRightNode()->GetRightNode()->GetLeftNode()->GetData().at(0) == point7); // x = 17
    REQUIRE(node->GetRightNode()->GetRightNode()->GetRightNode()->GetData().at(0) == point8); // x = 18

    JJDataStruct::KDTree::DistanceFinder<OneDim,double,1,SquaredDist> distFinder;

    SECTION("Finder throws an exception if the node is a null pointer")
    {
        Point<OneDim,double,1> newPointOneDim = {objOneDim1,objOneDim1.x};
        REQUIRE_THROWS(distFinder.Find(nullptr,newPointOneDim,1));
    }

    SECTION("Finder returns a point that is well within the hypersphere")
    {
        OneDim newObjOneDim{9,5};
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto closestPoints = distFinder.Find(node,newPointOneDim,2);

        CHECK_FALSE(closestPoints.empty()); // make sure that we found a point
        CHECK(closestPoints.size() == 1);

        REQUIRE(closestPoints.at(0) == point2); // the point we found is x = 4
    }

    SECTION("Finder returns a point that is at the edge of the hypershpere distance")
    {
        OneDim newObjOneDim{9,5};
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto closestPoints = distFinder.Find(node,newPointOneDim,1);

        CHECK_FALSE(closestPoints.empty()); // make sure that we found a point
        CHECK(closestPoints.size() == 1);

        REQUIRE(closestPoints.at(0) == point2); // the point we found is x = 4
    }

    SECTION("Finder returns empty vector if no point was within distance (point within distance was removed)")
    {
        CHECK(node->GetLeftNode()->GetLeftNode()->GetRightNode()->RemovePoint(point2).has_value()); // we remove the closest point
        CHECK(node->GetLeftNode()->GetLeftNode()->GetRightNode()->IsEmpty());

        OneDim newObjOneDim{9,5};
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto closestPoints = distFinder.Find(node,newPointOneDim,2);

        REQUIRE(closestPoints.empty()); // no point was found
        REQUIRE(closestPoints.size() == 0);
    }

    SECTION("Finder returns empty vector if no point was within distance (distance was too small)")
    {
        OneDim newObjOneDim{9,5};
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto closestPoints = distFinder.Find(node,newPointOneDim,0.5);

        REQUIRE(closestPoints.empty()); // no point was found
        REQUIRE(closestPoints.size() == 0);
    }

    SECTION("Two points are at the edge of the hypersphere, but in oppoiste directions")
    {
        OneDim newObjOneDim{9,13};
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto closestPoints = distFinder.Find(node,newPointOneDim,1);

        CHECK(closestPoints.size() == 2);

        CHECK_THAT(std::abs(SquaredDist::distance(newPointOneDim,point5) - SquaredDist::distance(newPointOneDim,point6)), Catch::Matchers::WithinRel(double())); // check if the two points are approx. within the same distance
        
        REQUIRE(closestPoints.at(0) == point5); // the point we found is x = 12
        REQUIRE(closestPoints.at(1) == point6); // the point we found is x = 14
    }

    SECTION("Finder returns all of the points within the tree")
    {
        OneDim newObjOneDim{9,10};
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto closestPoints = distFinder.Find(node,newPointOneDim,100);
        CHECK(closestPoints.size() == 8); // make sure that we found all points

        std::sort(closestPoints.begin(),closestPoints.end(),
                                [point = Point<OneDim,double,1>{OneDim{10,0},0}](const Point<OneDim,double,1> &lhs, const Point<OneDim,double,1> &rhs)
                                {
                                    return SquaredDist::distance(point,lhs) < SquaredDist::distance(point,rhs);
                                }); // sorting in ascending order to make it easier for me

        REQUIRE(closestPoints.at(0) == point1); // the first point we found is x = 1
        REQUIRE(closestPoints.at(1) == point2); // the second point we found is x = 4
        REQUIRE(closestPoints.at(2) == point3); // the first point we found is x = 7
        REQUIRE(closestPoints.at(3) == point4); // the second point we found is x = 8
        REQUIRE(closestPoints.at(4) == point5); // the first point we found is x = 12
        REQUIRE(closestPoints.at(5) == point6); // the second point we found is x = 14
        REQUIRE(closestPoints.at(6) == point7); // the first point we found is x = 17
        REQUIRE(closestPoints.at(7) == point8); // the second point we found is x = 18
    }

    SECTION("Finder with condition set to true works the same as if without it")
    {
        OneDim newObjOneDim{9,5};
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto closestPoints = distFinder.Find(node,newPointOneDim,2);
        auto closestPointsCond = distFinder.FindIf(node,newPointOneDim,2,[](const Point<OneDim,double,1> &lhs, const Point<OneDim,double,1> &rhs){return true;});

        CHECK_FALSE(closestPoints.empty()); // make sure that we found a point
        CHECK(closestPoints.size() == 1);
        CHECK_FALSE(closestPointsCond.empty()); // make sure that we found a point here too
        CHECK(closestPointsCond.size() == 1);

        REQUIRE(closestPoints.at(0) == point2); // the point we found is x = 4
        REQUIRE(closestPoints.at(0) == closestPointsCond.at(0));
    }

    SECTION("Finder doesn't return a point within distance, because it doesn't meet the condition")
    {
        OneDim newObjOneDim{objOneDim2.id,5}; // same ID as for point2
        Point<OneDim,double,1> newPointOneDim = {newObjOneDim,newObjOneDim.x};

        auto closestPointsCond = distFinder.FindIf(node,newPointOneDim,2,[](const Point<OneDim,double,1> &lhs, const Point<OneDim,double,1> &rhs){return lhs != rhs;});

        REQUIRE(closestPointsCond.empty());
    }
}