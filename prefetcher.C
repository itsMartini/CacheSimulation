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
		if (_offsets.back() != 0)
		{
			tempReq.addr=_arrivals.back().addr + _offsets.back();
			tempReq.fromCPU=0;
			tempPriority.priority=11;
			ret.push_back(make_pair(tempReq, tempPriority));
			// tempReq.addr+=16;_offsets.back();
                        // tempReq.fromCPU=0;
                        // tempPriority.priority=8;
			// ret.push_back(make_pair(tempReq, tempPriority));
		}
	}
	return ret;
}

vector<TReqPair> Prefetcher::GetTemporalRequests()
{
	ReqPriority tempPriority;
	Request tempReq;
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
	// cout << "mc: " << max_count << "\n";
	// cout << "addr: " << address << "\n";
	vector<TReqPair> ret;
	tempPriority.priority=2;
	tempReq.addr=address;
	tempReq.fromCPU=0;
	ret.push_back(make_pair(tempReq,tempPriority));
	return ret;
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
	if (!found)
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
		if(_offsets.size() > 2 )
		{
			_offsets.erase(_offsets.begin());
		}
		// _offsets.push_back((req.addr & 0xFFFF) - (_arrivals.back().addr & 0xFFFF)); 2.9656
                // _offsets.push_back((req.addr & 0xF0) - (_arrivals.back().addr & 0xF0)); 2.1951
                // _offsets.push_back((req.addr & 0x255) - (_arrivals.back().addr & 0x255));2.2567
                // _offsets.push_back((req.addr & 0x1FE) - (_arrivals.back().addr & 0x1FE)); 2.1721
                // _offsets.push_back((req.addr & 0x111) - (_arrivals.back().addr & 0x111));2.4001
                // _offsets.push_back((req.addr & 0x333) - (_arrivals.back().addr & 0x333));2.1702
                // _offsets.push_back((req.addr & 0x3CF) - (_arrivals.back().addr & 0x3CF));2.0757
                // _offsets.push_back((req.addr & 0x1C7) - (_arrivals.back().addr & 0x1C7)); 2.2322
                // _offsets.push_back((req.addr & 0x387) - (_arrivals.back().addr & 0x387)); 2.1599
		// _offsets.push_back((req.addr & 0x38F) - (_arrivals.back().addr & 0x38F)); 2.1390
		// _offsets.push_back((req.addr & 0x3EF) - (_arrivals.back().addr & 0x3EF)); // 2.0137
		_offsets.push_back((req.addr & 0x3EE) - (_arrivals.back().addr & 0x3EE)); // 2.0108
		// _offsets.push_back((req.addr & 0x2EE) - (_arrivals.back().addr & 0x2EE));2.1482
		// _offsets.push_back((req.addr & 0x1EE) - (_arrivals.back().addr & 0x1EE)); 2.1891
		// cout << (int)_offsets.back( ) << "\n";
	}
	if (_arrivals.size( ) > 60 )
	{
		_arrivals.erase(_arrivals.begin());
		_temporalMap.clear();
	}
	_arrivals.push_back(req);
	++_temporalMap.insert(make_pair(req.addr,0)).first->second;
	// printf("Hi\n");

	//if(!req.HitL1)
	{
		Request tempReq;
		ReqPriority tempPriority;
		// tempReq.addr = req.addr + 16; // 2.4321
		// tempReq.addr = req.addr + 32; // 2.1682
		// tempReq.addr = req.addr + 48; // 2.0805
		tempReq.addr = req.addr + 64; // 2.0418
		// tempReq.addr = req.addr + 80; // 2.0713
		// tempReq.addr = req.addr + 96; // 2.0315
		// tempReq.addr = req.addr + 112; // 2.0967
		// tempReq.addr = req.addr + 55; //2.0839 
		// tempReq.addr = req.addr + 87; // 2.0899 
		tempPriority.priority = 6;
		insert_request( make_pair(tempReq, tempPriority) );

		tempReq.addr += 16;
		tempPriority.priority = 7;
		insert_request( make_pair(tempReq, tempPriority) );

		tempReq.addr += 16;
        	tempPriority.priority = 8;
        	insert_request( make_pair(tempReq, tempPriority) ); 

	        /*tempReq.addr += 16;
        	tempPriority.priority = 9;
        	insert_request( make_pair(tempReq, tempPriority) ); 

		tempReq.addr += 16;
        	tempPriority.priority = 10;
        	insert_request( make_pair(tempReq, tempPriority) );*/  


	}
	
	vector<TReqPair> requests = GetSpatialRequests();
	for( size_t i=0; i<requests.size( ); ++i )
	{
		// cout << "HI\n";
		insert_request( requests[i] );
	}
	
	/* if( _arrivals.size()%20 == 0 )
	{
		requests = GetTemporalRequests();
    		for( size_t i=0; i<requests.size( ); ++i )
    		{
        		insert_request( requests[i] );
    		}
	} */
}
