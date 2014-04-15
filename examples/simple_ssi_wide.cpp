
#include <iostream>
#include <ajg/synth.hpp>

#ifndef AJG_SYNTH_NO_WCHAR_T

namespace synth = ajg::synth;

typedef synth::default_traits<wchar_t>          traits_type;
typedef synth::ssi::engine<traits_type>         engine_type;
typedef synth::string_template<engine_type>     template_type;

int wmain() {
    template_type const tpl(L"Howdy, <!--#echo var=\"user\" -->!");
    template_type::context_type ctx;
    ctx[L"user"] = L"Dolph Lundgren";

    // Render to different targets:
    tpl.render_to_stream(std::wcout);
    tpl.render_to_path(L"greeting.txt", ctx);
    std::wcout << tpl.render_to_string(ctx);
    return 0;
}

#endif
