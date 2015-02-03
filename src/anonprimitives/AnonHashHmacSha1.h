//
// Copyright (C) 2008-2011 Institute of Telematics, Karlsruhe Institute of Technology (KIT)
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

#ifndef __ANON_HASH_HMAC_SHA1_H
#define __ANON_HASH_HMAC_SHA1_H

#include <cstring>
#include "AnonPrimitive.h"
#include "anonprimitives/hmacsha1/sha1.h"
#include "anonprimitives/hmacsha1/sha1_hmac.h"

class AnonHashHmacSha1 : public AnonPrimitive {

public:
								AnonHashHmacSha1		(string hmackey);
								~AnonHashHmacSha1		();

protected:

	ANON_RESULT					anonymize				(void* buf, unsigned int len);

private:

	uint8_t*					key;
	unsigned short				keylen;
	uint8_t						digest [SHA1_DIGLEN];
};


#endif // __ANON_HASH_HMAC_SHA1_H

