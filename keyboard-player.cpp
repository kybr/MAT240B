// Karl Yerkes
//
// Example of...
// - using try_lock in the audio thread
// - using gam::SamplePlayer
// - sprintf
//
//

#include "Gamma/SamplePlayer.h"
#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
using namespace al;

#include <mutex>
using namespace std;

typedef gam::SamplePlayer<float, gam::ipl::Cubic, gam::phsInc::Loop>
    SoundPlayer;

struct MyApp : App {
  bool update{false};
  int which{0};
  float phase{0};
  float period{0};
  mutex m;  // protects the state above

  vector<SoundPlayer> playerList;

  void onInit() override {
    // onInit completes before audio is started!

    char fileName[100];
    for (int i = 0; i < 10; i++) {
      sprintf(fileName, "../sound/%d.wav", i);
      playerList.emplace_back();
      if (!playerList.back().load(fileName)) {
        printf("failed to load %s\n", fileName);
        exit(1);
      }
      printf("loaded %s with %d frames\n", fileName,
             playerList.back().frames());
    }

    gam::sampleRate(audioIO().framesPerSecond());
  }

  void onSound(AudioIOData& io) override {
    static SoundPlayer* player = nullptr;

    // use a "try lock" in the audio thread because we won't wait when we don't
    // get the lock; we just do nothing and move on. we'll do better next time.
    //
    if (m.try_lock()) {
      // we (the audio thread) have the lock so we know that our state
      // won't change until we release the lock.
      //
      if (update) {
        update = false;
        player = &playerList[which];
        player->range(phase, period);
        // printf("%d %f %f\n", which, player->pos(), player->period());
      }
      m.unlock();
    }

    while (io()) {
      float f = player == nullptr ? 0.0f : player->operator()();
      io.out(0) = io.out(1) = f * 0.1;
    }
  }

  bool onKeyDown(const Keyboard& k) override {
    // protect the critical data with a lock
    //
    m.lock();
    update = true;
    which = rnd::uniform(10);
    phase = rnd::uniform(1.0);
    period = rnd::uniform(0.3);
    // printf("%d %f %f\n", which, phase, period);
    m.unlock();
    return false;
  }
};

int main() {
  MyApp app;
  app.configureAudio(44100, 1024, 2, 0);
  app.start();
}
