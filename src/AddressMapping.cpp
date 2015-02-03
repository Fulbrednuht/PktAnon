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


#include "AddressMapping.h"


static bool bigEndian = Utils::isBigEndian();
static std::uniform_int_distribution<unsigned int> distribution(0,255);
AddressMapping* AddressMapping::object = nullptr;

AddressMapping::AddressMapping ()
{
	std::random_device rd;
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count() + rd();

	generator.seed(seed);
}


AddressMapping::~AddressMapping (){}

AddressMapping* AddressMapping::getInstance ()
{
	static Guard guard;

	if (object == NULL)
		object = new AddressMapping ();

	return object;
}

void AddressMapping::generateIPsv4(uint32_t mapStartAddress,  int const index, int const size)
{
		auto rndIp = std::bind ( distribution, generator );

    	union {
                uint32_t address = 0;
                uint8_t part[4];
        } ip;
		ip.address = mapStartAddress;

		int startVal = bigEndian ? 0 			: 	index	;
		int endVal 	 = bigEndian ? index + 1	: 	4		;

    	while(randomIPsv4.size() < size){
    		for(int i = startVal; i < endVal; i++){
    			ip.part[i] = rndIp();
    		}
    		randomIPsv4.insert(ip.address);
    	}
}


void AddressMapping::generateMACs(uint64_t mapStartAddress, int const index, int const size)
{
	auto rndMac = std::bind ( distribution, generator );

	union {
		uint64_t address = 0;
		uint8_t part[8];
	} mac;
	mac.address = mapStartAddress;

	int startVal = bigEndian ? 		0		: 	index 	;
    int endVal 	 = bigEndian ? 	index + 1	: 		6	;

	while(randomMACs.size() < size){
		for(int i = startVal; i < endVal; i++){
			mac.part[i] = rndMac();
		}
		randomMACs.insert(mac.address);
	}	
}

void AddressMapping::initIPv4(uint32_t startIp, int replaceSubnet, uint32_t mapToIpRange, int mapToIpIndex)
{
	int numbOfIp = pow2(32 - replaceSubnet);
	generateIPsv4(mapToIpRange, mapToIpIndex, numbOfIp);
	ipv4Mapping(startIp, numbOfIp);

//	for(auto const kv : ipv4Map) {
//    	std::cout << std::hex << kv.first << " => " << kv.second << std::endl;
//    }
}
/*		7		  	6			5		  	4			3		  	2			1		  	0
	0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000 | 0000 0000*/

void AddressMapping::initIPv6(bitset<128> ipv6, int subnet)
{
	int nSub = 128 - subnet;
	bitset<128> upperBound = 0;
	if(bigEndian){
		upperBound.set(nSub, true);
	}
	else{
		upperBound.set(subnet, true);
	}
	upperBound = ipv6 ^ upperBound;

	ipv6LowerBound.address = ipv6;
	ipv6UpperBound.address = upperBound;

	if(bigEndian){
		for(int i = 127; i >= subnet; i--){
           	v6subnetmask.address.set(i, true);
        }
	}
	else{
		for(int i = 0; i <= subnet; i++){
        	v6subnetmask.address.set(i, true);
        }
	}

	int max0, max1;

	if(nSub > 64){
		max0 = bigEndian ? pow2(nSub - 64) 	: ULLONG_MAX	 ;
		max1 = bigEndian ? ULLONG_MAX 		: pow2(nSub - 64);
	}
	else if(nSub == 64){
		max0 = bigEndian ? 0 			: ULLONG_MAX;
        max1 = bigEndian ? ULLONG_MAX 	: 0			;
	}
	else{
		max0 = bigEndian ? 0 			: pow2(nSub);
		max1 = bigEndian ? pow2(nSub) 	: 0			;
	}
	std::uniform_int_distribution<uint64_t> distr0(1, max0);
	std::uniform_int_distribution<uint64_t> distr1(1, max1);
	rndXORipv6[0] = max0 != 0 ? distr0(generator) : 0;
	rndXORipv6[1] = max1 != 0 ? distr1(generator) : 0;
}


void AddressMapping::initMAC(std::unordered_set<uint64_t> givenMacs, uint64_t mapToMacRange, int const macIndex)
{
	generateMACs(mapToMacRange, macIndex, givenMacs.size());

    for(auto const mac : givenMacs) {
    	macMap[mac] = *randomMACs.begin();
    	randomMACs.erase(randomMACs.begin());
    }

    /*for(auto const kv : macMap) {
       std::cout << std::hex << kv.first << " => " << kv.second << std::endl;
    }*/
}


void AddressMapping::mapAddress(void* buf, unsigned int const len)
{
	if(len == 4){ // IPv4
		uint32_t ip = *(uint32_t *)buf;
		if(ipv4Map[ip])
			*(uint32_t *)buf = ipv4Map[ip];
	}
	else if(len == 6){ // MAC
		uint64_t mac = *(uint64_t *)buf;
		if(macMap[mac])
			*(uint64_t *)buf = macMap[mac];
	}
	else if(len == 16){ // IPv6 TODO testen ob das so läuft
		ipv6cur.address = *(bitset<128> *)buf;
		bool match = true;

		tmpTest.address = ipv6cur.address & v6subnetmask.address;

		if(bigEndian){
			if( (tmpTest.part[1] <  ipv6LowerBound.part[1] || tmpTest.part[0] < ipv6LowerBound.part[0]) // cur < lower
				|| (tmpTest.part[1] >  ipv6UpperBound.part[1] || tmpTest.part[0] > ipv6UpperBound.part[0]) ){ // cur > upper
				match = false;
			}

		}
		else{
			if( (tmpTest.part[0] <  ipv6LowerBound.part[0] || tmpTest.part[1] < ipv6LowerBound.part[1]) // cur < lower
            	|| (tmpTest.part[0] >  ipv6UpperBound.part[0] || tmpTest.part[1] > ipv6UpperBound.part[1]) ){ // cur > upper
   				match = false;
            }
		}
		if(match){
			ipv6cur.part[0] = ipv6cur.part[0] ^ rndXORipv6[0];
			ipv6cur.part[1] = ipv6cur.part[1] ^ rndXORipv6[1];
            *(bitset<128> *)buf = ipv6cur.address;
		}
	}
}


void AddressMapping::ipv4Mapping(uint32_t ipStartAddress, int const size)
{
	union {
    	uint32_t address = 0;
        uint8_t part[4];
    } ip;
 	ip.address = ipStartAddress;

	int IP1 = bigEndian ? 3 : 0;
	int IP2 = bigEndian ? 2 : 1;
	int IP3 = bigEndian ? 1 : 2;
	int IP4 = bigEndian ? 0 : 3;

	int x = ip.part[IP4];

	for(int i = 0; i < size; i++){
		ip.part[IP4] = x;
    	ipv4Map[ip.address] = *randomIPsv4.begin();
		randomIPsv4.erase(randomIPsv4.begin());
		x++;
		if(x > 255){
			if(ip.part[IP3] >= 255){
				if(ip.part[IP2] >= 255){
					assert(ip.part[IP1] < 255);
					ip.part[IP1]++;
				}
				ip.part[IP2]++;
			}
			ip.part[IP3]++;
			x = 0;
		}

	}
}


uint64_t AddressMapping::pow2(int x)
{
	uint64_t result = 1;
	result <<= x;

	return result;
}
