// bulk of the includes are located here.

#include <iostream>

//From boost we need lzma, filesys, and serialization
// response_rules parser needs tokenizer
//in C++11 Boost::threads is now in std
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <boost/crc.hpp>

#include <boost/iostreams/filter/lzma.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

// see src/game/server/AI_responserules.cpp from source-sdk for reference implementation of response_rules parser

