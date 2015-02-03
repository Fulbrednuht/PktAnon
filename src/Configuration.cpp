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

// the packet includes must be here because
// we have a cyclic inclusion with the Packet class
#include "packets/EthernetPacket.h"
#include "packets/VlanPacket.h"
#include "packets/IpPacket.h"
#include "packets/UdpPacket.h"
#include "packets/TcpPacket.h"
#include "packets/IcmpPacket.h"
#include "packets/PayloadPacket.h"
#include "packets/ArpPacket.h"
#include "packets/Ipv6Packet.h"
#include "Configuration.h"

const string 	Configuration::PATH_RUN_FLOW_SOURCE				= "Settings/Flow/Input";
const string 	Configuration::PATH_RUN_FLOW_DEST				= "Settings/Flow/Output";
const string 	Configuration::PATH_RUN_MEASURE_ON				= "Settings/Misc/UseMeasure";
const string	Configuration::PATH_RUN_MEASURE_FILE			= "Settings/Misc/MeasureFile"; 
const string	Configuration::PATH_RUN_MEASURE_CREATEPLOT		= "Settings/Misc/CreatePlot"; 
const string	Configuration::PATH_RUN_PRINT_PACKETS			= "Settings/Misc/PrintPackets";
const string	Configuration::PATH_RUN_SINGLE_STEPPING			= "Settings/Misc/SingleStepping";
const string	Configuration::PATH_RUN_BLOCK_SIZE_READ			= "Settings/Misc/BlockSizeRead";
const string	Configuration::PATH_RUN_BLOCK_SIZE_WRITE		= "Settings/Misc/BlockSizeWrite";
const string 	Configuration::PATH_RUN_IPv4_REPLACE_RANGE		= "Settings/Misc/IPv4ReplaceRange";
const string 	Configuration::PATH_RUN_IPv4_MAP_TO_RANGE		= "Settings/Misc/IPv4MapToRange";
const string 	Configuration::PATH_RUN_IPv6_REPLACE_RANGE		= "Settings/Misc/IPv6ReplaceRange";
const string 	Configuration::PATH_RUN_IPv6_MAP_TO_RANGE		= "Settings/Misc/IPv6MapToRange";
const string 	Configuration::PATH_RUN_MAC_REPLACE_FILE		= "Settings/Misc/MacReplaceFile";
const string 	Configuration::PATH_RUN_MAC_MAP_TO_RANGE		= "Settings/Misc/MacMapToRange";
const string 	Configuration::PATH_CHKSUM_RECALC				= "Settings/Checksumming/ReCalculateChecksums";
const string 	Configuration::PATH_CHKSUM_BAD_TO_BAD			= "Settings/Checksumming/SetBadChecksumsToBad";


const string 	Configuration::ANON_MODULE					= "AnonMappings";

const string 	Configuration::ANON_SUBMODULE_ETHERNET		= "EthernetPacket";
const string 	Configuration::ANON_SUBMODULE_VLAN			= "VlanPacket";
const string	Configuration::ANON_SUBMODULE_ARP			= "ArpPacket";
const string 	Configuration::ANON_SUBMODULE_IP			= "IpPacket";
const string 	Configuration::ANON_SUBMODULE_ICMP			= "IcmpPacket";
const string 	Configuration::ANON_SUBMODULE_TCP			= "TcpPacket";
const string 	Configuration::ANON_SUBMODULE_UDP			= "UdpPacket";
const string 	Configuration::ANON_SUBMODULE_PAYLOAD		= "PayloadPacket";
const string 	Configuration::ANON_SUBMODULE_IPV6			= "Ipv6Packet";

const string 	Configuration::ANON_ATTR_PRIMITIVE			= "anon";

Configuration*	Configuration::object					= NULL;	
string		Configuration::filename						= "";

void Configuration::init (const string& file)
{
	filename = file;
}

Configuration* Configuration::instance ()
{
	static Guard guard;

	if (object == NULL)
		object = new Configuration (filename);
	
	return object;
}

bool fileAccessible (string path)
{
	std::ifstream myFile;
    myFile.open (path);
	return myFile.is_open();
}

