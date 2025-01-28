/**
 * @file KDTree.hxx
 * @author Jędrzej Kołaś (jedrzej.kolas.dokt@pw.edu.pl)
 * @brief 
 * @version 0.1.0
 * @date 2024-12-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef KDTree_hxx
    #define KDTree_hxx

    #include <iostream>
    #include <functional>

    #include "Actions.hxx"

    namespace JJDataStruct
    {
        namespace KDTree
        {
            /**
             * @brief K-Dimensional Tree class
             * 
             * @tparam Leaf Object type that will be stored in leafs
             * @tparam Dims Number of dimensions over which the data will be divided
             * @tparam T Arithmetic type of point coordinates
             * @tparam Distance Metric upon which the distance will be calculated
             */
            template <typename Leaf, std::size_t Dims, typename T = double, typename Distance = SquaredDist> 
            class KDTree
            {
                private:
                    bool m_isSplit;
                    std::size_t m_bucketSize, m_maxSizeBeforeSplit;
                    std::vector<Point<Leaf,T,Dims> > m_storedData;
                    std::shared_ptr<Node<Leaf,T,Dims,Distance> > m_rootNode;
                    Inserter<Leaf,T,Dims,Distance> m_inserter;
                    Deleter<Leaf,T,Dims,Distance> m_deleter;
                    NearestFinder<Leaf,T,Dims,Distance> m_nearestFinder;
                    NNearestFinder<Leaf,T,Dims,Distance> m_nNearestFinder;
                    DistanceFinder<Leaf,T,Dims,Distance> m_distanceFinder;

                    void Print(const std::string &prefix, std::shared_ptr<Node<Leaf,T,Dims,Distance> > node, bool isLeft) const
                    {
                        if( node != nullptr )
                        {
                            std::cout << prefix;

                            std::cout << (isLeft ? "├──" : "└──" );

                            // print the value of the node
                            if (node->IsSplit())
                            {
                                std::cout << node->GetMedian() << "\n";
                            }
                            else
                            {
                                std::cout << "[";
                                for (const auto &elem : node->GetData())
                                {
                                    std::cout << elem.object.id << ", ";
                                }
                                std::cout << "]\n";
                            }
                            //std::cout << (node->IsSplit() ? node->GetMedian() : node->size()) << std::endl;

                            // enter the next tree level - left and right branch
                            Print( prefix + (isLeft ? "│   " : "    "), node->GetLeftNode(), true);
                            Print( prefix + (isLeft ? "│   " : "    "), node->GetRightNode(), false);
                        }
                    }

                public:
                    /**
                     * @brief Construct a new KDTree object
                     * 
                     * @param bucketSize the maximal size of each bucket before it splits into two nodes
                     * @param maxSize the maximal size of points which will be stored in the KDTree before it splits (it is better to first collect many points and only split the tree after; this will result in a more balanced tree)
                     * @param data data that can be passed into the tree at construction (or use AddPoint method to add them later)
                     */
                    constexpr KDTree(std::size_t bucketSize, std::size_t maxSize = 10000, std::vector<Point<Leaf,T,Dims> > data = {}) 
                    : m_isSplit(false),
                    m_bucketSize(bucketSize), 
                    m_maxSizeBeforeSplit(maxSize), 
                    m_storedData(data), 
                    m_rootNode(nullptr),
                    m_inserter(),
                    m_deleter(),
                    m_nearestFinder(),
                    m_nNearestFinder(),
                    m_distanceFinder()
                    {
                        if (m_storedData.size() > maxSize)
                            SplitTree();
                    }
                    /**
                     * @brief Split the tree. 
                     * It is better to first collect many points and only split the tree after. This will result in a more balanced tree.
                     * 
                     */
                    void SplitTree()
                    {
                        m_isSplit = true;
                        if (m_rootNode == nullptr)
                        {
                            m_rootNode = std::make_shared<Node<Leaf,T,Dims,Distance> >(std::move(m_storedData),0,m_bucketSize);
                        }
                    }
                    /**
                     * @brief Add point to the tree
                     * 
                     * @param point 
                     * @return true if point could be added or
                     * @return false otherwise
                     */
                    bool AddPoint(Point<Leaf,T,Dims> &&point)
                    {
                        if (m_rootNode == nullptr)
                        {
                            m_storedData.push_back(std::move(point));
                            if (m_storedData.size() > m_maxSizeBeforeSplit)
                                SplitTree();

                            return true;
                        }
                        else
                        {
                            return m_inserter.Insert(m_rootNode,point);
                        }
                    }
                    /**
                     * @brief Remove point from the tree. Uses operator== of the stored Leaf-type object (so if you use your own class/struct, you have to implement it yourself)
                     * 
                     * @param point 
                     * @return true if point was removed or
                     * @return false otherwise
                     */
                    bool RemovePoint(const Point<Leaf,T,Dims> &point)
                    {
                        if (m_rootNode == nullptr)
                        {
                            const auto last = std::remove(m_storedData.begin(),m_storedData.end(),point);
                            if (last == m_storedData.end())
                                return false;
                            m_storedData.erase(last,m_storedData.end());
                            return true;
                        }
                        else
                        {
                            return m_deleter.Remove(m_rootNode,point);
                        }
                    }
                    /**
                     * @brief Find the nearest point. Utilises the KDTree unique structure to search only in the vicinity of provided point
                     * 
                     * @param pt Point to which the distance should be the smallest
                     * @return Point<Leaf,T,Dims> 
                     */
                    [[nodiscard]] Point<Leaf,T,Dims> FindNearest(const Point<Leaf,T,Dims> &pt)
                    {
                        if (m_rootNode == nullptr)
                        {
                            std::cerr << "Tree is not split\n";
                            return {};
                        }
                        else
                        {
                            return m_nearestFinder.Find(m_rootNode,pt);
                        }
                    }
                    /**
                     * @brief Find the N nearest points
                     * 
                     * @param pt Point to which the distance should be the smallest
                     * @param nPoints Number of closest points
                     * @return std::vector<Point<Leaf,T,Dims> > 
                     */
                    [[nodiscard]] std::vector<Point<Leaf,T,Dims> > FindNNearest(const Point<Leaf,T,Dims> &pt, std::size_t nPoints)
                    {
                        if (m_rootNode == nullptr)
                        {
                            std::cerr << "Tree is not split\n";
                            return {};
                        }
                        else
                        {
                            return m_nNearestFinder.Find(m_rootNode,pt,nPoints);
                        }
                    }
                    /**
                     * @brief Find all points within distance
                     * 
                     * @param point Reference point (center of the sphere)
                     * @param dist Maximal distance from point (radius of the sphere)
                     * @return std::vector<Point<Leaf,T,Dims> > 
                     */
                    [[nodiscard]] std::vector<Point<Leaf,T,Dims> > FindWithinDistance(const Point<Leaf,T,Dims> &point, T dist)
                    {
                        if (m_rootNode == nullptr)
                        {
                            std::cerr << "Tree is not split\n";
                            return {};
                        }
                        else
                        {
                            return m_distanceFinder.Find(m_rootNode,point,dist);
                        }
                    }
                    /**
                     * @brief Print the structure of the whole tree. If a given node is a leaf node. then the value shown represents the size of the data vector. If a give node is not a leaf node, then the median value is shown.
                     * 
                     */
                    void Print() const 
                    {
                        Print("",m_rootNode,false);
                    }
                    /**
                     * @brief Returns number of stored points within the K-D Tree.
                     * 
                     * @return std::size_t 
                     */
                    [[nodiscard]] inline std::size_t size() const noexcept 
                    {
                        if (m_rootNode == nullptr)
                        {
                            return m_storedData.size();
                        }
                        else
                        {
                            std::function<std::size_t(const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &)> sumElems;
                            sumElems = [&sumElems](const std::shared_ptr<Node<Leaf,T,Dims,Distance> > &node)
                            {
                                if (node->IsSplit())
                                {
                                    return sumElems(node->GetLeftNode()) + sumElems(node->GetRightNode());
                                }
                                else
                                {
                                    return node->size();
                                }
                            };

                            return sumElems(m_rootNode);
                        }
                    }
                    /**
                     * @brief Returns the max amout of points each leaf node can store
                     * 
                     * @return std::size_t 
                     */
                    [[nodiscard]] inline constexpr std::size_t GetBucketSize() const noexcept {return m_bucketSize;}
                    /**
                     * @brief Returns the maximal number of points which can be stored, before the KDTree will split automatically
                     * 
                     * @return constexpr std::size_t 
                     */
                    [[nodiscard]] inline constexpr std::size_t GetMaxSizeBeforeSplit() const noexcept {return m_maxSizeBeforeSplit;}
                    /**
                     * @brief Check if the KDTree is split
                     * 
                     * @return true if is split
                     * @return false otherwise
                     */
                    [[nodiscard]] inline constexpr bool IsSplit() const noexcept {return m_isSplit;}

            };  
            
        } // namespace KDTree
        
    } // namespace JJDataStruct

#endif