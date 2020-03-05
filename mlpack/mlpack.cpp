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
template <class T>
class MMM {
  size_t size;
  T sum{0};
  T minimum{std::numeric_limits<T>::max()};
  T maximum{-std::numeric_limits<T>::max()};

 public:
  void operator()(T value) {
    ++size;
    sum += value;
    if (value < minimum) minimum = value;
    if (value > maximum) maximum = value;
  }
  T mean() { return sum / size; }

  void print() {
    printf("min:%lf max:%lf mean:%lf\n", minimum, maximum, mean());
  }
  void csv() { printf("%lf,%lf,%lf", minimum, maximum, mean()); }
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

      MMM<double> indexTime, queryTime;

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

