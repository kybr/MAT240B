#include <cmath>
#include <complex>
#include <iostream>
#include <valarray>

#include "everything.h"

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

// Cooley–Tukey FFT (in-place, divide-and-conquer)
// Higher memory requirements and redundancy although more intuitive
void fft(CArray& x) {
  const size_t N = x.size();
  if (N <= 1) return;

  // divide
  CArray even = x[std::slice(0, N / 2, 2)];
  CArray odd = x[std::slice(1, N / 2, 2)];

  // conquer
  fft(even);
  fft(odd);

  // combine
  for (size_t k = 0; k < N / 2; ++k) {
    Complex t = std::polar(1.0, -2 * M_PI * k / N) * odd[k];
    x[k] = even[k] + t;
    x[k + N / 2] = even[k] - t;
  }
}

// inverse fft (in-place)
void ifft(CArray& x) {
  // conjugate the complex numbers
  x = x.apply(std::conj);

  // forward fft
  fft(x);

  // conjugate the complex numbers again
  x = x.apply(std::conj);

  // scale the numbers
  x /= x.size();
}

void fill_from_input(vector<float>& sample) {
  string line;
  getline(cin, line);
  while (line != "") {
    float f = stof(line);
    getline(cin, line);
    sample.push_back(f);
  }
}

int main() {
  vector<float> sample;
  fill_from_input(sample);

  const unsigned windowSize = 8192;  // 65536;
  const unsigned hopSize = windowSize / 4;

  CArray data;
  data.resize(windowSize);  // powers of 2 only!

  auto b2f = [](unsigned b) -> float {
    return (1.0f * b) / (windowSize / 2) * (44100 / 2);
  };

  for (unsigned n = 0; n + windowSize < sample.size(); n += hopSize) {
    float zcr = 0;
    for (unsigned i = 1; i < windowSize; i++) {
      // if neighboring samples have different sign, the a crossing happened
      if (sample[n + i] * sample[n + i - 1] < 0)  //
        zcr += 1.0f;
    }
    zcr /= windowSize;
    zcr /= 2;  // a sine crosses twice per cycle
    zcr *= 44400;
    printf("%f ", zcr);  // reference f0

    // XXX Sam: could we switch to calculating the *distance* between crossings?

    for (unsigned i = 0; i < windowSize; i++) {
      data[i] = Complex(sample[n + i], 0);
    }

    fft(data);

    double numerator = 0;
    double denominator = 0;
    for (unsigned i = 0; i < windowSize / 2 + 1; ++i) {
      numerator += abs(data[i]) * b2f(i);
      denominator += abs(data[i]);
    }
    float centroid = numerator / denominator;

    diy::say(centroid);
  }

  return 0;
}
