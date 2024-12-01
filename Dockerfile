# Use Ubuntu 20.04 as the base image
FROM ubuntu:20.04

# Set the working directory to /app
WORKDIR /app



# Install dependencies
ENV DEBIAN_FRONTEND=noninteractive

# Update the package list and install dependencies
RUN apt update && apt install -y build-essential git libasio-dev checkinstall zlib1g-dev libssl-dev wget

RUN wget https://github.com/Kitware/CMake/releases/download/v3.27.1/cmake-3.27.1.tar.gz

RUN tar -zxvf cmake-3.27.1.tar.gz

RUN cd cmake-3.27.1 && ./bootstrap && make && make install

RUN rm cmake-3.27.1.tar.gz

RUN cd /app
# Clone the CrowCPP repository
RUN git clone https://github.com/CrowCpp/Crow.git

# Change into the Crow directory and build CrowCPP
WORKDIR /app/Crow
RUN git checkout v1.2.0
RUN cmake .
RUN make

# Install CrowCPP
RUN make install

RUN apt install gdb -y

RUN apt install rsyslog -y
# Clean up
RUN rm -rf /app/Crow

RUN apt-get install libcurl4-openssl-dev -y

CMD ["rsyslog", "-n"]