Configuration::Configuration (const string& filename)
:	tconf (filename, false, true), ipv4Valid(), ipv6Valid(), macValid()
{

	//
	// check if the configuration file is fine
	//

	if (! tconf.isGood ()) {
		cerr << "configuration error: " << tconf.getError () << std::endl;
		exit (0);
	}

	//
	// do other pktanon specific checks
	//

	if (getSetBadChecksumsToBad () == true && getReCalcChecksums () == false) {
		cerr	<< "configuration error: " 
				<< PATH_CHKSUM_BAD_TO_BAD 
				<< " is set to 1 but " 
				<< PATH_CHKSUM_RECALC
				<< " is set to 0"
				<< std::endl;
		exit (0);
	}

}

Configuration::~Configuration ()
{
}

string Configuration::getFlowSource ()
{
	string ret	= tconf.getConfigValue (PATH_RUN_FLOW_SOURCE);
	exitOnBad	();
	return		ret;
}

string Configuration::getFlowDest ()
{
	string ret	= tconf.getConfigValue (PATH_RUN_FLOW_DEST);
	exitOnBad	();
	return		ret;
}

bool Configuration::getUseMeasure ()
{
	static bool value		= false;
	static bool valueset	= false;

	if (valueset)
		return value;

	bool ret	= tconf.getConfigValue (PATH_RUN_MEASURE_ON).compare ("1") == 0;
	exitOnBad	();

	value		= ret;
	valueset	= true;

	return		value;
}

string Configuration::getMeasureFile ()
{
	string ret = tconf.getConfigValue (PATH_RUN_MEASURE_FILE);
	exitOnBad	();

	return ret;
}

bool Configuration::getCreatePlot ()
{
	bool ret	= tconf.getConfigValue (PATH_RUN_MEASURE_CREATEPLOT).compare ("1") == 0;
	exitOnBad	();

	return ret;
}

bool Configuration::getPrintPackets ()
{
	static bool value		= false;
	static bool valueset	= false;

	if (valueset)
		return value;

	bool ret	= tconf.getConfigValue (PATH_RUN_PRINT_PACKETS).compare ("1") == 0;
	exitOnBad	();

	value		= ret;
	valueset	= true;

	return value;
}

uint64_t Configuration::getMacMapToRange()
{
	string mac	= tconf.getConfigValue (PATH_RUN_MAC_MAP_TO_RANGE);
    exitOnBad	();

    int count = std::count(mac.begin(), mac.end(), ':');
    string macPart = mac.substr (0, mac.find("*"));

	if(macPart.at(macPart.length() - 1) == ':' ){
    	macPart = macPart.substr(0, macPart.length() - 1);
    }
    else{
    	exitOnBad(false, "Bad MAC Format for: MacMapToRange (Should be like 12:34:56:*)");
    }

    for(int i = count; i < 6; i++){
    	macPart.append(":00");
    }
    exitOnBad(macValid.validate(macPart), "Bad MAC Format for: MacMapToRange (Should be like 12:34:56:*)");

    return macValid.convertToLong(macPart);
}

int Configuration::getMacMapToIndex()
{
	string mac	= tconf.getConfigValue (PATH_RUN_MAC_MAP_TO_RANGE);
    exitOnBad	();

    int index = 6;
    string macPart;
    std::istringstream iss(mac);
    for(int i = 0; i < 6; i++) {
    	std::getline( iss, macPart , ':' );
       	if (macPart.compare("*") == 0){
       		index = i;
       		break;
       	}
    }
	exitOnBad(index < 5, "Mac Map To Range too short or not set  (should be like 12:34:56:*)");
    return index;
}

unordered_set<uint64_t> Configuration::getMacReplaceSet()
{
	string path	= tconf.getConfigValue (PATH_RUN_MAC_REPLACE_FILE);
	exitOnBad	();
	exitOnBad(fileAccessible(path), "MAC-File not accessible or does not exist");

	return	getMACsFromFile(path);
}

