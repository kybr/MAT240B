#include "al/app/al_App.hpp"
using namespace al;

#include <mlpack/methods/neighbor_search/neighbor_search.hpp>

typedef mlpack::neighbor::NeighborSearch<   //
    mlpack::neighbor::NearestNeighborSort,  //
    mlpack::metric::EuclideanDistance,      //
    arma::mat,                              //
    mlpack::tree::BallTree>                 //
    MyKNN;

int main(int argc, char* argv[]) {
  // make up some random data
  //
  arma::mat dataset(3, 1000, arma::fill::randu);

  // make up some data not in the corpus above
  //
  arma::mat query(3, 50, arma::fill::randu);

  // empty; filled in by the search
  //
  arma::mat distances;
  arma::Mat<size_t> neighbors;

  // execute the search
  //
  MyKNN myknn(dataset);
  myknn.Search(query, 1, neighbors, distances);

  //
  for (size_t i = 0; i < neighbors.n_elem; ++i) {
    std::cout << neighbors[i] << " " << distances[i] << std::endl;
  }
}
