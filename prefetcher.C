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

vector<u_int32_t> Prefetcher::GetAddresses()
{
	vector<u_int32_t> ret;
	if (_offsets.size() > 1)
	{
		if (_offsets.back() == _offsets.at(_offsets.size()-2))
		{
			ret.push_back(_arrivals.back().addr + _offsets.back());
		}
	}
	return ret;
}

void Prefetcher::cpuRequest(Request req)
{
	if (!_arrivals.empty())
	{
		_offsets.push_back(req.addr - _arrivals.back().addr);
	}
	_arrivals.push_back(req);

	if(!req.HitL1)
	{
		Request tempReq;
		ReqPriority tempPriority;
		tempPriority.priority = 1;

		vector<u_int32_t> addresses = GetAddresses();
		for (size_t i = 0; i < addresses.size(); ++i)
		{
			tempReq.addr = addresses.at(i);
			_reqQueue.push(make_pair(tempReq, tempPriority));
		}

		/*tempReq.addr = req.addr + 16;
		tempPriority.priority = 1;
		_reqQueue.push(make_pair(tempReq, tempPriority));

		tempReq.addr += 16;
		_reqQueue.push(make_pair(tempReq, tempPriority));

		tempReq.addr += 16;
		_reqQueue.push(make_pair(tempReq, tempPriority));

		tempReq.addr += 16;
		_reqQueue.push(make_pair(tempReq, tempPriority));*/
		
	}
}