uint32_t Configuration::getIPv4ReplaceIp()
{
	string str	= tconf.getConfigValue (PATH_RUN_IPv4_REPLACE_RANGE);
    exitOnBad	();

    string ip = str.substr (0, str.find("/"));
    exitOnBad(ipv4Valid.validate(ip), "Bad IP Format for: IPv4ReplaceRange (should be like 192.168.1.0/24)");

	return ipv4Valid.convertToInt(ip);
}

int	Configuration::getIPv4ReplaceSubnet	()
{
	string str	= tconf.getConfigValue (PATH_RUN_IPv4_REPLACE_RANGE);
	exitOnBad	();

	int subnet = ipv4Valid.getSubnet(str.substr (str.find("/") + 1));
	exitOnBad((subnet > 0), "Bad Subnet for: IPv4ReplaceRange  (Should be a number between 1 - 31)");

	return subnet;
}


uint32_t Configuration::getIPv4MapToRange ()
{
	string ip	= tconf.getConfigValue (PATH_RUN_IPv4_MAP_TO_RANGE);
    exitOnBad	();

	int count = std::count(ip.begin(), ip.end(), '.');
    string ipPart = ip.substr (0, ip.find("*"));

	if(ipPart.at(ipPart.length() - 1) == '.' ){
		ipPart = ipPart.substr(0, ipPart.length() - 1);
	}
	else{
		exitOnBad(false, "Bad IP Format for: IPMapToRange (Should be like 192.168.*)");
	}

    for(int i = count; i < 4; i++){
        ipPart.append(".0");
    }


    exitOnBad(ipv4Valid.validate(ipPart), "Bad IP Format for: IPMapToRange (Should be like 192.168.*)");


    return	ipv4Valid.convertToInt(ipPart);
}

int Configuration::getIPv4MapToIndex()
{
	string str	= tconf.getConfigValue (PATH_RUN_IPv4_MAP_TO_RANGE);
    exitOnBad	();

    int index = 4;
    string ipPart;
    std::istringstream iss(str);
    for(int i = 0; i < 4; i++){
    	std::getline( iss, ipPart , '.' );
       	if (ipPart.compare("*") == 0){
       		index = i;
       		break;
       	}
    }
    exitOnBad(index < 3, "IP Map To Range too short or not set  (should be like 192.168.*)");
	return index;
}

bitset<128> Configuration::getIPv6ReplaceIp ()
{
	string str	= tconf.getConfigValue (PATH_RUN_IPv6_REPLACE_RANGE);
    exitOnBad	();

    string ipv6 = str.substr (0, str.find("/"));
	exitOnBad(ipv6Valid.validate(ipv6), "Bad IP Format for: IPv6ReplaceRange (Should be like 234:5678:abc::1)");

    return ipv6Valid.convertToBit128(ipv6);

}

int Configuration::getIPv6Subnet()
{
	string str	= tconf.getConfigValue (PATH_RUN_IPv6_REPLACE_RANGE);
    exitOnBad	();

    int subnet = ipv6Valid.getSubnet(str.substr (str.find("/") + 1));
    exitOnBad((subnet > 0), "Bad Subnet for: IPv6ReplaceRange  (Should be a number between 24 - 127)");

    return subnet;
}


bool Configuration::getReCalcChecksums ()
{
	// we put this static as it will be requested
	// quite often. So we don't produce overhead
	// with reading the same stuff from xml over and
	// over again ...
	static bool value		= false;
	static bool valueset	= false;

	if (valueset)
		return value;

	bool ret	= tconf.getConfigValue (PATH_CHKSUM_RECALC).compare ("1") == 0;
	exitOnBad	();

	value		= ret;
	valueset	= true;
	return		value;
}

bool Configuration::getSingleStepping ()
{
	static bool value		= false;
	static bool valueset	= false;

	if (valueset)
		return value;

	bool ret	= tconf.getConfigValue (PATH_RUN_SINGLE_STEPPING).compare ("1") == 0;
	exitOnBad	();

	value		= ret;
	valueset	= true;
	return		value;
}

unsigned long Configuration::getReadBlockSize ()
{
	string val = tconf.getConfigValue (PATH_RUN_BLOCK_SIZE_READ);
	if (val.length() <= 0) return 0;

	return strtoul (val.c_str(), NULL, 10);
}

