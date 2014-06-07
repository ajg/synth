
#include <map>
#include <string>
#include <iostream>
#include <ajg/synth.hpp>

#ifndef AJG_SYNTH_CONFIG_NO_WCHAR_T

typedef ajg::synth::default_traits<wchar_t>                 traits_type;
typedef ajg::synth::engines::ssi::engine<traits_type>       engine_type;
typedef ajg::synth::templates::string_template<engine_type> template_type;

int wmain() {
    // Parse the template.
    template_type const t(
        L"Howdy, <!--#echo var='user' -->! "
        L"Your balance is <!--#echo var='points' -->.");

    // Create some data.
    std::map<std::wstring, engine_type::value_type> m;
    m[L"user"] = L"Dolph Lundgren";
    m[L"points"] = 42;

    // Render to different destinations:
    t.render_to_stream(std::wcout, m);
    t.render_to_path(L"greeting.txt", m);
    std::wcout << t.render_to_string(m);
    return 0;
}

#endif
