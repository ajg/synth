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
#include <boost/throw_exception.hpp>

#include <ajg/synth/exceptions.hpp>
#include <ajg/synth/value_traits.hpp>
#include <ajg/synth/engines/detail.hpp>

namespace ajg {
namespace synth {
namespace templates {

using boost::throw_exception;

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
    typedef typename kernel_type::frame_type                                    frame_type;

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


  protected:

    base_template() : kernel_(&shared_kernel()) {}
    base_template(range_type const& range) : kernel_(&shared_kernel()) { this->reset(range); }
    base_template(iterator_type const& begin, iterator_type const& end) : kernel_(&shared_kernel()) { this->reset(begin, end); }

  public:

//
// render_to_stream
////////////////////////////////////////////////////////////////////////////////////////////////////

    void render_to_stream( ostream_type&       ostream
                         , context_type const& context = context_type()
                         , options_type const& options = options_type()
                         ) const {
        kernel_->render(ostream, frame_, context, options);
    }

//
// Convenience methods
////////////////////////////////////////////////////////////////////////////////////////////////////

    string_type render_to_string( context_type const& context = context_type()
                                , options_type const& options = options_type()
                                ) const {
        std::basic_ostringstream<char_type> ostream;
        kernel_->render(ostream, frame_, context, options);
        return ostream.str();
    }

    void render_to_path( path_type    const& path
                       , context_type const& context = context_type()
                       , options_type const& options = options_type()
                       ) const {
        std::string const narrow_path = traits_type::narrow(path);
        std::basic_ofstream<char_type> file;

        try {
            file.open(narrow_path.c_str(), std::ios::binary);
        }
        catch (std::exception const& e) {
            throw_exception(write_error(narrow_path, e.what()));
        }

        kernel_->render(file, this->frame_, context, options);
    }

    range_type const& range() const { return this->range_; }
    string_type       text()  const { return string_type(this->range_.first, this->range_.second); }

  private:

    inline static kernel_type const& shared_kernel() {
        static kernel_type const kernel;
        return kernel;
    }

    /*
    void mutate_locally() {
        if (!local_kernel_) {
            local_kernel_.reset(new kernel_type(shared_kernel()));
            std::swap(kernel_, local_kernel_.get());
        }
    }
    */

  protected:

    inline void reset() {
        this->range_ = range_type();
        this->frame_ = frame_type();
    }

    inline void reset(range_type const& range) {
        this->range_ = range;
        kernel_->parse(this->range_.first, this->range_.second, this->frame_);
    }

    inline void reset(iterator_type const& begin, iterator_type const& end) {
        this->reset(range_type(begin, end));
    }

  private:

    local_kernel_type   local_kernel_;
    kernel_type const*  kernel_;
    frame_type          frame_;
    range_type          range_;
};

}}} // namespace ajg::synth::templates

#endif // AJG_SYNTH_TEMPLATES_BASE_TEMPLATE_HPP_INCLUDED