unsigned long Configuration::getWriteBlockSize ()
{
	string val = tconf.getConfigValue (PATH_RUN_BLOCK_SIZE_WRITE);
	if (val.length() <= 0) return 0;

	return strtoul (val.c_str(), NULL, 10);
}

bool Configuration::getSetBadChecksumsToBad	()
{
	static bool value		= false;
	static bool valueset	= false;

	if (valueset)
		return value;

	bool ret	= tconf.getConfigValue (PATH_CHKSUM_BAD_TO_BAD).compare ("1") == 0;
	exitOnBad	();

	value		= ret;
	valueset	= true;
	return		value;
}

void Configuration::exitOnBad ()
{
	if (! tconf.isGood ()) {
		cerr << "configuration error: " << tconf.getError () << std::endl;
		exit (0);
	}
}

void Configuration::exitOnBad (bool ok, string error)
{
	if (! ok) {
		cerr << error << std::endl;
		exit (0);
	}
}

void Configuration::bindAnonPrimitives ()
{
	AnonFactory factory;

	//
	// configure ethernet packet
	//

	if (tconf.existsSubmodule (ANON_MODULE, ANON_SUBMODULE_ETHERNET)) {

		EthernetPacket::anonSourceMac	= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_ETHERNET, 
												"MacSource"
											)
															);

		EthernetPacket::anonDestMac		= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_ETHERNET, 
												"MacDest"
											)
															);

		EthernetPacket::anonType		= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_ETHERNET, 
												"MacType"
											)
															);

	} else {
		cerr << "no ethernet configuration found" << std::endl;
		exit (0);
	}

	//
	// configure vlan packet
	//

	if (tconf.existsSubmodule (ANON_MODULE, ANON_SUBMODULE_VLAN)) {

		VlanPacket::anonPriority	= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE,
												ANON_SUBMODULE_VLAN,
												"VlanPriority"
											)
															);

		VlanPacket::anonCfi		= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE,
												ANON_SUBMODULE_VLAN,
												"VlanCfi"
											)
															);

		VlanPacket::anonVlanId		= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE,
												ANON_SUBMODULE_VLAN,
												"VlanId"
											)
															);
		VlanPacket::anonType		= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE,
												ANON_SUBMODULE_VLAN,
												"VlanType"
											)
															);

	} else {
		cerr << "no vlan configuration found" << std::endl;
		exit (0);
	}

	//
	// configure ip packet
	//

	if (tconf.existsSubmodule (ANON_MODULE, ANON_SUBMODULE_IP)) {
		
		IpPacket::anonTos		= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_IP, 
												"IpTos"
											)
													);

		IpPacket::anonIdentifier = factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_IP, 
												"IpIdent"
											)
													);


		IpPacket::anonFlags		= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_IP, 
												"IpFlags"
											)
													);


		IpPacket::anonFragoffset= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_IP, 
												"IpFragoffset"
											)
													);


		IpPacket::anonTtl		= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_IP, 
												"IpTtl"
											)
													);


		IpPacket::anonSourceip	= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_IP, 
												"IpSourceip"
											)
													);


		IpPacket::anonDestip	= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_IP, 
												"IpDestip"
											)
													);


		IpPacket::anonOptions	= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_IP, 
												"IpOptions"
											)
													);

	} else {
		cerr << "no ip configuration found" << std::endl;
		exit (0);
	}

	//
	// configure icmp packet
	//

	if (tconf.existsSubmodule (ANON_MODULE, ANON_SUBMODULE_ICMP)) {
		
		IcmpPacket::anonType	= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_ICMP, 
												"IcmpType"
											)
													);

		IcmpPacket::anonCode	= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_ICMP, 
												"IcmpCode"
											)
													);

		IcmpPacket::anonMisc	= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_ICMP, 
												"IcmpMisc"
											)
													);

	} else {
		cerr << "no icmp configuration found" << std::endl;
		exit (0);
	}

	//
	// configure tcp packet
	//

	if (tconf.existsSubmodule (ANON_MODULE, ANON_SUBMODULE_TCP)) {
	
		TcpPacket::anonSourceport= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_TCP, 
												"TcpSourceport"
											)
													);

		TcpPacket::anonDestport	= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_TCP, 
												"TcpDestport"
											)
													);


		TcpPacket::anonSeqnum	= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_TCP, 
												"TcpSeqnum"
											)
													);


		TcpPacket::anonAcknum	= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_TCP, 
												"TcpAcknum"
											)
													);


		TcpPacket::anonFlags	= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_TCP, 
												"TcpFlags"
											)
													);


		TcpPacket::anonWindowsize= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_TCP, 
												"TcpWindowsize"
											)
													);


		TcpPacket::anonUrgentpnt= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_TCP, 
												"TcpUrgentpnt"
											)
													);

		TcpPacket::anonOptions	= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_TCP, 
												"TcpOptions"
											)
													);
	
	} else {
		cerr << "no tcp configuration found" << std::endl;
		exit (0);
	}

	//
	// configure udp packet
	//

	if (tconf.existsSubmodule (ANON_MODULE, ANON_SUBMODULE_UDP)) {

		UdpPacket::anonSourceport= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_UDP, 
												"UdpSourceport"
											)
													);

		UdpPacket::anonDestport	= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_UDP, 
												"UdpDestport"
											)
													);

	} else {
		cerr << "no udp configuration found" << std::endl;
		exit (0);
	}

	//
	// configure payloadpacket
	//


	if (tconf.existsSubmodule (ANON_MODULE, ANON_SUBMODULE_PAYLOAD)) {

		PayloadPacket::anonPayload= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_PAYLOAD, 
												"PayloadPacketData"
											)
													);

	} else {
		cerr << "no payload configuration found" << std::endl;
		exit (0);
	}

	//
	// configure arp packet
	//

	if(tconf.existsSubmodule (ANON_MODULE, ANON_SUBMODULE_ARP)) {

		ArpPacket::anonHardwaretype= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_ARP, 
												"ArpHardwaretp"
											)
													);

		ArpPacket::anonProtocoltype= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_ARP, 
												"ArpPrototp"
											)
													);

		ArpPacket::anonHardwareaddrlen= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_ARP, 
												"ArpHardwareaddrlen"
											)
													);

		ArpPacket::anonProtoaddrlen= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_ARP, 
												"ArpProtoaddrlen"
											)
													);

		ArpPacket::anonOpcode	= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_ARP, 
												"ArpOpcode"
											)
													);

		ArpPacket::anonSourcemac= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_ARP, 
												"ArpSourcemac"
											)
													);

		ArpPacket::anonSourceip	= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_ARP, 
												"ArpSourceip"
											)
													);

		ArpPacket::anonDestmac= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_ARP, 
												"ArpDestmac"
											)
													);

		ArpPacket::anonDestip	= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_ARP, 
												"ArpDestip"
											)
													);

	} else {
		cerr << "no arp configuration found" << std::endl;
		exit (0);
	}


	if(tconf.existsSubmodule (ANON_MODULE, ANON_SUBMODULE_IPV6)) {

		Ipv6Packet::anonTrafficclass= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_IPV6, 
												"Ipv6Trafficclass"
											)
													);

		Ipv6Packet::anonFlowlabel= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_IPV6, 
												"Ipv6Flowlabel"
											)
													);

		Ipv6Packet::anonHoplimit= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_IPV6, 
												"Ipv6Hoplimit"
											)
													);
													

		Ipv6Packet::anonSourceaddr= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_IPV6, 
												"Ipv6Sourceaddr"
											)
													);
													
		Ipv6Packet::anonDestaddr= factory.create	(
			tconf.getConfigItemAttributes	(	ANON_MODULE, 
												ANON_SUBMODULE_IPV6, 
												"Ipv6Destaddr"
											)
													);

	} else {
		cerr << "no ipv6 configuration found" << std::endl;
		exit (0);
	}

}

