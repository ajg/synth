
#include <iostream>
#include <ajg/synth.hpp>

#ifndef AJG_SYNTH_NO_WCHAR_T

namespace synth = ajg::synth;

typedef synth::default_traits<wchar_t>                      traits_type;
typedef synth::engines::ssi::engine<traits_type>            engine_type;
typedef synth::templates::string_template<engine_type>      template_type;

int wmain() {
    template_type const t(L"Howdy, <!--#echo var=\"user\" -->!");
    template_type::context_type c;
    c[L"user"] = L"Dolph Lundgren";

    // Render to different destinations:
    t.render_to_stream(std::wcout);
    t.render_to_path(L"greeting.txt", c);
    std::wcout << t.render_to_string(c);
    return 0;
}

#endif
