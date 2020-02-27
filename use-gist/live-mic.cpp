#include "al/app/al_App.hpp"
#include "al/sound/al_SoundFile.hpp"
#include "al/spatial/al_HashSpace.hpp"
using namespace al;

#include "Gist.h"

#include <vector>
using namespace std;

const int sampleRate = 44100;
const int frameSize = 1024;
const int hopSize = frameSize / 4;

static Gist<float> gist(frameSize, sampleRate);

struct Appp : App {
  vector<float> sample;
  vector<Vec3f> feature;

  HashSpace space;
  Mesh mesh;

  Appp(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
      SoundFile soundFile;
      if (!soundFile.open(argv[i]))  //
        exit(1);
      if (soundFile.channels > 1)  //
        exit(1);
      for (int i = 0; i < soundFile.data.size(); i++)  //
        sample.push_back(soundFile.data[i]);
    }

    if (sample.size() == 0)  //
      exit(1);
  }

  // choose between features here:
  //
  float f1() { return gist.complexSpectralDifference(); }
  float f2() { return gist.spectralCentroid(); }
  float f3() { return gist.pitch(); }

  Vec3f minimum{1e30f}, maximum{-1e30f};
  void onCreate() override {
    for (int n = 0; n + frameSize < sample.size(); n += frameSize) {
      gist.processAudioFrame(&sample[n], frameSize);
      Vec3f v(f1(), f2(), f3());
      if (v.x > maximum.x) maximum.x = v.x;
      if (v.y > maximum.y) maximum.y = v.y;
      if (v.z > maximum.z) maximum.z = v.z;
      if (v.x < minimum.x) minimum.x = v.x;
      if (v.y < minimum.y) minimum.y = v.y;
      if (v.z < minimum.z) minimum.z = v.z;
      feature.push_back(v);
    }

    space = HashSpace(6, feature.size());

    float dim = space.dim();

    mesh.primitive(Mesh::POINTS);

    for (int n = 0; n < feature.size(); n++) {
      feature[n].x = (feature[n].x - minimum.x) / (maximum.x - minimum.x);
      feature[n].y = (feature[n].y - minimum.y) / (maximum.y - minimum.y);
      feature[n].z = (feature[n].z - minimum.z) / (maximum.z - minimum.z);
      mesh.vertex(feature[n]);
      feature[n] *= dim;
      space.move(n, feature[n].x, feature[n].y, feature[n].z);
    }
  }

  void onSound(AudioIOData& io) override {
    gist.processAudioFrame(io.inBuffer(0), frameSize);

    float dim = space.dim();
    Vec3f v(dim * (f1() - minimum.x) / (maximum.x - minimum.x),
            dim * (f2() - minimum.y) / (maximum.y - minimum.y),
            dim * (f3() - minimum.z) / (maximum.z - minimum.z));

    HashSpace::Query query(1);
    if (query(space, v, space.maxRadius())) {
      float* window = &sample[query[0]->id * frameSize];
      for (int i = 0; i < frameSize; i++)  //
        io.outBuffer(0)[i] = window[i];
      return;
    }

    while (io()) {
      float f = 0;
      io.out(0) = f;
      io.out(1) = f;
    }
  }

  void onDraw(Graphics& g) override {
    g.clear(Color(0.21));

    g.draw(mesh);
  }
};

int main(int argc, char* argv[]) {
  Appp app(argc, argv);
  app.audioDomain()->configure(44100, frameSize, 2, 2);
  app.start();
}
