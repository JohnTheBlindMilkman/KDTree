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

        /**
         * @brief Container which acts as a FILO queue with a certain size limit. It will add new elements at the back and pop elements at the front once the size of the container has exceeded the maximal size of the buffer
         * 
         * @tparam T the type of the elements
         */
        template <typename T>
        class fixed_size_deque
        {
            public:
                /**
                 * @brief Default c-tor. The max buffer size is set to max value of std::size_t
                 * 
                 */
                fixed_size_deque() : m_maxSize(std::numeric_limits<std::size_t>::max()), m_deque() {}
                /**
                 * @brief Construct a new fixed size deque object with a given buffer size
                 * 
                 * @param maxSize maximal size of the deque, before it starts to pop elememnts
                 */
                fixed_size_deque(std::size_t maxSize) : m_maxSize(maxSize), m_deque() {}
                /**
                 * @brief Copy c-tor
                 * 
                 * @param fsd other fixed_size_deque object
                 */
                fixed_size_deque(const fixed_size_deque& fsd) : m_maxSize(fsd.m_maxSize), m_deque(fsd.m_deque) {}
                /**
                 * @brief Move c-tor
                 * 
                 * @param fsd other fixed_size_deque object
                 */
                fixed_size_deque(fixed_size_deque &&fsd) : m_maxSize(std::move(fsd.m_maxSize)) m_deque(std::move(fsd.m_deque)) {}
                /**
                 * @brief Get the number of elements
                 * 
                 * @return std::size_t 
                 */
                inline std::size_t size() const {return m_deque.size();}
                /**
                 * @brief Get the iterator pointing at the beginnig of the container
                 * 
                 * @return std::deque<T>::iterator 
                 */
                std::deque<T>::iterator begin() {return m_deque.begin();}
                /**
                 * @brief Get the iterator pointing at the beginnig of the container
                 * 
                 * @return std::deque<T>::const_iterator 
                 */
                std::deque<T>::const_iterator begin() const {return m_deque.begin();}
                /**
                 * @brief Get the iterator pointing at the one-after-last element of the container
                 * 
                 * @return std::deque<T>::iterator 
                 */
                std::deque<T>::iterator end() {return m_deque.end();}
                /**
                 * @brief Get the iterator pointing at the one-after-last element of the container
                 * 
                 * @return std::deque<T>::const_iterator 
                 */
                std::deque<T>::const_iterator end() const {return m_deque.end();}
                /**
                 * @brief Add a new element at the end. This method will also call std::deque::pop_front if the size of the container exceeds the max buffer size
                 * 
                 * @param value 
                 */
                void push_back(const T &value) {m_deque.push_back(value); if (m_deque.size() > m_maxSize) m_deque.pop_front();}
                /**
                 * @brief Add a new element at the end. This method will also call std::deque::pop_front if the size of the container exceeds the max buffer size
                 * 
                 * @param value 
                 */
                void push_back(T &&value) {m_deque.push_back(std::move(value)); if (m_deque.size() > m_maxSize) m_deque.pop_front();}
                /**
                 * @brief Check if the container is empty
                 * 
                 * @return true is empty and
                 * @return false otherwise
                 */
                bool empty() const {return m_deque.empty();}

            private:
                /**
                 * @brief Max buffer size of the container
                 * 
                 */
                std::size_t m_maxSize;
                /**
                 * @brief Underlying container
                 * 
                 */
                std::deque<T> m_deque;
        };

    } // namespace JJUtils

#endif
