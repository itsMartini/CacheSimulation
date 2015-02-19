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
	char priority;
	u_int32_t policy;
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
	std::vector<TReqPair> _reqQueue;		// (4+1.25)*500=2625
	std::map<u_int32_t,u_int32_t> _temporalMap;	// 4*50=200
	std::vector<TReqPair> _recentRequests;		// 5.25
	
	std::vector<Request> _arrivals;			// 4*20=80
	std::vector<u_int32_t> _periodicRequests;	// 4*10=40
	std::vector<short> _offsets;			// 2*2=4

	std::vector<char> _policyBasePriorities;	// 4

	std::vector<TReqPair> GetOffsetRequests();
	std::vector<TReqPair> GetTemporalRequests();
	std::vector<TReqPair> GetLookAheadRequests(Request req);
	std::vector<TReqPair> GetPeriodicRequests();
	std::vector<ReqPriority> GetPriorities();

	void insertRequest( TReqPair newReq );
	void updatePriorities( u_int32_t policy );

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
