
#include <iostream>
#include <ajg/synth.hpp>

namespace synth = ajg::synth;

#ifndef AJG_SYNTH_NO_WCHAR_T

typedef synth::default_traits<wchar_t>                              traits_type;
typedef synth::string_template<traits_type, synth::ssi::engine<> >  template_type;

int wmain() {
    template_type const tpl(L"Howdy, <!--#echo var=\"user\" -->!");
    template_type::context_type ctx;
    ctx[L"user"] = L"Dolph Lundgren";

    // Render to different targets:
    tpl.render(std::wcout);
    tpl.render_to_file(L"greeting.txt", ctx);
    std::wcout << tpl.render_to_string(ctx);
    return 0;
}

#endif
