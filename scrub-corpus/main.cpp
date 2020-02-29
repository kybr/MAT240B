#include "al/app/al_App.hpp"
#include "al/sound/al_SoundFile.hpp"
#include "al/spatial/al_HashSpace.hpp"
#include "al/ui/al_ControlGUI.hpp"  // gui.draw(g)
using namespace al;

#include "Gist.h"

#include <vector>
using namespace std;

const int sampleRate = 44100;
const int frameSize = 1024;
const int hopSize = frameSize / 4;

static Gist<float> gist(frameSize, sampleRate);

struct Appp : App {
  Parameter p1{"/p1", "", 0.5, "", 0.0, 1.0};
  Parameter p2{"/p2", "", 0.5, "", 0.0, 1.0};
  Parameter p3{"/p3", "", 0.5, "", 0.0, 1.0};
  Parameter radius{"/radius", "", 0.5, "", 0.0, 1.0};
  ParameterBool mic{"mic", "", 1.0};
  ControlGUI gui;

  vector<float> sample;   // contain all sample data
  vector<Vec3f> feature;  //

  HashSpace space;
  Mesh mesh;
  Mesh line;

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

    if (sample.size() < frameSize)  //
      exit(1);
  }

  // choose between features here:
  //
  float f1() { return gist.complexSpectralDifference(); }
  float f2() { return gist.spectralCentroid(); }
  float f3() { return gist.pitch(); }

  Vec3f minimum{1e30f}, maximum{-1e30f};
  void onCreate() override {
    gui << p1 << p2 << p3 << radius << mic;
    gui.init();
    navControl().useMouse(false);

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

    line.primitive(Mesh::LINES);
    line.vertex(0, 0, 0);
    line.vertex(1, 1, 1);

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

  // doesn't start until App::start() is called
  //
  void onSound(AudioIOData& io) override {
    Vec3f v;
    if (mic) {
      gist.processAudioFrame(io.inBuffer(0), frameSize);

      v.set((f1() - minimum.x) / (maximum.x - minimum.x),
            (f2() - minimum.y) / (maximum.y - minimum.y),
            (f3() - minimum.z) / (maximum.z - minimum.z));
      // XXX check if v is out of bounds? maybe re-position all points

    } else {
      v.set(p1, p2, p3);
    }

    if (line.vertices().size())  //
      line.vertices()[0] = v;

    v *= space.dim();

    HashSpace::Query query(1);
    if (query(space, v, radius * space.maxRadius())) {
      float* frame = &sample[query[0]->id * frameSize];
      for (int i = 0; i < frameSize; i++)  //
        io.outBuffer(0)[i] = frame[i];
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
    g.draw(line);
    gui.draw(g);
  }
};

int main(int argc, char* argv[]) {
  Appp app(argc, argv);  // blocks until contructor is complete
  app.audioDomain()->configure(44100, frameSize, 2, 2);
  app.start();  // blocks; hand over control to the framework
}
