#pragma once
#include "pch.hpp"


namespace VSIF {
    class ValveScenesImageFile;
}

#pragma once
namespace Helper {
	static inline uint32_t FourCC(char ID[4]) {
		return (ID[0] | (ID[1] << 8) | (ID[2] << 16) | (ID[3] << 24));
	}

    extern VSIF::ValveScenesImageFile* vsif;


    static inline void ReplaceAll(std::string &str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
        //return str;
    }

    static inline std::istream& safeGetline(std::istream& is, std::string& t)
    {
        t.clear();

        // The characters in the stream are read one-by-one using a std::streambuf.
        // That is faster than reading them one-by-one using the std::istream.
        // Code that uses streambuf this way must be guarded by a sentry object.
        // The sentry object performs various tasks,
        // such as thread synchronization and updating the stream state.

        std::istream::sentry se(is, true);
        std::streambuf* sb = is.rdbuf();

        for(;;) {
            int c = sb->sbumpc();
            switch (c) {
            case '\n':
                return is;
            case '\r':
                if(sb->sgetc() == '\n')
                    sb->sbumpc();
                return is;
            case std::streambuf::traits_type::eof():
                // Also handle the case when the last line has no line ending
                if(t.empty())
                    is.setstate(std::ios::eofbit);

                if (t.empty())
                    is.setstate(std::ios::badbit);
                return is;
            default:
                t += (char)c;
            }
        }
    }
};
