//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_CONFIG_HPP_INCLUDED
#define AJG_SYNTH_CONFIG_HPP_INCLUDED

#define AJG_SYNTH_VERSION        0.7.0
#define AJG_SYNTH_VERSION_STRING BOOST_PP_STRINGIZE(AJG_SYNTH_VERSION)

#ifndef AJG_SYNTH_SEQUENCE_LIMIT
#define AJG_SYNTH_SEQUENCE_LIMIT 60
#endif

#ifndef AJG_SYNTH_CONSTRUCT_LIMIT
#define AJG_SYNTH_CONSTRUCT_LIMIT 10
#endif

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_FUSION_DONT_USE_PREPROCESSED_FILES

// #define BOOST_MPL_LIMIT_VECTOR_SIZE AJG_SYNTH_SEQUENCE_LIMIT
#define FUSION_MAX_MAP_SIZE            AJG_SYNTH_SEQUENCE_LIMIT
#define FUSION_MAX_VECTOR_SIZE         AJG_SYNTH_SEQUENCE_LIMIT

#include <boost/preprocessor/stringize.hpp>

#endif // AJG_SYNTH_CONFIG_HPP_INCLUDED
