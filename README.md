# Optimer
## Discrete Optimization Library

### Build instruction

```
% mdkir build
% cd build
% export CXX=clang++ # or another compiler that you wish to use
% cmake ..
% make
% make test
% ./atsp <path-to-config-file>
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