bool Configuration::checkAnonObj (AnonPrimitive* anon, string name)
{
	if (anon == NULL) {
		cerr << "anonymization primitive " << name << " not set" << std::endl;
		return false;
	}

	return true;
}

unordered_set<uint64_t> Configuration::getMACsFromFile(string path)
{
		std::ifstream file( path );
        string line;
        uint64_t mac;
        unordered_set<uint64_t> givenMACs;
		bool ok;
		int lineNr = 1;

        while( std::getline( file, line ) ){
        	ok = macValid.validate(line);
        	exitOnBad(ok, "Bad MAC-Address format in File: " + path + " in Line:" + std::to_string(lineNr) + "\n Is:" + line + " \tbut should be like: 12:34:56:78:90:ab");
        	mac = macValid.convertToLong(line);
        	givenMACs.insert(mac);
        	lineNr++;
        }

        return givenMACs;
}

unordered_set<bitset<128>> Configuration::getIPv6FromFile(string path)
{
		std::ifstream file( path );
        string line;
        bitset<128> ip;
        unordered_set<bitset<128>> givenIPs;
		bool ok;
		int lineNr = 1;

        while( std::getline( file, line ) ){
        	ok = ipv6Valid.validate(line);
        	exitOnBad(ok, "Bad IPv6-Address format in File: " + path + " in Line: " + std::to_string(lineNr) + "\n Is:" + line + " \tbut should be like: 1234:334:156::78:ab92");
        	ip = macValid.convertToLong(line);
        	givenIPs.insert(ip);
        	lineNr++;
        }

        return givenIPs;
}

