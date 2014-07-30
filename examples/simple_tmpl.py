
import synth, sys

def simple_tmpl_example():
    t = synth.Template('Howdy, <TMPL_VAR user>!', 'tmpl')
    c = {'user': 'Dolph Lundgren'}

    # Render to different destinations:
    t.render_to_path("greeting.txt", c)
    t.render_to_file(sys.stdout, c)
    print(t.render_to_string(c))
