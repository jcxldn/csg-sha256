# IN2029 SHA-256 Exercise


## Usage

```
mpirun -n <processors> build/runner <num_zeros> <starting_value> <step_value>
```

### Building
```
flight env activate gridware
module add compilers/gcc/11.2.0
module add mpi/openmpi/4.1.1/gcc-11.2.0
module add apps/cmake/3.21.1/gcc-11.2.0

git clone https://github.com/jcxldn/csg-sha256
cmake -B build csg-sha256
cmake --build build --target runner
cmake --build build --target benchmark_runner_all
```

### Execution Setup

```
flight env activate gridware
module add libs/gcc/11.2.0
```
