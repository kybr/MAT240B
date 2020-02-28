// Karl Yerkes
// Example of using emplace_back to avoid copying data or calling new
//
#include <vector>
using namespace std;

struct SamplePlayer : vector<float> {
  char name[100] = {0};
  void load(const char* fileName) {  //
    sprintf(name, "%s", fileName);
  }
};

struct CorpusFile {
  float x = 0, y = 0, z = 0;
  SamplePlayer player;
};

int main() {
  vector<CorpusFile> corpus;
  while (true) {
    corpus.emplace_back();
    corpus.back().player.load("some/file/path.wav");
    break;
  }
  printf("%s\n", corpus[0].player.name);
}
