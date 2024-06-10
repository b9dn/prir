# Parallel and Distributed Programming Tasks

This repository contains a collection of tasks completed during the "Parallel and Distributed Programming" course. The tasks involve various parallel and distributed programming techniques using POSIX, OpenMP, MPI, and parallel mechanisms in Java and Python. Additionally, there is a final project focused on image tracking using parallel processing in C++.

## Technologies and Tools

- **POSIX (pthreads)**
- **OpenMP**
- **MPI**
- **Java (threads, ExecutorService)**
- **Python (multiprocessing)**
- **C++ (threads, OpenCV)**

## Final Project: Parallel Image Tracking

- **Language:** C++
- **Library:** OpenCV
- **Description:** The project uses OpenCV to capture video frames. One thread captures the frames, a group of threads processes them in parallel, and another thread displays the results.

## How to Run

### Prerequisites

0. Install the necessary libraries, tools (OpenCV C++, Cmake)

1. Clone the repository:
    ```bash
    git clone https://github.com/b9dn/prir
    cd prir
    ```

2. Create a build directory:
    ```bash
    mkdir build
    cd build
    ```

3. Generate the build files using CMake:
    ```bash
    cmake ..
    ```

4. Compile the project using `make`:
    ```bash
    make
    ```

5. Run the executable:
    ```bash
    ./objectTracking [path_to_video]
    ```

