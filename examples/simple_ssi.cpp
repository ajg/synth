
#include <map>
#include <string>
#include <iostream>
#include <ajg/synth.hpp>

typedef ajg::synth::default_traits<char>                    traits_type;
typedef ajg::synth::engines::ssi::engine<traits_type>       engine_type;
typedef ajg::synth::templates::string_template<engine_type> template_type;

int main() {
    // Parse the template.
    template_type const t(
        "Howdy, <!--#echo var='user' -->! "
        "Your balance is <!--#echo var='points' -->.");

    // Create some data.
    std::map<std::string, engine_type::value_type> m;
    m["user"] = "Dolph Lundgren";
    m["points"] = 42;

    // Render to different destinations:
    t.render_to_stream(std::cout, m);
    t.render_to_path("greeting.txt", m);
    std::cout << t.render_to_string(m);
    return 0;
}
