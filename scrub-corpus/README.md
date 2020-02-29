# Scrub Corpus

Analyse a set of .wav files using [Gist][], then search this corpus, concatenating nearest-neighbor frames.

First, grab and build [Gist][]:

    git clone https://github.com/kybr/Gist
    cd Gist
    make

Next, use [allolib_playground][] to build this example:

    ./run.sh -n path/to/scrub-cortpus/main.cpp

The `-n` means "just build; don't run". This creates an executable file `bin/main` which is a command line tool that expects .wav files as arguments. like this:

  ./bin/main foo.wav bar.wav quux.wav

Have a look at `flags.cmake` in this folder. This is one way we can build against non-AlloLib c++ libraries.

[Gist]: https://github.com/adamstark/Gist
[allolib_playground]: https://github.com/AlloSphere-Research-Group/allolib_playground
