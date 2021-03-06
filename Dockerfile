FROM docker.io/ubuntu:16.04
MAINTAINER Jacek Danecki <jacek.danecki@intel.com>

COPY neo /root/neo
RUN apt-get -y update; apt-get install -y --allow-unauthenticated cmake g++ git wget pkg-config ninja-build
RUN cd /root; git clone https://github.com/intel/gmmlib gmmlib ;\
    git clone --depth 1 -b release-1.7.0 https://github.com/google/googlemock gmock ; \
    git clone --depth 1 -b release-1.7.0 https://github.com/google/googletest gtest ; \
    git clone --depth 1 https://github.com/KhronosGroup/OpenCL-Headers khronos ; \
    cd neo/scripts/igc ; ./prepare.sh ; cd ../../.. ; \
    mkdir build; cd build ; cmake -G Ninja -DBUILD_TYPE=Release -DCMAKE_BUILD_TYPE=Release ../neo ; ninja -j `nproc`
CMD ["/bin/bash"]
