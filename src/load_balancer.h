/*
 * load_balancer.h
 *
 *  Created on: 06.11.2009
 *      Author: sash
 */

#ifndef LOAD_BALANCER_H_
#define LOAD_BALANCER_H_

struct LoadBalancerParams
{
	unsigned threads;
	unsigned minimum_nodes;
	unsigned maximum_nodes;
};

#endif /* LOAD_BALANCER_H_ */
