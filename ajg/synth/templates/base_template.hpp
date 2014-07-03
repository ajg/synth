//  (C) Copyright 2014 Alvaro J. Genial (http://alva.ro)
//  Use, modification and distribution are subject to the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).

#ifndef AJG_SYNTH_TEMPLATES_BASE_TEMPLATE_HPP_INCLUDED
#define AJG_SYNTH_TEMPLATES_BASE_TEMPLATE_HPP_INCLUDED

#include <map>
#include <string>
#include <cerrno>
#include <fstream>
#include <sstream>
#include <utility>
#include <stdexcept>

#include <boost/optional.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <ajg/synth/exceptions.hpp>
#include <ajg/synth/value_traits.hpp>

namespace ajg {
namespace synth {
namespace templates {

// TODO: Factor out an intermediate range_template (or iterator_template) for use with arbitrary iterators.
template <class Engine, class Iterator>
struct base_template : boost::noncopyable {
  protected:

    typedef base_template                                                       template_type;
    typedef Engine                                                              engine_type;
    typedef Iterator                                                            iterator_type;
    typedef typename engine_type::template kernel<iterator_type>                kernel_type;
    typedef boost::scoped_ptr<kernel_type const>                                local_kernel_type;

  public:

    typedef typename kernel_type::range_type                                    range_type;
    typedef typename kernel_type::state_type                                    state_type;

    typedef typename engine_type::value_type                                    value_type;
    typedef typename engine_type::context_type                                  context_type;
    typedef typename engine_type::options_type                                  options_type;
    typedef typename engine_type::traits_type                                   traits_type;

    typedef typename traits_type::boolean_type                                  boolean_type;
    typedef typename traits_type::char_type                                     char_type;
    typedef typename traits_type::size_type                                     size_type;
    typedef typename traits_type::string_type                                   string_type;
    typedef typename traits_type::istream_type                                  istream_type;
    typedef typename traits_type::ostream_type                                  ostream_type;
    typedef typename traits_type::path_type                                     path_type;
    typedef typename traits_type::paths_type                                    paths_type;

    typedef typename context_type::data_type                                    data_type;
    typedef typename context_type::metadata_type                                metadata_type;

  private:

    typedef detail::text<string_type>                                           text;

  protected:

    inline base_template(options_type const& options = options_type())
        { this->reset(options); }

    inline base_template(range_type const& range, options_type const& options = options_type())
        { this->reset(range, options); }

    inline base_template(iterator_type const& begin, iterator_type const& end, options_type const& options = options_type())
        { this->reset(begin, end, options); }

  public:

//
// render_to_stream
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline void render_to_stream(ostream_type& ostream, context_type& context) const {
        ostream.imbue(traits_type::standard_locale());

        this->kernel().render(ostream, this->state_.options, this->state_, context);
    }

    inline void render_to_stream(ostream_type& ostream, data_type const& data) const {
        context_type context(data, this->options().metadata);
        this->render_to_stream(ostream, context);
    }

//
// render_to_string
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline string_type render_to_string(context_type& context) const {
        std::basic_ostringstream<char_type> oss;
        this->render_to_stream(oss, context);
        return oss.str();
    }

    inline string_type render_to_string(data_type const& data) const {
        context_type context(data, this->options().metadata);
        return this->render_to_string(context);
    }

//
// render_to_path
////////////////////////////////////////////////////////////////////////////////////////////////////

    inline void render_to_path(path_type const& path, context_type& context) const {
        std::string const narrow_path = text::narrow(path);
        std::basic_ofstream<char_type> file;

        try {
            file.open(narrow_path.c_str(), std::ios::binary);
        }
        catch (std::exception const& e) {
            AJG_SYNTH_THROW(write_error(narrow_path, e.what()));
        }

        this->render_to_stream(file, context);
    }

    inline void render_to_path(path_type const& path, data_type const& data) const {
        context_type context(data, this->options().metadata);
        this->render_to_path(path, context);
    }

////////////////////////////////////////////////////////////////////////////////////////////////////

    inline string_type         str()     const { return string_type(this->range().first, this->range().second); }
    inline range_type   const& range()   const { return this->state_.range; }
    inline options_type const& options() const { return this->state_.options; }

  protected:

    inline void reset(options_type const& options = options_type()) {
        this->state_ = state_type(range_type(), options);
        // NOTE: Don't parse in this case.
    }

    inline void reset(iterator_type const& begin, iterator_type const& end, options_type const& options = options_type()) {
        this->state_ = state_type(range_type(begin, end), options);
        this->kernel().parse(this->state_);
    }

  private:

    inline static kernel_type const& kernel() {
        static kernel_type const kernel;
        return kernel;
    }

  private:

    state_type state_;
};

}}} // namespace ajg::synth::templates

#endif // AJG_SYNTH_TEMPLATES_BASE_TEMPLATE_HPP_INCLUDED
