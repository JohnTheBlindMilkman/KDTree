#ifndef Node_hxx
    #define Node_hxx

    #include <memory>
    #include <algorithm>
    #include <optional>
    #include <utility>

    #include "Metrics.hxx"
    #include "JJUtils.hxx"

    namespace JJDataStruct
    {
        namespace KDTree
        {
            /**
             * @brief Strucrure repsenting a node of a tree. It holds points (if it is at the bottom of the tree) or points to its two child nodes (if it is not at the bottom of the tree)
             * 
             * @tparam Leaf Object type that will be stored in leafs 
             * @tparam T Arithmetic type of point coordinates
             * @tparam Dims Number of dimensions over which the data will be divided 
             * @tparam Distance Metric upon which the distance will be calculated 
             */
            template <typename Leaf, typename T , std::size_t Dims, typename Distance>
            class Node
            {                    
                private:
                    bool m_isSplit;
                    T m_median;
                    std::size_t m_depth, m_dimensionIndex, m_bucketSize;
                    std::vector<Point<Leaf,T,Dims> > m_storedData;
                    std::shared_ptr<Node<Leaf,T,Dims,Distance> > m_leftNode, m_rightNode;
                    Node<Leaf,T,Dims,Distance> *m_parentNode; // non-owning pointer

                    void Split()
                    {
                        m_isSplit = true;

                        // sort the passed data
                        std::sort(m_storedData.begin(),m_storedData.end(),
                            [&](const Point<Leaf,T,Dims> &p1, const Point<Leaf,T,Dims> &p2)
                            {
                                return p1.coords[m_dimensionIndex] < p2.coords[m_dimensionIndex];
                            });

                        // calculate median for our data at given Dim
                        auto [leftData,rightData] = JJUtils::split<Point<Leaf,T,Dims> >(std::move(m_storedData));
                        m_storedData.resize(0);

                        if (leftData.size() == rightData.size()) // if the median is between the points
                        {
                            m_median = (leftData.back().coords.at(m_dimensionIndex) + rightData.front().coords.at(m_dimensionIndex)) / 2;
                        }
                        else // if the median is at point
                        {
                            m_median = rightData.front().coords.at(m_dimensionIndex);
                        }

                        m_leftNode = std::make_shared<Node<Leaf,T,Dims,Distance> >(std::move(leftData), this, m_depth + 1, m_bucketSize);
                        m_rightNode = std::make_shared<Node<Leaf,T,Dims,Distance> >(std::move(rightData), this, m_depth + 1, m_bucketSize);
                    }
                    void Join()
                    {
                        if (!m_leftNode->IsEmpty())
                        {
                            auto data = m_leftNode->GetData();
                            std::move(data.begin(),data.end(),std::back_inserter(m_storedData));
                        }
                        if (!m_rightNode->IsEmpty())
                        {
                            auto data = m_rightNode->GetData();
                            std::move(data.begin(),data.end(),std::back_inserter(m_storedData));
                        }

                        m_leftNode = nullptr;
                        m_rightNode = nullptr;
                        m_isSplit = false;
                        m_median = T();
                    }

                public:
                    Node(std::vector<Point<Leaf,T,Dims> > &&data, Node<Leaf,T,Dims,Distance> *parentNode, std::size_t depth ,std::size_t bucketSize): m_isSplit(false), m_median(T()), 
                        m_depth(depth), m_dimensionIndex(depth % Dims), m_bucketSize(bucketSize), m_storedData(std::move(data)),m_leftNode(nullptr), m_rightNode(nullptr), m_parentNode(parentNode)
                    {
                        if (m_storedData.size() > m_bucketSize)
                        {
                            Split();
                        }
                    }
                    [[nodiscard]] std::shared_ptr<Node<Leaf,T,Dims,Distance> > GetChild(const Point<Leaf,T,Dims> &point) const noexcept
                    {
                        if (point.coords.at(m_dimensionIndex) > m_median)
                        {
                            return GetRightNode();
                        }
                        else
                        {
                            return GetLeftNode();
                        } 
                    }
                    [[nodiscard]] std::shared_ptr<Node<Leaf,T,Dims,Distance> > GetOtherChild(const Point<Leaf,T,Dims> &point) const noexcept
                    {
                        if (point.coords.at(m_dimensionIndex) <= m_median)
                        {
                            return GetRightNode();
                        }
                        else
                        {
                            return GetLeftNode();
                        } 
                    }
                    [[nodiscard]] T CalculateDistanceToMedian(const Point<Leaf,T,Dims> &point) const noexcept
                    {
                        Point<Leaf,T,Dims> pointOnHyperplane{{},std::array<T,Dims>{}};
                        std::fill_n(pointOnHyperplane.coords.begin(),Dims,T(0));
                        pointOnHyperplane.coords.at(m_dimensionIndex) = m_median;
                        
                        return Distance::distance(point,pointOnHyperplane);
                    }
                    bool TryJoin()
                    {
                        if (!m_leftNode->IsSplit() && !m_rightNode->IsSplit() && (m_leftNode->size() + m_rightNode->size() <= m_bucketSize))
                        {
                            Join();
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    bool AddPoint(Point<Leaf,T,Dims> &&point)
                    {
                        if (m_isSplit)
                        {
                            return false;
                        }
                        else
                        {
                            m_storedData.push_back(std::move(point));
                            if (m_storedData.size() > m_bucketSize)
                            {
                                Split();
                            }
                            return true;
                        }
                    }
                    bool AddPoint(const Point<Leaf,T,Dims> &point)
                    {
                        if (m_isSplit)
                        {
                            return false;
                        }
                        else
                        {
                            m_storedData.push_back(point);
                            if (m_storedData.size() > m_bucketSize)
                            {
                                Split();
                            }
                            return true;
                        }
                    }
                    std::optional<Point<Leaf,T,Dims>> RemovePoint(const Point<Leaf,T,Dims> &point)
                    {
                        auto location = std::find(m_storedData.begin(),m_storedData.end(),point);

                        if (location != m_storedData.end())
                        {
                            Point<Leaf,T,Dims> removed_point = *location;
                            m_storedData.erase(std::remove(m_storedData.begin(),m_storedData.end(),point),m_storedData.end());
                            return removed_point;
                        }
                        else
                        {
                            return std::nullopt;
                        }
                    }
                    [[nodiscard]] std::optional<Point<Leaf,T,Dims> > FindNearest(const Point<Leaf,T,Dims> &point) noexcept
                    {
                        if (m_storedData.empty())
                        {
                            return std::nullopt;
                        }
                        else
                        {
                            std::sort(m_storedData.begin(),m_storedData.end(),[&point](const Point<Leaf,T,Dims> &lhs, const Point<Leaf,T,Dims> &rhs){return Distance::distance(point,lhs) < Distance::distance(point,rhs);});
                            return m_storedData.front();
                        }
                    }
                    [[nodiscard]] std::vector<Point<Leaf,T,Dims> > FindNNearest(const Point<Leaf,T,Dims> &point, unsigned nPoints)
                    {
                        if (m_storedData.empty())
                        {
                            return {};
                        }
                        else
                        {
                            std::sort(m_storedData.begin(),m_storedData.end(),[&point](const Point<Leaf,T,Dims> &lhs, const Point<Leaf,T,Dims> &rhs){return Distance::distance(point,lhs) < Distance::distance(point,rhs);});
                            return std::vector<Point<Leaf,T,Dims> >(m_storedData.begin(),(m_storedData.begin() + nPoints > m_storedData.end()) ? m_storedData.end() : m_storedData.begin() + nPoints);
                        }
                    }
                    [[nodiscard]] std::vector<Point<Leaf,T,Dims> > FindWithinDistance(const Point<Leaf,T,Dims> &point, T dist)
                    {
                        if (m_storedData.empty())
                        {
                            return {};
                        }
                        else
                        {
                            std::vector<Point<Leaf,T,Dims> > outVec;
                            std::copy_if(m_storedData.begin(),m_storedData.end(),std::back_inserter(outVec),[&point,&dist](const Point<Leaf,T,Dims> &pt){return Distance::distance(point,pt) <= dist;});
                            return outVec;
                        }
                    }
                    [[nodiscard]] inline std::vector<Point<Leaf,T,Dims> > GetData() const noexcept {return m_storedData;}
                    [[nodiscard]] inline std::size_t size() const noexcept {return m_storedData.size();}
                    [[nodiscard]] inline bool IsEmpty() const noexcept {return m_storedData.empty();}
                    [[nodiscard]] inline bool IsSplit() const noexcept {return m_isSplit;}
                    [[nodiscard]] inline T GetMedian() const noexcept {return m_median;}
                    [[nodiscard]] inline Node<Leaf,T,Dims,Distance>* GetParentNode() const noexcept {return m_parentNode;}
                    [[nodiscard]] inline std::shared_ptr<Node<Leaf,T,Dims,Distance> > GetLeftNode() const noexcept {return m_leftNode;}
                    [[nodiscard]] inline std::shared_ptr<Node<Leaf,T,Dims,Distance> > GetRightNode() const noexcept {return m_rightNode;}
            };

        } // namespace KDTree
        
    } // namespace JJDataStruct
    

#endif