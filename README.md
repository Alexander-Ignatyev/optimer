# Optimer
## Discrete Optimization Library
## Performace testing


The results of the reference implementation are presented in the third column "time (s.)". Results of other implementations are presented in columns with names "(n)", where n is a reference number of the implementation. Please see below the table to get the full list of the implementations.


| Problem | # of cities | time (s.) | # of cores |  (1) | (2) | (3) | (3) - Concorde | (4) | (5) |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| br17 | 17 | 2.891 | 8 | 3 | 4.18 | -- | -- | 6.937 | 1256.524 |
| ft53 | 53 | 57.136 | 8 | -- | 2.53 | 0.44 | 0.27 | -- | -- |
| ft70 | 70 | 0.078 | 2 | -- | 1.87 | 1.51 | -- | -- | 147.265 |
| ftv33 | 34 | 0.004 | 4 | -- | 0.11 | 0.01 | -- | 3.94 | 0.156 |
| ftv35 | 36 | 0.007 | 8 | -- | 0.22 | 0.46 | -- | 97.425 | 0.031 |
| ftv38 | 39 | 0.012 | 8 | -- | 0.22 | 0.51 | -- | 382.958 | 0.031 |
| ftv44 | 45 | 0.008 | 4 | -- | 0.05 | 0.06 | -- | -- | 0.078 |
| ftv47 | 48 | 0.041 | 8 | -- | 1.37 | 2.07 | -- | -- | 0.406 |
| ftv55 | 56 | 0.347 | 8 | 6 | 4.56 | 10.59 | -- | -- | 2.437 |
| ftv64 | 65 | 0.191 | 8 | -- | 3.24 | 1.56 | 30.21 | -- | 1.984 |
| ftv70 | 71 | 0.301 | 8 | 9 | 8.57 | 4.89 | 8.43 | -- | 8.203 |
| ftv90 | 91 | 0.077 | 8 | -- | 1.54 | -- | -- | -- | 1.484 |
| ftv100 | 101 | 0.975 | 8 | -- | 21.87 | -- | -- | -- | -- |
| ftv110 | 111 | 1.291 | 8 | -- | 89.56 | -- | -- | -- | -- |
| ftv120 | 121 | 8.453 | 8 | -- | 305.71 | -- | -- | -- | -- |
| ftv130 | 131 | 0.421 | 8 | -- | 19.01 | -- | -- | -- | 114.265 |
| ftv140 | 141 | 0.756 | 8 | -- | 13.3 | -- | -- | -- | 108.375 |
| ftv150 | 151 | 0.402 | 8 | -- | 4.67 | -- | -- | -- | 132.078 |
| ftv160 | 161 | 8.328 | 8 | -- | 496.92 | -- | -- | -- | 3771.093 |
| ftv170 | 171 | 39.827 | 8 | 480 | 656.59 | -- | -- | -- | -- |
| kro124p | 100 | 219.555 | 8 | -- | Not | 1000+ | 5.61 | -- | 3505.406 |
| rbg323 | 323 | 0.306 | 2 | -- | 0 | -- | -- | 0.034 | 2.968 |
| rbg358 | 358 | 0.393 | 2 | -- | 0 | 0.03 | -- | 0.042 | 7.328 |
| rbg403 | 403 | 0.602 | 2 | -- | 0.05 | -- | 847.1 | 0.079 | 5.484 |
| rbg443 | 443 | 0.734 | 2 | -- | 0.05 | 0.05 | 81.62 | 0.079 | 3.578 |
| ry48p | 48 | 15.908 | 8 | -- | 105.88 | 77.35 | 11.39 | -- | 54.406 |
| p43 | 43 | 2280* | 8 | 4800 | Not | -- | -- | -- | -- |



1. AV Tyulenev, Application of integer linear programming with sequential elimination of cycles to solve the traveling salesman problem. 2012
2. Marcel Turkensteen, Diptesh Ghosh, Boris Goldengorin, Gerard Sierksma. Tolerance-based Branch and Bound algorithms for the ATSP. 2008.							
3. Remco Germs, Boris Goldengorin, Marcel Turkenstee. Lower tolerance-based Branch and Bound algorithms for the ATSP. 2012							
4. Pessoa, Tiago Carneiro. Estratégias paralelas inteligentes para o método branch-and-
bound aplicadas ao problema do caixeiro viajante assimétrico. 2012.							
5. IF Borhanov, VR Fazylov, "Little's method with optimal matrix reduction", Physics and mathematics, Cat. App. Kazan. State. University. Ser. Phys.-Math. Science, 148, No. 4, Kazan Univ., Kazan, 2006, 13-22


### Build instruction

```
% mdkir build
% cd build
% export CXX=clang++ # or another compiler that you wish to use
% cmake ..
% make
% make test   # run unit tests
% cd ../  # back to the project's root dir
% ./build/atsp config/default.ini
% cmake -DCMAKE_BUILD_TYPE=Release .. # to build release version
```

### Build instruction for Visual Studio 2012 (by Alexey Voytenko)

```
% mkdir build
% cd build
% cmake -DUSE_UNIT_TESTS=OFF ..
```

*unit tests are not supported for VS now*

### Known issues 

* MacPorts clang-3.3: http://trac.macports.org/ticket/38527
to fix it just add a line to `~/.profile`:
  
  `export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/opt/local/libexec/llvm-3.3/lib/clang/3.3/lib/darwin`
