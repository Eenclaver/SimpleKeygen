FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    g++ \           
    cmake \         
    libpq-dev \     
    libhiredis-dev \ 
    libssl-dev \    
    libboost-all-dev \ 
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app  
COPY . .      

RUN mkdir build && cd build && \
    cmake .. && make  

EXPOSE 8080 
CMD ["./build/license_server"]  