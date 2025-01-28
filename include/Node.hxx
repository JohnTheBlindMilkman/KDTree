#ifndef Node_hxx
    #define Node_hxx

    #include <memory>
    #include <algorithm>

    #include "Metrics.hxx"
    #include "JJUtils.hxx"

    namespace JJDataStruct
    {
        namespace KDTree
        {
            /**
             * @brief Strucrure repsenting a node of a tree. It holds points (if it is at the bottom og the tree) or points to its two child nodes (if it is not at the bottom of the tree)
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

                public:
                    Node(std::vector<Point<Leaf,T,Dims> > &&data, std::size_t depth ,std::size_t bucketSize)
                    : m_isSplit(false), 
                    m_median(T()), 
                    m_depth(depth),
                    m_dimensionIndex(depth % Dims), 
                    m_bucketSize(bucketSize), 
                    m_storedData(std::move(data)), 
                    m_leftNode(nullptr), 
                    m_rightNode(nullptr)
                    {
                        if (m_storedData.size() > m_bucketSize)
                        {
                            Split();
                        }
                    }
                    void Split()
                    {
                        m_isSplit = true;

                        // sort the passed data
                        std::sort(m_storedData.begin(),m_storedData.end(),
                            [&](const Point<Leaf,T,Dims> &p1, const Point<Leaf,T,Dims> &p2)
                            {
                                return p1.coords.at(m_dimensionIndex) < p2.coords.at(m_dimensionIndex);
                            });

                        // calculate median for our data at given Dim
                        std::pair<std::vector<Point<Leaf,T,Dims> >,std::vector<Point<Leaf,T,Dims> > > dataPair = JJUtils::split<Point<Leaf,T,Dims> >(std::move(m_storedData));
                        // if the median is between the points
                        if (dataPair.first.size() == dataPair.second.size())
                        {
                            m_median = (dataPair.first.back().coords.at(m_dimensionIndex) + dataPair.second.front().coords.at(m_dimensionIndex)) / 2;
                        }
                        // if the median is at point
                        else
                        {
                            m_median = dataPair.second.front().coords.at(m_dimensionIndex);
                        }

                        m_leftNode = std::make_shared<Node<Leaf,T,Dims,Distance> >(std::move(dataPair.first),m_depth + 1,m_bucketSize);
                        m_rightNode = std::make_shared<Node<Leaf,T,Dims,Distance> >(std::move(dataPair.second),m_depth + 1,m_bucketSize);
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
                    bool RemovePoint(const Point<Leaf,T,Dims> &point)
                    {   
                        const auto last = std::remove(m_storedData.begin(),m_storedData.end(),point);
                        if (last == m_storedData.end())
                            return false;
                        m_storedData.erase(last,m_storedData.end());
                        return true;
                    }
                    [[nodiscard]] Point<Leaf,T,Dims> FindNearest(const Point<Leaf,T,Dims> &point) noexcept
                    {
                        if (m_storedData.empty())
                        {
                            return {};
                        }
                        else
                        {
                            std::sort(m_storedData.begin(),m_storedData.end(),[&point](const Point<Leaf,T,Dims> &lhs, const Point<Leaf,T,Dims> &rhs){return Distance::distance(point,lhs) < Distance::distance(point,rhs);});
                            return m_storedData.front();
                        }
                    }
                    std::vector<Point<Leaf,T,Dims> > FindNNearest(const Point<Leaf,T,Dims> &point, std::size_t nPoints)
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
                    std::vector<Point<Leaf,T,Dims> > FindWithinDistance(const Point<Leaf,T,Dims> &point, T dist)
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
                    [[nodiscard]] inline bool IsSplit() const noexcept {return m_isSplit;}
                    [[nodiscard]] inline T GetMedian() const noexcept {return m_median;}
                    [[nodiscard]] inline std::shared_ptr<Node<Leaf,T,Dims,Distance> > GetLeftNode() const noexcept {return m_leftNode;}
                    [[nodiscard]] inline std::shared_ptr<Node<Leaf,T,Dims,Distance> > GetRightNode() const noexcept {return m_rightNode;}      
            };

        } // namespace KDTree
        
    } // namespace JJDataStruct
    

#endif