bool Configuration::configOk ()
{
	bool ok = true;

	ok &= checkAnonObj (EthernetPacket::anonDestMac,	"EthernetPacket::anonDestMac");
	ok &= checkAnonObj (EthernetPacket::anonSourceMac,	"EthernetPacket::anonSourceMac");
	ok &= checkAnonObj (EthernetPacket::anonType,		"EthernetPacket::anonType");
	ok &= checkAnonObj (VlanPacket::anonPriority,		"VlanPacket::anonPriority");
	ok &= checkAnonObj (VlanPacket::anonCfi,			"VlanPacket::anonCfi");
	ok &= checkAnonObj (VlanPacket::anonVlanId,			"VlanPacket::anonVlanId");
	ok &= checkAnonObj (VlanPacket::anonType,			"VlanPacket::anonType");
	ok &= checkAnonObj (ArpPacket::anonDestip,			"ArpPacket::anonDestip");
	ok &= checkAnonObj (ArpPacket::anonDestmac,			"ArpPacket::anonDestmac");
	ok &= checkAnonObj (ArpPacket::anonHardwareaddrlen,	"ArpPacket::anonHardwareaddrlen");
	ok &= checkAnonObj (ArpPacket::anonHardwaretype,	"ArpPacket::anonHardwaretype");
	ok &= checkAnonObj (ArpPacket::anonOpcode,			"ArpPacket::anonOpcode");
	ok &= checkAnonObj (ArpPacket::anonProtoaddrlen,	"ArpPacket::anonProtoaddrlen");
	ok &= checkAnonObj (ArpPacket::anonProtocoltype,	"ArpPacket::anonProtocoltype");
	ok &= checkAnonObj (ArpPacket::anonSourceip,		"ArpPacket::anonSourceip");
	ok &= checkAnonObj (ArpPacket::anonSourcemac,		"ArpPacket::anonSourcemac");
	ok &= checkAnonObj (IpPacket::anonDestip,			"IpPacket::anonDestip");
	ok &= checkAnonObj (IpPacket::anonFlags,			"IpPacket::anonFlags");
	ok &= checkAnonObj (IpPacket::anonFragoffset,		"IpPacket::anonFragoffset");
	ok &= checkAnonObj (IpPacket::anonIdentifier,		"IpPacket::anonIdentifier");
	ok &= checkAnonObj (IpPacket::anonOptions,			"IpPacket::anonOptions");
	ok &= checkAnonObj (IpPacket::anonSourceip,			"IpPacket::anonSourceip");
	ok &= checkAnonObj (IpPacket::anonTos,				"IpPacket::anonTos");
	ok &= checkAnonObj (IpPacket::anonTtl,				"IpPacket::anonTtl");
	ok &= checkAnonObj (Ipv6Packet::anonDestaddr,		"Ipv6Packet::anonDestaddr");
	ok &= checkAnonObj (Ipv6Packet::anonFlowlabel,		"Ipv6Packet::anonFlowlabel");
	ok &= checkAnonObj (Ipv6Packet::anonHoplimit,		"Ipv6Packet::anonHoplimit");
	ok &= checkAnonObj (Ipv6Packet::anonSourceaddr,		"Ipv6Packet::anonSourceaddr");
	ok &= checkAnonObj (Ipv6Packet::anonTrafficclass,	"Ipv6Packet::anonTrafficclass");
	ok &= checkAnonObj (UdpPacket::anonDestport,		"UdpPacket::anonDestport");
	ok &= checkAnonObj (UdpPacket::anonSourceport,		"UdpPacket::anonSourceport");
	ok &= checkAnonObj (TcpPacket::anonAcknum,			"TcpPacket::anonAcknum");
	ok &= checkAnonObj (TcpPacket::anonDestport,		"TcpPacket::anonDestport");
	ok &= checkAnonObj (TcpPacket::anonFlags,			"TcpPacket::anonFlags");
	ok &= checkAnonObj (TcpPacket::anonOptions,			"TcpPacket::anonOptions");
	ok &= checkAnonObj (TcpPacket::anonSeqnum,			"TcpPacket::anonSeqnum");
	ok &= checkAnonObj (TcpPacket::anonSourceport,		"TcpPacket::anonSourceport");
	ok &= checkAnonObj (TcpPacket::anonUrgentpnt,		"TcpPacket::anonUrgentpnt");
	ok &= checkAnonObj (TcpPacket::anonWindowsize,		"TcpPacket::anonWindowsize");
	ok &= checkAnonObj (IcmpPacket::anonCode,			"IcmpPacket::anonCode");
	ok &= checkAnonObj (IcmpPacket::anonMisc,			"IcmpPacket::anonMisc");
	ok &= checkAnonObj (IcmpPacket::anonType,			"IcmpPacket::anonType");
	ok &= checkAnonObj (PayloadPacket::anonPayload, 	"PayloadPacket::anonPayload");

	return ok;
}

