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

#ifndef __IPv6VALIDATION_H
#define __IPv6VALIDATION_H

#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <bitset>
#include "Utils.h"

using std::unordered_set;
using std::bitset;
using std::string;

class IPv6Validation
{
public:
    IPv6Validation();
    ~IPv6Validation();

    bool        validate            (string ipv6);
    bitset<128> convertToBit128     (string address);
    int         getSubnet           (string subnet);

private:
    void        convertToFullIPv6   (string &ipv6);
};

#endif // __IPv6VALIDATION_H