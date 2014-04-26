require "formula"

class Synth < Formula
  homepage "https://github.com/ajg/synth"
  head "https://github.com/ajg/synth.git"
  url "https://github.com/ajg/synth/archive/v0.52.0.tar.gz"
  sha1 "a5ba180c0d8a53c67859da1b49958352c4d807f7"

  depends_on "scons"  => :build
  depends_on "python" => :optional
  depends_on "boost" if build.without? "python"
  depends_on "boost"  => "with-python" if build.with? "python"
  # TODO: debug bool option
  # TODO: boost enum option {auto,local,system}

  def install
    scons "synth", "debug=0", "boost=system"
    bin.install "synth"
    # TODO: include.install "ajg/synth.hpp", Dir["ajg/**/synth.hpp"]
    # See http://stackoverflow.com/q/23307205/1272391

    if build.with? "python"
      system "python", "setup.py", "install", "--prefix=#{prefix}"
    end
  end

  # TODO: bottle do ...

  test do
    system "#{bin}/synth", "--version"
  end
end