void Configuration::releaseAnonPrimitivs() {

	delete EthernetPacket::anonDestMac;
	delete EthernetPacket::anonSourceMac;
	delete EthernetPacket::anonType;
	delete VlanPacket::anonPriority;
	delete VlanPacket::anonCfi;
	delete VlanPacket::anonVlanId;
	delete VlanPacket::anonType;
	delete ArpPacket::anonDestip;
	delete ArpPacket::anonDestmac;
	delete ArpPacket::anonHardwareaddrlen;
	delete ArpPacket::anonHardwaretype;
	delete ArpPacket::anonOpcode;
	delete ArpPacket::anonProtoaddrlen;
	delete ArpPacket::anonProtocoltype;
	delete ArpPacket::anonSourceip;
	delete ArpPacket::anonSourcemac;
	delete IpPacket::anonDestip;
	delete IpPacket::anonFlags;
	delete IpPacket::anonFragoffset;
	delete IpPacket::anonIdentifier;
	delete IpPacket::anonOptions;
	delete IpPacket::anonSourceip;
	delete IpPacket::anonTos;
	delete IpPacket::anonTtl;
	delete Ipv6Packet::anonDestaddr;
	delete Ipv6Packet::anonFlowlabel;
	delete Ipv6Packet::anonHoplimit;
	delete Ipv6Packet::anonSourceaddr;
	delete Ipv6Packet::anonTrafficclass;
	delete UdpPacket::anonDestport;
	delete UdpPacket::anonSourceport;
	delete TcpPacket::anonAcknum;
	delete TcpPacket::anonDestport;
	delete TcpPacket::anonFlags;
	delete TcpPacket::anonOptions;
	delete TcpPacket::anonSeqnum;
	delete TcpPacket::anonSourceport;
	delete TcpPacket::anonUrgentpnt;
	delete TcpPacket::anonWindowsize;
	delete IcmpPacket::anonCode;
	delete IcmpPacket::anonMisc;
	delete IcmpPacket::anonType;
	delete PayloadPacket::anonPayload;
}
