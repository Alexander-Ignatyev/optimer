// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

/*
 * load_balancer.h
 *
 *  Created on: 06.11.2009
 *      Author: sash
 */

#ifndef SRC_LOAD_BALANCER_H_
#define SRC_LOAD_BALANCER_H_

struct LoadBalancerParams {
    unsigned threads;
    unsigned minimum_nodes;
    unsigned maximum_nodes;
};

#endif  // SRC_LOAD_BALANCER_H_
