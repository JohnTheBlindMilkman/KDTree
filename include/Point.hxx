#ifndef Point_hxx
    #define Point_hxx

    #include <array>

    namespace JJDataStruct
    {
        namespace KDTree
        {
            /**
             * @brief Structure representing a point in space with a payload
             * 
             * @tparam Leaf Object type that will be stored in leafs
             * @tparam T Arithmetic type of point coordinates
             * @tparam Dims Number of dimensions over which the data will be divided
             */
            template <typename Leaf, typename T , std::size_t Dims>
            struct Point
            {
                Leaf object;
                std::array<T,Dims> coords;
                [[nodiscard]] bool operator==(const Point<Leaf,T,Dims> &other) noexcept {return (object == other.object);}
            };

        } // namespace KDTree
        
    } // namespace JJDataStruct
    

#endif