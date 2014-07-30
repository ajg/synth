require "formula"

class Synth < Formula
  homepage "https://github.com/ajg/synth"
  head "https://github.com/ajg/synth.git"
  url "https://github.com/ajg/synth/releases/download/v1.0.1/archive.tar.gz"
  sha1 "e5aa53954050a0b0dd2fa05702b49f0241e1b38c"
  version "1.0.1"

  depends_on "scons"  => :build
  depends_on "python" => :optional
  # TODO: debug flag

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
