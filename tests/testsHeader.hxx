#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>

#include "KDTree.hxx"

// =====================================================================================================
// Settign up variables and aliases
// =====================================================================================================

template <typename Leaf, typename T, std::size_t Dims>
using Point = JJDataStruct::KDTree::Point<Leaf,T,Dims>;
template <typename Leaf, typename T, std::size_t Dims, typename Distance>
using Node = JJDataStruct::KDTree::Node<Leaf,T,Dims,Distance>;
template <typename Leaf, std::size_t Dims, typename T, typename Distance>
using KDTree = JJDataStruct::KDTree::KDTree<Leaf,Dims,T,Distance>;
using SquaredDist = JJDataStruct::KDTree::SquaredDist;
using RootSquaredDist = JJDataStruct::KDTree::RootSquaredDist;

struct Event
{
    std::size_t id;
    double Xvertex,Yvertex,Zvertex;
    bool operator==(const Event &other) {return (id == other.id);}
};

std::ostream& operator<<(std::ostream &stream,const JJDataStruct::KDTree::Point<Event,double,3> &pt)
{
    stream << "ID: " << pt.object.id << " [" << pt.coords.at(0) << "," << pt.coords.at(1) << "," << pt.coords.at(2) << "]";
    //stream << "ID: " << pt.object.id << " [" << pt.object.Xvertex << "," << pt.object.Yvertex << "," << pt.object.Zvertex << "]";
    return stream;
}