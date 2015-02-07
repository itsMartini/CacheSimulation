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
#include <algorithm>
#include <iostream>

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
	return _reqQueue.back().first;
}

void Prefetcher::completeRequest(u_int32_t cycle)
{
	// printf("%ld\n", _reqQueue.size());
	_reqQueue.pop_back();
}

vector<TReqPair> Prefetcher::GetSpatialRequests()
{
	vector<TReqPair> ret;
	Request tempReq;
	ReqPriority tempPriority;
	if (_offsets.size() > 1)
	{
		if (_offsets.back() == _offsets.at(_offsets.size()-2) &&
			_offsets.back() > 0 ) // && (_arrivals.back().fromCPU ))
		{
			tempReq.addr=_arrivals.back().addr - _offsets.back();
			tempPriority.priority=3;
			ret.push_back(make_pair(tempReq, tempPriority));
		}
	}
	return ret;
}

vector<TReqPair> Prefetcher::GetTemporalRequests()
{
	ReqPriority tempPriority;
    Request req;
	u_int32_t max_count=0;
	u_int32_t address;
	for (map<u_int32_t,u_int32_t>::iterator it=_temporalMap.begin(); 
			it!=_temporalMap.end(); ++it)
	{
		if( it->second > max_count )
		{
			max_count=it->second;
			address=it->first;
		}
	}
	tempPriority.priority=1;
	req.addr=address;
	vector<TReqPair> tPairs;
	tPairs.push_back(make_pair(req,tempPriority));
	return tPairs;
}
void Prefetcher::insert_request( TReqPair newReq )
{
	if( _reqQueue.empty( ) )
	{
		_reqQueue.push_back(newReq);
		return;
	}
	bool found=0;
	for( size_t j=0; j<_reqQueue.size( ); ++j )
	{
		if( _reqQueue[j].first.addr==newReq.first.addr )
		{
			if( _reqQueue[j].second.priority!=1 )
			{
				_reqQueue[j].second.priority-=1;
				sort(_reqQueue.begin(), _reqQueue.end(), ReqComp());
			}
			found=1;
			break;
		}
	}
	if( !found )
	{
		for (vector<TReqPair>::iterator it = _reqQueue.end()-1;;--it)
		{
			if (newReq.second.priority < it->second.priority)
			{
				++it;
				_reqQueue.insert(it, newReq);
				break;
			}
			if (_reqQueue.begin() == it)
			{
				_reqQueue.insert(it, newReq);
				break;
			}
		}
	}
}

void Prefetcher::cpuRequest(Request req)
{
	if (!_arrivals.empty())
	{
		_offsets.push_back(req.addr - _arrivals.back().addr);
		// cout << _offsets.back( ) << "\n";
	}
	_arrivals.push_back(req);
	++_temporalMap.insert(make_pair(req.addr,0)).first->second;
	// printf("Hi\n");

	/* if(!req.HitL1)
	{
		Request tempReq;
		ReqPriority tempPriority;
		tempReq.addr = req.addr + 16;
		tempPriority.priority = 1;
		insert_request( make_pair(tempReq, tempPriority) );

		tempReq.addr += 16;
		tempPriority.priority = 1;
		insert_request( make_pair(tempReq, tempPriority) );

		tempReq.addr += 16;
        tempPriority.priority = 1;
        insert_request( make_pair(tempReq, tempPriority) );

        tempReq.addr += 16;
        tempPriority.priority = 1;
        insert_request( make_pair(tempReq, tempPriority) );

	} */
	vector<TReqPair> requests = GetSpatialRequests();
	for( size_t i=0; i<requests.size( ); ++i )
	{
		insert_request( requests[i] );
	}
	/* vector<TReqPair> requests = GetTemporalRequests();
    for( size_t i=0; i<requests.size( ); ++i )
    {
        insert_request( requests[i] );
    } */
}
