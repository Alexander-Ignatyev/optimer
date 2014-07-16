// Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

#include "data_loader.h"

#include <cstdlib>

#include <iostream>
#include <sstream>
#include <string>
#include <functional>
#include <stdexcept>

#include <common/log.h>

#include <common/algo_string.h>
#include <common/geometry2d.h>

namespace tsp {

bool starts_with(const std::string &str, const std::string &pattern) {
    return str.compare(0, pattern.size(), pattern) == 0;
}

void write_coords_as_json(const std::vector<Geometry2D::Point> &coords
                         , std::ostream &os) {
    os << '[';
    if (!coords.empty()) {
        os << "{x: " << coords[0].x << ", y:" << coords[0].y << "}";
        for (size_t i = 1; i < coords.size(); ++i) {
            os << "," << std::endl;
            os << "{x: " << coords[i].x;
            os << ", y:" << coords[i].y << "}";
        }
    }
    os << "];" << std::endl;
}

class TspLibLoader {
    static const std::string MARKER_DIMENSION;
    static const std::string MARKER_FORMAT;
    static const std::string MARKER_DISP_DATA_TYPE;
    static const std::string MARKER_DATA;
    static const std::string MARKER_DATA_COORDS;
    static const std::string MARKER_EOF;

 public:
    TspLibLoader();
    void load(std::istream &is, std::vector<value_type> &matrix
              , size_t &dimension);

 private:
    TspLibLoader(const TspLibLoader &);
    TspLibLoader &operator=(const TspLibLoader &);
    // actions:
    void read_common_line(const std::string &line);
    void read_lower_diag_data_line(const std::string &line);
    void read_full_matrix_data_line(const std::string &line);
    void read_coords_data_line(const std::string &line);

    void read_dimension(const std::string &line);
    void read_format(const std::string &line);
    void read_display_data_type(const std::string &line);
    void read_edges(const std::string &line);

