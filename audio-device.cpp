#include "al/app/al_App.hpp"
using namespace al;

struct MyApp : App {
  void onSound(AudioIOData& io) override {
    float sum = 0;
    while (io()) {
      float f = io.in(0) + io.in(1);
      sum += f > 0 ? f : -f;
      io.out(0) = io.out(1) = f / 2;
    }
    sum /= 1024;
    if (sum > .707)  //
      printf("%f\n", sum);
  }
};

int main(int argc, char* argv[]) {
  //  AudioDevice out = AudioDevice::defaultOutput();
  //  out.print();
  //
  //  AudioDevice in = AudioDevice::defaultInput();
  //  in.print();
  //
  for (int i = 0; i < AudioDevice::numDevices(); i++) {
    printf(" --- [%2d] ", i);
    AudioDevice dev(i);
    dev.print();
  }

  // Create an "Aggregate Audio Device"
  //
  // - Start the "Audio MIDI Setup" app
  // - Click + in the bottom left corner and choose "Create Aggregate Device"
  // - Choose the in/out devices in the area on the right
  // - Rename the new aggregate device something meaningful
  // - Run this program to see *exactly* what that name is
  // - Use that whole name in the AudioDevice constructor like we see below
  //
  AudioDevice aggregate = AudioDevice("Apple Inc.: Zoom-In-Line-Out");
  aggregate.print();

  MyApp app;
  app.audioDomain()->configure(aggregate, 48000, 1024, 2, 2);
  app.audioDomain()->audioIO().print();
  app.start();
}
