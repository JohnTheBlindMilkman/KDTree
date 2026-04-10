#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>

#include "KDTree.hxx"

// =====================================================================================================
// Settign up variables and aliases
// =====================================================================================================

template <typename Leaf, typename T, std::size_t Dims> using Point = JJDataStruct::KDTree::Point<Leaf,T,Dims>;
template <typename Leaf, typename T, std::size_t Dims, typename Distance> using Node = JJDataStruct::KDTree::Node<Leaf,T,Dims,Distance>;
template <typename Leaf, std::size_t Dims, typename T, typename Distance> using KDTree = JJDataStruct::KDTree::KDTree<Leaf,Dims,T,Distance>;
using SquaredDist = JJDataStruct::KDTree::SquaredDist;
using RootSquaredDist = JJDataStruct::KDTree::RootSquaredDist;

struct Event
{
    std::size_t id;
    double Xvertex,Yvertex,Zvertex;
    [[nodiscard]] bool operator==(const Event &other) const noexcept {return (id == other.id);}
};

struct OneDim
{
    std::size_t id;
    double x;
    [[nodiscard]] bool operator==(const OneDim &other) const noexcept {return (id == other.id);}
};

struct TwoDim
{
    std::size_t id;
    double x, y;
    [[nodiscard]] bool operator==(const TwoDim &other) const noexcept {return (id == other.id);}
};

struct ThreeDim
{
    std::size_t id;
    double x, y, z;
    [[nodiscard]] bool operator==(const ThreeDim &other) const noexcept {return (id == other.id);}
};

std::ostream& operator<<(std::ostream &stream,const JJDataStruct::KDTree::Point<Event,double,3> &pt)
{
    stream << "ID: " << pt.object.id << " [" << pt.coords.at(0) << "," << pt.coords.at(1) << "," << pt.coords.at(2) << "]";
    //stream << "ID: " << pt.object.id << " [" << pt.object.Xvertex << "," << pt.object.Yvertex << "," << pt.object.Zvertex << "]";
    return stream;
}

std::ostream& operator<<(std::ostream &stream,const JJDataStruct::KDTree::Point<OneDim,double,1> &pt)
{
    stream << "(" << pt.object.id << ", [" << pt.coords.at(0) << "])";
    return stream;
}

std::ostream& operator<<(std::ostream &stream,const JJDataStruct::KDTree::Point<TwoDim,double,2> &pt)
{
    stream << "(" << pt.object.id << ", [" << pt.coords.at(0) << "," << pt.coords.at(1) << "])";
    return stream;
}

std::ostream& operator<<(std::ostream &stream,const JJDataStruct::KDTree::Point<ThreeDim,double,3> &pt)
{
    stream << "(" << pt.object.id << ", [" << pt.coords.at(0) << "," << pt.coords.at(1) << "," << pt.coords.at(2) << "])";
    return stream;
}