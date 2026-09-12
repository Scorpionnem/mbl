#pragma once

#include "math/math.hpp"
#include <sys/time.h>

namespace mbl { namespace net {

#define MBL_PCKT_MAGIC 0xCACACACA

/// Packed wire-format structs shared between Client and Server, tagged by a leading Header. used privately by mbl
namespace	Packet
{
	struct	Header
	{
		u32	type;
		u32	magic = MBL_PCKT_MAGIC;
	} __attribute__((packed));

	#define RTTREQUEST_TYPE 1
	struct	RTTRequest
	{
		Packet::Header	hdr = {.type = RTTREQUEST_TYPE};
	} __attribute__((packed));
	#define RTTREPLY_TYPE 2
	struct	RTTReply
	{
		Packet::Header	hdr = {.type = RTTREPLY_TYPE};
	} __attribute__((packed));
};

}}
