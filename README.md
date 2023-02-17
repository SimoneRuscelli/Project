# Computer Science for High Energy Physics Project
The aim of this program is to compute and graphically plot the Mandelbrot set.

In order to do so, the code is parallelized so that the computations required to get each single complex point of the Mandelbrot set are partitioned amongst different threads.
In particular, the core of the program can be identified in the `tbb::parallel_for` function, which in turn gets advantage of the `tbb::blocked_range2d` structure.

Another very important aspect of the program is to test the processing efficiency: the goal here is to find the minimum computation time when computing the Mandelbrot set, as function of the grain size. 

The entire code can be found in the *main.cpp* file, whereas the building options are contained in the *CMakeLists.txt* file. 

This code has been uploaded on this VM and can be executed by simply running the docker image `image_run`.

The two images `image_run` and `image_compilation` are built via two Dockerfiles that include the instructions to install the packages needed for the compilation and the execution of the code, along with the creation of a directory required as volume which will be then linked to a local directory.
The Dockerfiles for the images building can be found in the corresponding directories `Dockerfile_compilation` and `Dockerfile_run`.

An example of the Dockerfile used to build the images might be the following:

    FROM ubuntu:22.04

    RUN apt update && DEBIAN_FRONTEND=noninteractive apt install -y \
        g++ \
        cmake \
        libsfml-dev \
        libtbb-dev \
        git-all 

    RUN mkdir -p /workspace
    VOLUME /workspace

    WORKDIR /workspace


The libraries **g++** and **cmake** are used to compile and build the project.
The libraries libsfml-dev and libtbb-dev are required in order to access the graphical tools of __SFML__ and the parallelization functions of **TBB**.
The git toolkit is installed to clone the directory https://github.com/SimoneRuscelli/Project.git which contains the project.

In the creation process of the image used for running the program, the installation of the following libraries: **g++**, **cmake** and **git** can be avoided, as they are not used. 

## To run the code
The running process of the program is then straightforward: after building the code, one can run it via:

    cmake -S . -B build_release -DCMAKE_BUILD_TYPE=Release
    cmake --build build_release
    build_release/mandelbrot

Please note that the output is saved in the current directory and embeds a _.png_ file and a _.txt_ file:
- Mandelbrot.png ⟶ Representation of the Mandelbrot set in shades of red (associated to a grain size of 800 pixels);
- Time_vs_grain_size.txt ⟶ Elapsed times corresponding to the tested grain sizes.