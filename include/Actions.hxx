#ifndef Actions_hxx
    #define Actions_hxx

    #include <deque>

    #include "Node.hxx"

    namespace JJDataStruct
    {
        namespace KDTree
        {
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
                public:
                    /**
                     * @brief Insert point starting from the given node
                     * 
                     * @param node starting node (top of the tree)
                     * @param point point to be inserted
                     * @return true if successful, flase otherweise
                     * @throws std::runtime_error if the node is a nullptr an exception is thrown
                     */
                    bool Insert(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, Point<Leaf,T,Dims> &&point)
                    {
                        if (node != nullptr)
                        {
                            if (node->IsSplit())
                            {
                                return Insert(node->GetChild(point),std::move(point));
                            }
                            else
                            {
                                return node->AddPoint(std::move(point));
                            }
                        }
                        else
                        {
                            throw std::runtime_error("Inserter::Insert - Node is nullptr");
                        }
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
                public:
                    /**
                     * @brief Remove point starting from the given node
                     * 
                     * @param node starting node (top of the tree)
                     * @param point point to be removed
                     * @return true if successful, flase otherweise
                     * @throws std::runtime_error if the node is a nullptr an exception is thrown
                     */
                    bool Remove(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, const Point<Leaf,T,Dims> &point)
                    {
                        if (node != nullptr)
                        {
                            if (node->IsSplit())
                            {
                                return Remove(node->GetChild(point),point);
                            }
                            else
                            {
                                return node->RemovePoint(point);
                            }
                        }
                        else
                        {
                            throw std::runtime_error("Deleter::Remove - Node is nullptr");
                        }
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
                    void FindClosestPoint(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, const Point<Leaf,T,Dims> &point, Point<Leaf,T,Dims> &closestPoint, bool &pointWasFound)
                    {      
                        if (node != nullptr)
                        {
                            if (node->IsSplit())
                            {
                                FindClosestPoint(node->GetChild(point), point, closestPoint, pointWasFound);
                                auto distToMedian = node->CalculateDistanceToMedian(point);
                                auto distToClosest = (pointWasFound) ? Distance::distance(point,closestPoint) : std::numeric_limits<T>::infinity(); // this is calculated unnecessarily often
                                
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
                        else
                        {
                            throw std::runtime_error("NearestFinder::FindClosestPoint - Node is nullptr");
                        }
                    }

                public:
                    /**
                     * @brief Find closest point in the tree
                     * 
                     * @param node starting node (top of the tree)
                     * @param point point to match against
                     * @return closest point or std::nullopt if no point was found in the tree
                     */
                    std::optional<Point<Leaf,T,Dims> > Find(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, const Point<Leaf,T,Dims> &point)
                    {
                        Point<Leaf,T,Dims> closestPoint = {Leaf(), {}};

                        bool pointWasFound = false;
                        FindClosestPoint(node, point, closestPoint, pointWasFound);

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
                    void FindAtLeastNClosestPoints(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, const Point<Leaf,T,Dims> &point, std::vector<Point<Leaf,T,Dims> > &closestPoints, unsigned nPoints)
                    {
                        if (node != nullptr)
                        {
                            if (node->IsSplit())
                            {
                                FindAtLeastNClosestPoints(node->GetChild(point), point, closestPoints, nPoints);
                                auto distToMedian = node->CalculateDistanceToMedian(point);
                                auto distToClosest = (closestPoints.empty()) ? std::numeric_limits<T>::infinity()  : Distance::distance(point,closestPoints.back()); // this is calculated unnecessarily often
                                
                                if (distToMedian < distToClosest || closestPoints.size() < nPoints)
                                    FindAtLeastNClosestPoints(node->GetOtherChild(point), point, closestPoints, nPoints);
                            }
                            else
                            {
                                auto newClosestPoints = node->FindNNearest(point,nPoints);
                                closestPoints.insert(closestPoints.end(),newClosestPoints.begin(),newClosestPoints.end());
                                std::sort(closestPoints.begin(),closestPoints.end(),
                                    [&point](const Point<Leaf,T,Dims> &lhs, const Point<Leaf,T,Dims> &rhs){return Distance::distance(point,lhs) < Distance::distance(point,rhs);});
                            }
                        }
                        else
                        {
                            throw std::runtime_error("NNearestFinder::FindAtLeastNClosestPoints - Node is nullptr");
                        }
                    }

                public:
                    /**
                     * @brief Find N closest points in a tree.
                     * 
                     * @param node starting node (top of the tree)
                     * @param point point to match against
                     * @param nPoints number of closest points to look for
                     * @return a vector of N closest points or less (if there were not enough points)
                     */
                    std::vector<Point<Leaf,T,Dims> > Find(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, const Point<Leaf,T,Dims> &point, unsigned nPoints)
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
                    void FindWithinDistance(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node, const Point<Leaf,T,Dims> &point, std::vector<Point<Leaf,T,Dims> > &closestPoints, T distance)
                    {
                        if (node != nullptr)
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
                        else
                        {
                            throw std::runtime_error("DistanceFinder::FindWithinDistance - Node is nullptr");
                        }
                    }

                public:
                    /**
                     * @brief Find all points withing given distance.
                     * 
                     * @param node starting node (top of the tree)
                     * @param point point to match agains
                     * @param distance maximum distance
                     * @return a vector of all points within given distance 
                     */
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