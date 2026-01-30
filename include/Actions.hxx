#ifndef Actions_hxx
    #define Actions_hxx

    #include <deque>

    #include "Node.hxx"

    namespace JJDataStruct
    {
        namespace KDTree
        {
            /**
             * @brief Visitor object. It traverses throught tree and returns the taken path
             * 
             * @tparam Leaf Object type that will be stored in leafs 
             * @tparam T Arithmetic type of point coordinates
             * @tparam Dims Number of dimensions over which the data will be divided 
             * @tparam Distance Metric upon which the distance will be calculated 
             */
            template <typename Leaf, typename T , std::size_t Dims, typename Distance>
            class Visitor
            {
                private:
                    std::deque<std::shared_ptr<Node<Leaf,T,Dims,Distance> > > m_nodePath;

                public:
                    std::deque<std::shared_ptr<Node<Leaf,T,Dims,Distance> > > Visit(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &startNode, const Point<Leaf,T,Dims> &point)
                    {
                        if (startNode != nullptr)
                        {
                            m_nodePath.push_front(startNode);

                            if (startNode->IsSplit())
                            {
                                return Visit(startNode->GetChild(point),std::move(point));
                            }
                        }

                        return m_nodePath;
                    }
            };

            /**
             * @brief Inserter object. It tries to emplace passed object at a correct node
             * 
             * @tparam Leaf Object type that will be stored in leafs 
             * @tparam T Arithmetic type of point coordinates
             * @tparam Dims Number of dimensions over which the data will be divided 
             * @tparam Distance Metric upon which the distance will be calculated 
             */
            template <typename Leaf, typename T , std::size_t Dims, typename Distance>
            class Inserter
            {
                private:
                    Visitor<Leaf,T,Dims,Distance> m_visitor;

                public:
                    bool Insert(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, Point<Leaf,T,Dims> point)
                    {
                        auto nodes = m_visitor.Visit(node,point);
                        return nodes.front()->AddPoint(std::move(point));
                    }
            };

            /**
             * @brief Deleter object. It tries to delete passed object at a correct node
             * 
             * @tparam Leaf Object type that will be stored in leafs 
             * @tparam T Arithmetic type of point coordinates
             * @tparam Dims Number of dimensions over which the data will be divided 
             * @tparam Distance Metric upon which the distance will be calculated 
             */
            template <typename Leaf, typename T , std::size_t Dims, typename Distance>
            class Deleter
            {
                private:
                    Visitor<Leaf,T,Dims,Distance> m_visitor;

                public:
                    bool Remove(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, const Point<Leaf,T,Dims> &point)
                    {
                        auto nodes = m_visitor.Visit(node,point);
                        return nodes.front()->RemovePoint(point);
                    }
            };

            /**
             * @brief Finder object. It tries to find closest point to the one passed as argument. It will look only within the vicinity of the point
             * 
             * @tparam Leaf Object type that will be stored in leafs 
             * @tparam T Arithmetic type of point coordinates
             * @tparam Dims Number of dimensions over which the data will be divided 
             * @tparam Distance Metric upon which the distance will be calculated 
             */
            template <typename Leaf, typename T , std::size_t Dims, typename Distance>
            class NearestFinder
            {
                private:
                    std::size_t m_maxDepth;
                    Visitor<Leaf,T,Dims,Distance> m_visitor;

                    void FindClosestPoint(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, const Point<Leaf,T,Dims> &point, Point<Leaf,T,Dims> &closestPoint, bool &pointWasFound)
                    {                        
                        if (node->IsSplit())
                        {
                            FindClosestPoint(node->GetChild(point), point, closestPoint, pointWasFound);
                            auto distToMedian = node->CalculateDistanceToMedian(point);
                            auto distToClosest = Distance::distance(point,closestPoint); // this is calculated unnecessarily often
                            
                            if (distToMedian < distToClosest)
                                FindClosestPoint(node->GetOtherChild(point), point, closestPoint, pointWasFound);
                        }
                        else if (!node->IsEmpty())
                        {
                            pointWasFound = true;
                            auto newClosestPoint = node->FindNearest(point).value(); // I have a guarantee that I will find a point, because I check if this node is not empty
                            if (Distance::distance(newClosestPoint,point) < Distance::distance(closestPoint,point))
                                closestPoint = newClosestPoint;
                        }
                    }

                public:
                    std::optional<Point<Leaf,T,Dims> > Find(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, const Point<Leaf,T,Dims> &point)
                    {
                        std::array<T,Dims> coordinates;
                        std::fill_n(coordinates.begin(),Dims,std::numeric_limits<T>::infinity());
                        Point<Leaf,T,Dims> closestPoint = {Leaf(), coordinates};

                        bool pointWasFound = false;
                        FindClosestPoint(node,point,closestPoint,pointWasFound);

                        // how do I check if I have found any point at all?
                        return (pointWasFound) ? std::optional<Point<Leaf,T,Dims> >{closestPoint} : std::nullopt;
                    }
            };

            /**
             * @brief Finder object. It tries to find N closest points to the one passed as argument.
             * 
             * @tparam Leaf Object type that will be stored in leafs 
             * @tparam T Arithmetic type of point coordinates
             * @tparam Dims Number of dimensions over which the data will be divided 
             * @tparam Distance Metric upon which the distance will be calculated 
             */
            template <typename Leaf, typename T , std::size_t Dims, typename Distance>
            class NNearestFinder
            {
                private:
                    Visitor<Leaf,T,Dims,Distance> m_visitor;

                    void FindAtLeastNClosestPoints(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, const Point<Leaf,T,Dims> &point, std::vector<Point<Leaf,T,Dims> > &closestPoints, std::size_t nPoints)
                    {
                        if (node->IsSplit())
                        {
                            FindAtLeastNClosestPoints(node->GetChild(point), point, closestPoints, nPoints);
                            auto distToMedian = node->CalculateDistanceToMedian(point);
                            auto distToClosest = Distance::distance(point,closestPoints.front()); // this is calculated unnecessarily often
                            
                            if (distToMedian < distToClosest || closestPoints.size() < nPoints)
                                FindAtLeastNClosestPoints(node->GetOtherChild(point), point, closestPoints, nPoints);
                        }
                        else
                        {
                            auto newClosestPoints = node->FindNNearest(point,nPoints);
                            closestPoints.insert(closestPoints.end(),newClosestPoints.begin(),newClosestPoints.end());
                            std::sort(closestPoints.begin(),closestPoints.end(),[&point](const Point<Leaf,T,Dims> &lhs, const Point<Leaf,T,Dims> &rhs){return Distance::distance(point,lhs) < Distance::distance(point,rhs);});
                        }
                    }

                public:
                    std::vector<Point<Leaf,T,Dims> > Find(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, const Point<Leaf,T,Dims> &point, std::size_t nPoints)
                    {
                        std::vector<Point<Leaf,T,Dims> > closestPoints;

                        FindAtLeastNClosestPoints(node,point,closestPoints,nPoints);
                        std::sort(closestPoints.begin(),closestPoints.end(),
                                [&point](const Point<Leaf,T,Dims> &lhs, const Point<Leaf,T,Dims> &rhs)
                                {
                                    return Distance::distance(point,lhs) < Distance::distance(point,rhs);
                                });

                        return (closestPoints.size() > nPoints) ? std::vector<Point<Leaf,T,Dims> >(closestPoints.begin(),closestPoints.begin() + nPoints) : closestPoints;
                    }
            };

             /**
             * @brief Finder object. It tries to find all points within a given distance to the point passed as argument. It will look only within the vicinity of the point and may return less than all possible points.
             * 
             * @tparam Leaf Object type that will be stored in leafs 
             * @tparam T Arithmetic type of point coordinates
             * @tparam Dims Number of dimensions over which the data will be divided 
             * @tparam Distance Metric upon which the distance will be calculated 
             */
            template <typename Leaf, typename T , std::size_t Dims, typename Distance>
            class DistanceFinder
            {
                private:
                    Visitor<Leaf,T,Dims,Distance> m_visitor;

                    void FindWithinDistance(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, const Point<Leaf,T,Dims> &point, std::vector<Point<Leaf,T,Dims> > &closestPoints, T distance)
                    {
                        if (node->IsSplit())
                        {
                            FindWithinDistance(node->GetChild(point), point, closestPoints, distance);
                            auto distToMedian = node->CalculateDistanceToMedian(point);
                            
                            if (distToMedian < distance)
                                FindWithinDistance(node->GetOtherChild(point), point, closestPoints, distance);
                        }
                        else
                        {
                            auto newClosestPoints = node->FindWithinDistance(point,distance);
                            closestPoints.insert(closestPoints.end(),newClosestPoints.begin(),newClosestPoints.end());
                        }
                    }

                public:
                    std::vector<Point<Leaf,T,Dims> > Find(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, const Point<Leaf,T,Dims> &point, T distance)
                    {
                        std::vector<Point<Leaf,T,Dims> > closestPoints;
                        FindWithinDistance(node,point,closestPoints,distance);
                        return closestPoints;
                    }
            };

        } // namespace KDTree
        
    } // namespace JJDataStruct
    

#endif