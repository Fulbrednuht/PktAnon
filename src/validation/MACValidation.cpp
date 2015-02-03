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

#include "MACValidation.h"

#define N 6

static bool bigEndian = Utils::isBigEndian();


MACValidation::MACValidation(){}

MACValidation::~MACValidation(){}


bool MACValidation::validate(std::string mac)
{
    if(std::count(mac.begin(), mac.end(), ':') != N - 1)
            return false;

    std::string macPart;
    try{
        std::istringstream iss(mac);
        for(int i = 0; i < N; i++){
            std::getline( iss, macPart , ':' );
            int x = std::stoi(macPart, nullptr, 16);
            if(x > 255)
                return false;
            }
    }
    catch(...) {
        return false;
    }
    return true;
}

uint64_t MACValidation::convertToLong(std::string macAddress)
{
    union {
        uint64_t address = 0;
        uint8_t part[8];
    } mac, ret;

    std::istringstream iss(macAddress);
    std::string macPart;

    for(int i = 0; i < N; i++){
        std::getline( iss, macPart , ':' );
        mac.part[i] = std::stoi(macPart, nullptr, 16);
    }

    ret.address = mac.address;
     if(bigEndian){
            ret.part[0]     = mac.part[5];
            ret.part[1]     = mac.part[4];
            ret.part[2]     = mac.part[3];
            ret.part[3]     = mac.part[2];
            ret.part[4]     = mac.part[1];
            ret.part[5]     = mac.part[0];
     }

    return ret.address;
}
