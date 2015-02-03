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

#ifndef __MACVALIDATION_H
#define __MACVALIDATION_H

#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "Utils.h"

using std::unordered_set;
using std::string;

class MACValidation
{
public:
    MACValidation();
    ~MACValidation();

   bool     validate        (string mac);
   uint64_t convertToLong   (string mac);
};

#endif // __MACVALIDATION_H