/*
 *
 * File: prefetcher.C
 * Author: Sat Garcia (sat@cs)
 * Description: This simple prefetcher waits until there is a D-cache miss then 
 * requests location (addr + 16), where addr is the address that just missed 
 * in the cache.
 *
 */

#include "prefetcher.h"
#include <stdio.h>

using namespace std;

Prefetcher::Prefetcher()
{
	
}

bool Prefetcher::hasRequest(u_int32_t cycle)
{
	return !_reqQueue.empty();
}

Request Prefetcher::getRequest(u_int32_t cycle)
{
	return _reqQueue.top().first;
}

void Prefetcher::completeRequest(u_int32_t cycle)
{
	printf("%ld\n", _reqQueue.size());
	_reqQueue.pop();
}

void Prefetcher::cpuRequest(Request req)
{
	_arrivals.push_back(req);

	if(!req.HitL1)
	{
		Request tempReq;
		ReqPriority tempPriority;

		tempReq.addr = req.addr + 16;
		tempPriority.priority = 1;
		_reqQueue.push(make_pair(tempReq, tempPriority));

		tempReq.addr += 16;
		_reqQueue.push(make_pair(tempReq, tempPriority));

		tempReq.addr += 16;
		_reqQueue.push(make_pair(tempReq, tempPriority));

		tempReq.addr += 16;
		_reqQueue.push(make_pair(tempReq, tempPriority));
		
	}
}
