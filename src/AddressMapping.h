//
// Copyright (C) 2014 Hochschule Hannover  (Fakultaet 4 Abt. Informatik)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#ifndef __ADDRESSMAPPING_H
#define __ADDRESSMAPPING_H

#include <unordered_map>
#include <unordered_set>
#include <random>
#include <functional>
#include <bitset>
#include <cassert>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>
#include "Utils.h"

using std::unordered_set;
using std::unordered_map;
using std::bitset;


class AddressMapping
{
public:

	static AddressMapping* getInstance();

	typedef unordered_map<uint32_t, uint32_t>		 		map;
	typedef unordered_map<uint64_t, uint64_t> 				mapLong;
	typedef unordered_set<uint32_t> 						set;
	typedef	unordered_set<uint64_t> 						setLong;


	void 		initIPv4	(uint32_t startIp, int replaceSubnet, uint32_t mapToIpRange, int mapToIpIndex);
	void 		initIPv6	(bitset<128> startIp, int subnet);
	void 		initMAC		(unordered_set<uint64_t> givenMacs, uint64_t mapToMacRange, int const macIndex);

	void 		mapAddress	(void* buf, unsigned int const len);

private:

	AddressMapping	();
    ~AddressMapping	();

	// Singleton
    class Guard {
    	public: ~Guard() {
    		if (AddressMapping::object != NULL){
    			delete AddressMapping::object;
    			AddressMapping::object = NULL;
    		}
    	}
    };
    friend class			Guard;
	static 	AddressMapping*	object;

	// IPv4 & MAC
    map 		ipv4Map;
    mapLong 	macMap;
    set 		randomIPsv4;
    setLong 	randomMACs;

    // IPv6
    union{
    	bitset<128> address = 0;
    	uint64_t  part[2];
    } ipv6cur, v6subnetmask, tmpTest, ipv6LowerBound, ipv6UpperBound;
	int v6subnet;
	uint64_t rndXORipv6[2];

	// Random Number Generator
	std::knuth_b generator;

	// Helper Functions
    void 		generateIPsv4	(uint32_t mapStartAddress,  int const index, int const size);
	void 		generateMACs	(uint64_t mapStartAddress, int const index, int const size);
	void 		ipv4Mapping		(uint32_t ipStartAddress, int const size);
	uint64_t	pow2			(int x);
};

#endif // __ADDRESSMAPPING_H

