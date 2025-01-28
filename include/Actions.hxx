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
                    Visitor<Leaf,T,Dims,Distance> m_visitor;

                    void FindClosestPoint(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, const Point<Leaf,T,Dims> &point, Point<Leaf,T,Dims> &closestPoint)
                    {
                        if (node->IsSplit())
                        {
                            FindClosestPoint(node->GetLeftNode(),point,closestPoint);
                            FindClosestPoint(node->GetRightNode(),point,closestPoint);
                        }
                        else
                        {
                            auto newClosestPoint = node->FindNearest(point);
                            if (Distance::distance(newClosestPoint,point) < Distance::distance(closestPoint,point))
                                closestPoint = newClosestPoint;
                        }
                    }

                public:
                    Point<Leaf,T,Dims> Find(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, const Point<Leaf,T,Dims> &point)
                    {
                        auto nodes = m_visitor.Visit(node,point);

                        std::array<T,Dims> coordinates;
                        std::fill_n(coordinates.begin(),Dims,std::numeric_limits<T>::infinity());
                        Point<Leaf,T,Dims> closestPoint = {.object = Leaf(), .coords = coordinates};

                        FindClosestPoint(nodes.at(std::min(nodes.size(),Dims) - 1),point,closestPoint);

                        return closestPoint;
                    }
            };

            /**
             * @brief Finder object. It tries to find N closest points to the one passed as argument. It will look only within the vicinity of the point and may return less than N points.
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

                    void FindNClosestPoints(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, const Point<Leaf,T,Dims> &point, std::vector<Point<Leaf,T,Dims> > &closestPoints, std::size_t nPoints)
                    {
                        if (node->IsSplit())
                        {
                            FindNClosestPoints(node->GetLeftNode(),point,closestPoints,nPoints);
                            FindNClosestPoints(node->GetRightNode(),point,closestPoints,nPoints);
                        }
                        else
                        {
                            auto newClosestPoints = node->FindNNearest(point,nPoints);
                            closestPoints.insert(closestPoints.end(),newClosestPoints.begin(),newClosestPoints.end());
                        }
                    }

                public:
                    std::vector<Point<Leaf,T,Dims> > Find(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, const Point<Leaf,T,Dims> &point, std::size_t nPoints)
                    {
                        auto nodes = m_visitor.Visit(node,point);

                        std::vector<Point<Leaf,T,Dims> > closestPoints;

                        FindNClosestPoints(nodes.at(std::min(nodes.size(),Dims) - 1),point,closestPoints,nPoints);
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
                            FindWithinDistance(node->GetLeftNode(),point,closestPoints,distance);
                            FindWithinDistance(node->GetRightNode(),point,closestPoints,distance);
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
                        auto nodes = m_visitor.Visit(node,point);
                        std::vector<Point<Leaf,T,Dims> > closestPoints;

                        FindWithinDistance(nodes.at(std::min(nodes.size(),Dims) - 1),point,closestPoints,distance);

                        return closestPoints;
                    }
            };

        } // namespace KDTree
        
    } // namespace JJDataStruct
    

#endif