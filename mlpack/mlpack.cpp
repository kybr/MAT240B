// Karl Yerkes / 2020-02-20
//
// Example of using mlpack to do NN search of random data
//
#include <iostream>
#include <mlpack/methods/neighbor_search/neighbor_search.hpp>

#include <chrono>

typedef mlpack::neighbor::NeighborSearch<   //
    mlpack::neighbor::NearestNeighborSort,  //
    mlpack::metric::EuclideanDistance,      //
    arma::mat,                              //
    mlpack::tree::BallTree>                 //
    MyKNN;

// tree types:
// mlpack::tree::BallTree
// mlpack::tree::KDTree
// mlpack::tree::RStarTree
// mlpack::tree::RTree
// mlpack::tree::StandardCoverTree

int main() {
  int N = 100;
  for (int d = 1; d < 32; d *= 2)
    for (int n = pow(2, 8); n < pow(2, 15); n *= 2) {
      arma::mat dataset(d, n, arma::fill::randu);

      double mean = 0, minimum = 100, maximum = 0;
      for (int i = 0; i < N; i++) {
        arma::mat query(d, 1, arma::fill::randu);
        arma::mat distances;
        arma::Mat<size_t> neighbors;

        auto then = std::chrono::high_resolution_clock::now();
        MyKNN myknn(dataset);
        myknn.Search(query, 1, neighbors, distances);

        double t = std::chrono::duration<double>(
                       std::chrono::high_resolution_clock::now() - then)
                       .count();
        if (t < minimum) minimum = t;
        if (t > maximum) maximum = t;
        mean += t;
      }
      mean /= N;

      printf("%d,%d,%f,%f,%f\n", d, n, minimum, mean, maximum);
    }
}

