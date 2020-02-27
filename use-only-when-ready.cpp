#include "al/app/al_App.hpp"
#include "al/sound/al_SoundFile.hpp"
using namespace al;

#include <iostream>
using namespace std;

struct MyApp : App {
  SoundFile soundFile;
  int index{0};
  bool loaded{false};

  void onSound(AudioIOData& io) override {
    while (io()) {
      float f = 0;

      // use file data; only when ready
      //
      if (loaded) {
        f = soundFile.data[index];
        index++;
        if (index > soundFile.data.size())  //
          index = 0;
      }

      io.out(0) = io.out(1) = f;
    }
  }

  bool onKeyDown(const Keyboard& k) override {
    // load file; rather "start loading file"
    //
    if (!loaded)
      if (!soundFile.open("../Guitar-Tuner-Example.wav")) {
        exit(1);
      }
    loaded = true;
    cout << soundFile.data.size() << " samples found" << endl;
    return false;
  }
};

int main() {
  MyApp app;
  app.start();
}
