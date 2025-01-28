# Modern C++ K-Dimensional Tree Data Structure

## Table of Contents
* [General Info](#general-info)
* [Technologies](#technologies)
* [Setup](#setup)
* [Benchmarks](#benchmarks)
* [Usage](#usage)
* [Current Limitations](#current-limitations)

## General Info
lorem ipsum

## Technologies
The project has been build using:
- OS: Ubuntu 20.04 LTS
- Compiler: gcc 9.4.0
- C++ standard: C++17
- Black Magic (most likely)

## Setup
It is very simple, you just add `#include "KDTree.hxx"` to your program and it will work!

## Benchmarks
In the future I would like to compare my KDTree to other KDTrees or maybe at lest to `std::map` and `std::unordered_map`.

## Usage
The whole tree is stronly based on templates to give the user a lot of freedom in terms of the data storage. In the future I'd like to make it work more like a STL container, but for now:

### Template Arguments
The KDTree has four template parameters:
- Leaf (mandatory) - object type that will be stored in the tree
- Dims (mandatory) - number of dimensions of the space we want to build the tree in
- T (default=double) - data type in which the points will be represented
- Distance (default=SquaredDistance) - metric that will be used to represent the distance between points; default is squared euclidean distance

Really what's needed is just the type of the stored object and the number of dimensions in our space:
```
KDTree<Object,N> tree;
```

The stored type can be arbitrary. With one limitation: it has to have an operator==, required by the point removal functinoality.

### Adding Points to the Tree
To add a point first you need to create the point itself. It is done as following:
```
Point<Object,T,N> point = {\*Object-class object*\,{\*coordinates as std::array<T,N> or std::initializer_list*\}};
```
Then it is as simple as calling `AddPoint` function:

```
tree.AddPoint(point);
```

The function will return `true` if the point was successfully added and `false` otherwise.

### Removing Points from the Tree
To remove a point simply call
```
tree.RemovePoint(point);
```

The function will return `true` if the point was successfully removed and `false` otherwise.

### Searching for Close Points
The points in a KDTree are partitioned for optimal search times. My KDTree offers three search functions:
- `KDTree::FindNearest(Point)` - tries ot find the closest point to `Point`
- `KDTree::FindNNearest(Point,n)` - tries to find `n` closest points to `Point`
- `KDTree::FindWithinDistance(Point,d)` - tries to find all points in a ball centered at `Point` with a radius equal to `d`

I wrote *tries* because it may return an empty point/vector of points of no such point was found.

The current implementations of those search algorithms are not perfect. Please see [this section](#current-limitations) to learn what are the current imperfections.

## Current Limitations
1. `KDTree::FindNNearest(Point,n)` and `KDTree::FindWithinDistance(Point,d)` currently will only look in a subtree which is only up to N nodes above the leaf node; where N - dimensions of the space proveided as tmplate argument to `KDTree`. If one specifies more points to be found than there are points in this subtree it will not extend the search to higher nodes. It is not what we should expect, but I am still too dumb to implment this in an elegant way. If you are more knowledgeable than me, and would like to solve to this issue, please submit a pull request.
2. I'm not using concepts, as for now I am keeping this project in C++17. I am also not fluent in elvish (a.k.a. template metaprogramming) so no SFINAE trickery is implemented in here to stop you from breaking the KDTree. Please be cautious.
3. The current tests ~~cover more cases than half of the repos here~~ are very limited and very much work in progress. They just take a lot of time finish, but I'm updating them consistently. Also the fact that this is a template class does not help me.