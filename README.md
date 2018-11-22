# k-max Distance

The k-max distance for images and graphs.
The repository contains the source code of the k-max distance and also three samples how to use it.

## Requirements

* c++ std11
* opencv

## Usage
```
    cmake .
    make
```
##### The distance computed in images using *Image* structure
```
./main 
```

#####  The distance computed in images using C arrays
```
./main2
```
#####  The distance computed in graphs using *Graph* structure
```
./main3
```
In all the examples, the function `compute` is used for the k-max distance computing. <br />
This function requires the Image structure (`main.cpp`), Graph structure (`main3.cpp`), or arrays with the edge costs in x and y directions (`main2.cpp`).
In addition, the array of source nodes, the number of source nodes, and the destination point (if a negative value is used, the distance is computed to all the image nodes and the distance to the destination node is returned) are required.
The function returns the distance to the destination point.
Optionally, the distance transform (the negative value of destination point is required), the shortest path, and the maximum vector that gives the distance can be returned from the function.

## Publication

Michael Holuša, Eduard Sojka, The k-max distance in graphs and images, In Pattern Recognition Letters, Volume 98, 2017, Pages 103-109, ISSN 0167-8655, https://doi.org/10.1016/j.patrec.2017.09.003.
(http://www.sciencedirect.com/science/article/pii/S0167865517303148)

### BibTeX

```
@article{holusa2017,
title = {The k-max distance in graphs and images},
author = {Michael Holuša and Eduard Sojka},
journal = {Pattern Recognition Letters},
volume = {98},
pages = {103 - 109},
year = {2017},
issn = {0167-8655},
doi = {https://doi.org/10.1016/j.patrec.2017.09.003},
url = {http://www.sciencedirect.com/science/article/pii/S0167865517303148},
keywords = "Distance measuring, Shortest path, -max distance, Geodesic distance, Image processing, Image segmentation",
}
```
