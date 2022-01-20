# cairoxp
This repository provides a minimal example of rendering 2D panel graphics (Cairo) on a background thread inside an X-Plane plugin. A bit of an explanation is provided below, as well as a guide to getting the plugin built and running inside the simulator. This is not meant for absolute beginners--there are many things here that are not explained and/or error prone.

## Screenshot inside the default C172

![Preview](https://github.com/aeroc7/cairoxp/blob/main/screenshots/panel.png)

## Screenshot of the performance monitor
Due to the rendering occuring on a background thread, separate from X-Plane, there is minimal impact on the simulator performance itself, leaving a smooth experience for the end user. The main drawback of this approach is the lack of flexibility in interacting with X-Plane. X-Plane SDK functions can **only** be called from the main X-Plane thread, AKA the thread that `XPluginStart` and `XPluginStop` get called on. Failing to adhere to this could lead to undefined behavior, and possibly even X-Plane crashing. This means that in order to get data from the simulator, you'd have to setup your own structure of variables protected with some sort of synchronization between your reads and writes, so that you're not calling X-Plane SDK functions from anywhere but the main thread.

![Performance](https://github.com/aeroc7/cairoxp/blob/main/screenshots/performance.png)

# Building
Firstly, you need to clone the repository: `git clone https://github.com/aeroc7/cairoxp.git`

## Windows (using MinGW on [MSYS2](https://www.msys2.org/))

## Linux (tested on Ubuntu 21.10)
You require the following dependencies:
`sudo apt-get install build-essential cmake meson libglu1-mesa-dev mesa-common-dev python3 python3-pip python3-setuptools`

- `cd` into the cloned folder (ex. `cd cairoxp`)
- Create a build directory `mkdir build && cd build`
- Generate a release build with CMake `cmake -DCMAKE_BUILD_TYPE=Release ..`
  - You may also want to set an install location with `-DCMAKE_INSTALL_PREFIX=your/install/directory` (The proper X-Plane plugin folder structure will be generated automatically as well)
- Start the build process with `make -j[num_cpus + 1]`
  - Replace the `[num_cpus + 1]` with the number of physical threads your processor has + 1 (`cat /proc/cpuinfo | grep "siblings"`)
- Install with `make install`

