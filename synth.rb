require "formula"

class Synth < Formula
  homepage "https://github.com/ajg/synth"
  head "https://github.com/ajg/synth.git"
  url "https://github.com/ajg/synth/releases/download/v1.0.3/archive.tar.gz"
  sha1 "8a5c9d09c9b95b5c4e72af94cedb9e53924fd933"
  version "1.0.3"

  depends_on "scons"  => :build
  depends_on "python" => :optional
  # TODO: Add debug flag

  def install
    scons "synth", "debug=0", "boost=local"
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
