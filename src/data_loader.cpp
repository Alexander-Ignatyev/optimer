// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#include "data_loader.h"

#include <cstdlib>

#include <iostream>
#include <string>
#include <sstream>

static const std::string MARKER_DIMENSION = "DIMENSION: ";
static const std::string MARKER_EDGE_WEIGHT_SECTION = "EDGE_WEIGHT_SECTION";
static const std::string MARKER_EOF = "EOF";

bool starts_with(const std::string &str, const std::string &pattern) {
    return str.compare(0, pattern.size(), pattern) == 0;
}

size_t read_dimension(std::string &line) {
    return atoi(line.substr(MARKER_DIMENSION.size()).c_str());
}

void read_edges(std::string &line, value_type *matrix, size_t &position) {
    std::istringstream iss(line);
    value_type val;
    iss >> val;
    do {
        matrix[position] = val;
        ++position;
        iss >> val;
    } while (iss);
}

bool load_tsplib_problem(std::istream &is, value_type *&matrix
    , size_t &dimension) {
    matrix = nullptr;

    std::string line;
    bool edges_section = false;
    size_t position = 0;
    while (std::getline(is, line)) {
        if (starts_with(line, MARKER_DIMENSION)) {
            dimension = read_dimension(line);
            if (matrix != nullptr) {
                return false;
            }
            matrix = new value_type[dimension*dimension];
        } else if (starts_with(line, MARKER_EDGE_WEIGHT_SECTION)) {
            edges_section = true;
        } else if (starts_with(line, MARKER_EOF)) {
            edges_section = false;
        } else if (edges_section) {
            if (matrix == nullptr) {
                return false;
            }
            read_edges(line, matrix, position);
        } else {
        }
    }
    return true;
}
