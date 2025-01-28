#ifndef Metrics_hxx
    #define Metrics_hxx

    #include <cmath>

    #include "Point.hxx"

    namespace JJDataStruct
    {  
       namespace KDTree
       {
            /**
             * @brief Structure representic a distance metric. Here is a squared euclidean distance
             * 
             */
            struct SquaredDist
            {
                template <typename Leaf, typename T , std::size_t Dims>
                static T distance(Point<Leaf,T,Dims> p1, Point<Leaf,T,Dims> p2)
                {
                    auto pow2 = [](T t){return t * t;};
                    T dist = 0;
                    for (std::size_t dim = 0; dim < Dims; ++dim)
                    {
                        dist += pow2(p1.coords[dim] - p2.coords[dim]);
                    }

                    return dist;
                }
            };

            /**
             * @brief Structure representic a distance metric. Here is a euclidean distance
             * 
             */
            struct RootSquaredDist
            {
                template <typename Leaf, typename T , std::size_t Dims>
                static T distance(Point<Leaf,T,Dims> p1, Point<Leaf,T,Dims> p2)
                {
                    auto pow2 = [](T t){return t * t;};
                    T dist = 0;
                    for (std::size_t dim = 0; dim < Dims; ++dim)
                    {
                        dist += pow2(p1.coords[dim] - p2.coords[dim]);
                    }

                    return std::sqrt(dist);
                }
            };

       } // namespace KDTree
        
    } // namespace JJDataStruct
    

#endif