// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_SOLVER_PROVIDER_H_
#define SRC_SOLVER_PROVIDER_H_

template <typename S>
class ClonedSolverProvider {
 public:
    typedef S Solver;

    static Solver *get_solver() {
        return new Solver();
    }

    static void free_solver(Solver *solver) {
        delete solver;
    }
};

#endif  // SRC_SOLVER_PROVIDER_H_
