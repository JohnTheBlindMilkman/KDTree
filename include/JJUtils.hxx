#include <tuple>
#include <vector>
#include <algorithm>

#ifndef JJUtils_hxx
    #define JJUtils_hxx

    namespace JJUtils
    {
        template<typename T>
        std::pair<std::vector<T>,std::vector<T> > split(std::vector<T> &&vec)
        {
            std::vector<T> left, right;
            std::move(vec.begin()+vec.size()/2,vec.end(),std::back_inserter(right));
            left = std::move(vec);
            left.resize(left.size()-right.size());

            return std::make_pair(left,right);
        }
    } // namespace JJUtils

#endif
