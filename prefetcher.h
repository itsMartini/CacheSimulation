/*
 *
 * File: prefetcher.h
 * Author: Sat Garcia (sat@cs)
 * Description: Header file for prefetcher implementation
 *
 */

#ifndef PREFETCHER_H
#define PREFETCHER_H

#include <sys/types.h>
#include "mem-sim.h"
#include <queue>
#include <utility>
#include <vector>
#include <map>

struct ReqPriority
{
	u_int32_t priority;
};

typedef std::pair<Request, ReqPriority> TReqPair;

class ReqComp
{
	public:
	bool operator() (const TReqPair& lhs, const TReqPair& rhs) const
	{
		return (lhs.second.priority > rhs.second.priority);
	}
};

class Prefetcher
{
  private:
	// std::priority_queue<TReqPair, std::vector<TReqPair>, ReqComp> _reqQueue;
	std::vector<TReqPair> _reqQueue;
	std::map<u_int32_t,u_int32_t> _temporalMap;
	
	std::vector<Request> _arrivals;
	std::vector<int> _offsets;

	std::vector<TReqPair> GetSpatialRequests();
	std::vector<TReqPair> GetTemporalRequests();
	std::vector<ReqPriority> GetPriorities();

	void insert_request( TReqPair newReq );

  public:
	Prefetcher();

	// should return true if a request is ready for this cycle
	bool hasRequest(u_int32_t cycle);

	// request a desired address be brought in
	Request getRequest(u_int32_t cycle);

	// this function is called whenever the last prefetcher request was successfully sent to the L2
	void completeRequest(u_int32_t cycle);

	/*
	 * This function is called whenever the CPU references memory.
	 * Note that only the addr, pc, load, issuedAt, and HitL1 should be considered valid data
	 */
	void cpuRequest(Request req); 
};

#endif
