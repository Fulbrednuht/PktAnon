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

#include "IPv6Validation.h"

#define N 16

static bool bigEndian = Utils::isBigEndian();

IPv6Validation::IPv6Validation(){}

IPv6Validation::~IPv6Validation(){}

bool IPv6Validation::validate(string ip)
{
    unsigned int found = ip.find("::");
    if(found != std::string::npos)
        convertToFullIPv6(ip);

    if(std::count(ip.begin(), ip.end(), ':') != N - 1)
        return false;

    string ipPart;
    try{
        std::istringstream iss(ip);
        for(int i = 0; i < N; i++){
            std::getline( iss, ipPart , ':' );
            int x = std::stoi(ipPart, nullptr, 16);
            if(x > 65536)
                return false;
        }
    }
    catch(...) {
        return false;
    }

    return true;
}

void IPv6Validation::convertToFullIPv6(string &ipv6)
{
    unsigned int pos = ipv6.find("::");
    unsigned int count = std::count(ipv6.begin(), ipv6.end(), ':');

    pos++;
    ipv6.insert(pos, "0");
    count++; pos++;
    while (count < N){
        ipv6.insert(pos, ":0");
        pos += 2;
        count++;
    }
    if(ipv6.at(ipv6.length() - 1) == ':' ){
    		ipv6 = ipv6.append(0);
    }
}

int IPv6Validation::getSubnet(string subnetmask)
{
    int subnet;
    try{
        subnet = std::stoi(subnetmask);
        if(subnet > 127)
            subnet = - 1;
    }
    catch(...) {
        subnet = -1;
    }
    return subnet;
}

bitset<128> IPv6Validation::convertToBit128(string address)
{
    union {
        bitset<128> address = 0;
        uint8_t part[N];
    } ipv6, ret;

    union {
        uint16_t chunk  = 0;
        uint8_t part[2];
    } ipv6block;

    unsigned int found = address.find("::");
    if(found != std::string::npos)
        convertToFullIPv6(address);

    std::istringstream iss(address);
    string ipPart;

    for(int i = 0; i < N; i++){
        std::getline( iss, ipPart , ':' );
        ipv6block.chunk = std::stoi(ipPart, nullptr, 16);
        ipv6.part[i++]   = ipv6block.part[1];
        ipv6.part[i]     = ipv6block.part[0];
    }

    ret.address = ipv6.address;
    if(bigEndian){
        ret.part[0]     = ipv6.part[15];
        ret.part[1]     = ipv6.part[14];
        ret.part[2]     = ipv6.part[13];
        ret.part[3]     = ipv6.part[12];
        ret.part[4]     = ipv6.part[11];
        ret.part[5]     = ipv6.part[10];
        ret.part[6]     = ipv6.part[9];
        ret.part[7]     = ipv6.part[8];
        ret.part[8]     = ipv6.part[7];
        ret.part[9]     = ipv6.part[6];
        ret.part[10]    = ipv6.part[5];
        ret.part[11]    = ipv6.part[4];
        ret.part[12]    = ipv6.part[3];
        ret.part[13]    = ipv6.part[2];
        ret.part[14]    = ipv6.part[1];
        ret.part[15]    = ipv6.part[0];
    }

    return ret.address;
}