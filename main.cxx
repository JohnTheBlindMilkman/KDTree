#include <iostream>
#include <random>

#include "KDTree.hxx"

struct Event
{
    std::size_t id;
    double Xvertex,Yvertex,Zvertex;
    [[nodiscard]] bool operator==(const Event &other) noexcept {return (id == other.id);}
};

std::ostream& operator<<(std::ostream &stream,const JJDataStruct::KDTree::Point<Event,double,3> &pt)
{
    stream << "ID: " << pt.object.id << " [" << pt.coords.at(0) << "," << pt.coords.at(1) << "," << pt.coords.at(2) << "]";
    //stream << "ID: " << pt.object.id << " [" << pt.object.Xvertex << "," << pt.object.Yvertex << "," << pt.object.Zvertex << "]";
    return stream;
}

std::ostream& operator<<(std::ostream &stream,const std::vector<JJDataStruct::KDTree::Point<Event,double,3> > &pts)
{
    for (const auto &point : pts)
        stream << point << "\t";

    return stream;
}


int main()
{
    constexpr std::size_t nPoints{10000};
    constexpr float min{-5};
    constexpr float max{5};
    constexpr float step = (std::abs(min) + std::abs(max)) / nPoints;
    Event evt;

    /* std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> norm(0,5); */

    JJDataStruct::KDTree::KDTree<Event,3> tree(32);

    for (std::size_t i = 0; i < nPoints; ++i)
    {
        evt = {i,min+(i*step),max-(i*step),2*min+(2*i*step)};
        tree.AddPoint({evt,{evt.Xvertex,evt.Yvertex,evt.Zvertex}});
    }
    tree.SplitTree();
    //tree.Print();

    Event evt2{nPoints+1,1,-1,2};
    JJDataStruct::KDTree::Point<Event,double,3> point = {evt2,{evt2.Xvertex,evt2.Yvertex,evt2.Zvertex}};
    std::cout << tree.size() << "\n";
    std::cout << "Our point: " << point << "\n";
    std::cout << "Closest point: " << tree.FindNearest(point) << "\n";
    std::cout << "Closest points: " << tree.FindNNearest(point,5) << "\n";
    std::cout << "Closest points: " << tree.FindWithinDistance(point,0.0001) << "\n";
    std::cout << std::boolalpha << tree.AddPoint(std::move(point)) <<"\n";
    std::cout << std::boolalpha << tree.RemovePoint(point) <<"\n"; 
    //tree.Print();

    /* std::cout << "=============================================================\n";
    for (std::size_t i = 0; i < nPoints; ++i)
    {
        evt = {i,norm(gen),norm(gen),norm(gen)};
        tree.AddPoint({evt,{evt.Xvertex,evt.Yvertex,evt.Zvertex}});
    }
    tree.Print(); */
}