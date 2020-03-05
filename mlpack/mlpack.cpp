// Karl Yerkes / 2020-02-20
//
// Example of using mlpack to do NN search of random data
//
#include <chrono>
#include <iostream>
#include <limits>
#include <mlpack/methods/neighbor_search/neighbor_search.hpp>

// helper class for calculating statistics
//
struct Stats : std::vector<double> {
  double minimum, maximum, mean, dev;

  void operator()(double value) { push_back(value); }

  void calculate() {
    minimum = std::numeric_limits<double>::max();
    maximum = -std::numeric_limits<double>::max();
    mean = 0;
    for (int i = 0; i < size(); i++) {
      mean += at(i);
      if (at(i) < minimum) minimum = at(i);
      if (at(i) > maximum) maximum = at(i);
    }
    mean /= size();

    dev = 0;
    for (int i = 0; i < size(); i++) {
      double v = at(i) - mean;
      dev += v * v;
    }
    dev /= size() - 1;
    dev = sqrt(dev);
  }

  void csv() {
    calculate();
    printf("%lf,%lf,%lf,%lf", minimum, maximum, mean, dev);
  }
};

// helper functions for measuring time
//
auto tic() { return std::chrono::high_resolution_clock::now(); }
double toc(std::chrono::high_resolution_clock::time_point then) {
  return std::chrono::duration<double>(tic() - then).count();
}

// type of nearest neighbor search index
//
typedef mlpack::neighbor::NeighborSearch<   //
    mlpack::neighbor::NearestNeighborSort,  //
    mlpack::metric::EuclideanDistance,      //
    arma::mat,                              //
    mlpack::tree::BallTree>                 //
    MyKNN;

// also try these:
//
// mlpack::tree::BallTree
// mlpack::tree::KDTree
// mlpack::tree::RStarTree
// mlpack::tree::RTree
// mlpack::tree::StandardCoverTree

int main() {
  int N = 1000;  // number of tries

  for (int d = 1; d <= 32; d *= 2)
    for (int n = pow(2, 8); n <= pow(2, 16); n *= 2) {
      arma::mat dataset(d, n, arma::fill::randu);

      Stats indexTime, queryTime;

      for (int i = 0; i < N; i++) {
        arma::mat query(d, 1, arma::fill::randu);
        arma::mat distances;
        arma::Mat<size_t> neighbors;

        // build index
        //
        auto indexThen = tic();
        MyKNN myknn(dataset);
        indexTime(toc(indexThen));

        // execute query
        //
        auto queryThen = tic();
        myknn.Search(query, 1, neighbors, distances);
        queryTime(toc(queryThen));
      }

      printf("%d,%d,", d, n);
      indexTime.csv();
      printf(",");
      queryTime.csv();
      printf("\n");
    }
}

