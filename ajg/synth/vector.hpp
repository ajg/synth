//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_VECTOR_HPP_INCLUDED
#define AJG_SYNTH_VECTOR_HPP_INCLUDED

#include <ajg/synth/config.hpp>

#if AJG_SYNTH_SEQUENCE_LIMIT <= 50

#include <boost/fusion/include/vector.hpp>

#else

#include <boost/fusion/include/vector50.hpp>

namespace boost { namespace mpl {

#   define BOOST_PP_ITERATION_PARAMS_1 \
    (3,(51, AJG_SYNTH_SEQUENCE_LIMIT, <boost/mpl/vector/aux_/numbered.hpp>))
#   include BOOST_PP_ITERATE()

}}

namespace boost { namespace fusion
{
    struct vector_tag;
    struct fusion_sequence_tag;
    struct random_access_traversal_tag;
// expand vector51 to vector{AJG_SYNTH_SEQUENCE_LIMIT}
#define BOOST_PP_FILENAME_1 <boost/fusion/container/vector/detail/vector_n.hpp>
#define BOOST_PP_ITERATION_LIMITS (51, AJG_SYNTH_SEQUENCE_LIMIT)
#include BOOST_PP_ITERATE()
}}

#endif // AJG_SYNTH_SEQUENCE_LIMIT > 50
#endif // AJG_SYNTH_VECTOR_HPP_INCLUDED
