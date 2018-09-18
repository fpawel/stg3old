//---------------------------------------------------------------------------
#ifndef MY_BOOST_DYNAMIC_BITSET_HELPER_INCLUDED___
#define MY_BOOST_DYNAMIC_BITSET_HELPER_INCLUDED___
//---------------------------------------------------------------------------
// boost
#include "boost\dynamic_bitset.hpp"
//---------------------------------------------------------------------------
// std
//#include <string>
//#include <algorithm>
//#include <vector>
#include <sstream>
//---------------------------------------------------------------------------

namespace my
{
	std::string BitsetToStrF(boost::dynamic_bitset<> bs)
    {
    	std::stringstream ret;
        bool hasChars = false;

    	const size_t sz = bs.size();
        size_t i=0;
    	while( i<sz )
        {

            if ( !bs[i] )
            {
                ++i;
                continue;
            }
            if( !(i<sz) ) break;

            if( !hasChars )
            	ret << i;
            else
            	ret << " " << i;
            hasChars = true;

            if( !(++i<sz) ) break;
            if ( !bs[i] )
            {
                ++i;
                continue;
            }

            ret << "-";
            for( ; i<sz && bs[i]; ++i ) ;

            ret << i-1;

        }
        return ret.str();
    }

    boost::dynamic_bitset<> StrFToBitset( const std::string& s )
    {

    	using boost::dynamic_bitset;
    	dynamic_bitset<> bs(0);
        std::stringstream ss(s);
        while( !ss.eof() )
        {
        	size_t val;
            char ch;

            ss >> ch;
            if( ss.fail() || ch=='-' ) return bs;
            ss.putback(ch);

            ss >> val;
            if( ss.fail() ) return bs;

            if( bs.size()<val+1 )
            	bs.resize( val+1 );
            bs[val] = true;

            ss >> ch;
            if( ss.fail() ) return bs;
            if( ch!='-' )
            {
            	ss.putback(ch);
                continue;
            }

            size_t val_e;
            ss >> val_e;
            if( ss.fail() ) return bs;
            if( bs.size()<val_e+1 )
            	bs.resize( val_e+1 );


            for( ++val; val<=val_e; ++val )
              	bs[val] = true;
        }
        return bs;
    }

    bool GetBsBit( unsigned n, const boost::dynamic_bitset<> bs )
    { return n<bs.size() ? bs[n] : false; }





};
#endif