    typedef void (TspLibLoader::*action_ptr)(const std::string &);
    action_ptr next_action_;
    action_ptr data_action_;
    size_t dimension_;
    std::vector<value_type> matrix_;
    std::vector<Geometry2D::Point> coords_;
};

const std::string TspLibLoader::MARKER_DIMENSION = "DIMENSION: ";
const std::string TspLibLoader::MARKER_FORMAT = "EDGE_WEIGHT_FORMAT: ";
const std::string TspLibLoader::MARKER_DISP_DATA_TYPE = "DISPLAY_DATA_TYPE: ";
const std::string TspLibLoader::MARKER_DATA = "EDGE_WEIGHT_SECTION";
const std::string TspLibLoader::MARKER_DATA_COORDS = "NODE_COORD_SECTION";
const std::string TspLibLoader::MARKER_EOF = "EOF";

TspLibLoader::TspLibLoader(): dimension_(0) {
}

void TspLibLoader::load(std::istream &is, std::vector<value_type> &matrix
    , size_t &dimension) {
    matrix_.clear();
    dimension_ = 0;
    std::string line;
    next_action_ = &TspLibLoader::read_common_line;
    while (std::getline(is, line)) {
        (this->*next_action_)(line);
    }
    matrix.swap(matrix_);
    dimension = dimension_;
}

void TspLibLoader::read_common_line(const std::string &line) {
    if (starts_with(line, MARKER_DIMENSION)) {
        read_dimension(line);
    } else if (starts_with(line, MARKER_FORMAT)) {
        read_format(line);
    } else if (starts_with(line, MARKER_DISP_DATA_TYPE)) {
        read_display_data_type(line);
    } else if (starts_with(line, MARKER_DATA)) {
        next_action_ = data_action_;
    } else if (starts_with(line, MARKER_DATA_COORDS)) {
        next_action_ = data_action_;
    }
}

void TspLibLoader::read_lower_diag_data_line(const std::string &line) {
    if (!starts_with(line, MARKER_EOF)) {
        read_edges(line);
    } else {
        // construct matrix
        std::vector<value_type> data;
        data.swap(matrix_);
        matrix_.resize(dimension_*dimension_);
        size_t pos = 0;
        for (size_t i = 0; i < dimension_; ++i) {
            for (size_t j = 0; j < i+1; ++j) {
                if (i != j) {
                    matrix_[i*dimension_+j] = data[pos];
                    matrix_[j*dimension_+i] = data[pos];
                } else {
                    matrix_[i*dimension_+j] = M_VAL;
                }
                ++pos;
            }
        }
    }
}

void TspLibLoader::read_full_matrix_data_line(const std::string &line) {
    if (!starts_with(line, MARKER_EOF)) {
        read_edges(line);
    } else {
        // fix diagonal values
        for (size_t i = 0; i < dimension_; ++i) {
            matrix_[i*dimension_+i] = M_VAL;
        }
    }
}

void TspLibLoader::read_coords_data_line(const std::string &line) {
    if (!starts_with(line, MARKER_EOF)) {
        Geometry2D::Point point;
        unsigned coord_index;
        std::istringstream iss(line);
        iss >> coord_index >> point.x >> point.y;
        coords_.push_back(point);
    } else {
        if (coords_.size() != dimension_) {
            throw std::logic_error("Read invalid number of coords");
        }
        std::ostringstream oss;
        write_coords_as_json(coords_, oss);
        LOG(INFO) << "vertices = " << oss.str();
        matrix_.resize(dimension_*dimension_);
        for (size_t i = 0; i < dimension_; ++i) {
            for (size_t j = i; j < dimension_; ++j) {
                if (i != j) {
                    value_type distance = static_cast<value_type>(
                            Geometry2D::distance(coords_[i], coords_[j]));
                    matrix_[i*dimension_+j] = distance;
                    matrix_[j*dimension_+i] = distance;
                } else {
                    matrix_[i*dimension_+i] = M_VAL;
                }
            }
        }
    }
}

void TspLibLoader::read_dimension(const std::string &line) {
    dimension_ = atoi(line.substr(MARKER_DIMENSION.size()).c_str());
    matrix_.reserve(dimension_*dimension_);
}

void TspLibLoader::read_format(const std::string &line) {
    std::string format = line.substr(MARKER_FORMAT.size()).c_str();
    format = trim(format);
    if (format == "LOWER_DIAG_ROW") {
        data_action_ = &TspLibLoader::read_lower_diag_data_line;
    } else if (format == "FULL_MATRIX") {
        data_action_ = &TspLibLoader::read_full_matrix_data_line;
    } else {
        throw std::domain_error("Unknown format: " + format);
    }
}

void TspLibLoader::read_display_data_type(const std::string &line) {
    std::string data_type = line.substr(MARKER_DISP_DATA_TYPE.size()).c_str();
    data_type = trim(data_type);
    if (data_type == "COORD_DISPLAY") {
        data_action_ = &TspLibLoader::read_coords_data_line;
    } else {
        throw std::domain_error("Unknown display data type: " + data_type);
    }
}

void TspLibLoader::read_edges(const std::string &line) {
    std::istringstream iss(line);
    value_type val;
    iss >> val;
    while (iss) {
        matrix_.push_back(val);
        iss >> val;
    }
}

bool load_tsplib_problem(std::istream &is, std::vector<value_type> &matrix
                         , size_t &dimension, size_t max_dimension) {
    TspLibLoader loader;
    loader.load(is, matrix, dimension);

    // shrink to max size if needed
    if (max_dimension != 0 && max_dimension < dimension) {
        std::vector<value_type> new_matrix(max_dimension*max_dimension);
        for (size_t i = 0; i < max_dimension; ++i) {
            for (size_t j = 0; j < max_dimension; ++j) {
                new_matrix[i+j*max_dimension] = matrix[i+j*dimension];
            }
        }
        dimension = max_dimension;
        matrix.swap(new_matrix);
    }

    return true;
}

}  // namespace tsp

