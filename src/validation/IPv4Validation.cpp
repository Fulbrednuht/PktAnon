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

#include "IPv4Validation.h"

#define N 4

static bool bigEndian = Utils::isBigEndian();

IPv4Validation::IPv4Validation(){}
                    
IPv4Validation::~IPv4Validation(){}

bool IPv4Validation::validate(string ip)
{
    if(std::count(ip.begin(), ip.end(), '.') != N - 1)
        return false;

    string ipPart;
    try{
        std::istringstream iss(ip);
        for(int i = 0; i < N; i++){
            std::getline( iss, ipPart , '.' );
            int x = std::stoi(ipPart, nullptr, 10);
            if(x > 255)
                return false;
        }
    }
    catch(...) {
        return false;
    }

    return true;
}

int IPv4Validation::getSubnet(string subnetmask)
{
    int subnet;
    try{
        subnet = std::stoi(subnetmask);
        if(subnet > 32)
            subnet = - 1;
    }
    catch(...) {
        subnet = -1;
    }
    return subnet;
}

uint32_t IPv4Validation::convertToInt(string ipAddress)
{
    union {
        uint32_t address = 0;
        uint8_t part[N];
    } ip, ret;

    std::istringstream iss(ipAddress);
    string ipPart;

    for(int i = 0; i < N; i++){
        std::getline( iss, ipPart , '.' );
        ip.part[i] = std::stoi(ipPart, nullptr, 10);
    }

    ret.address = ip.address;
    if(bigEndian){
        ret.part[0]     = ip.part[3];
        ret.part[1]     = ip.part[2];
        ret.part[2]     = ip.part[1];
        ret.part[3]     = ip.part[0];
    }

    return ret.address;
}