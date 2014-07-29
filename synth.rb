require "formula"

class Synth < Formula
  homepage "https://github.com/ajg/synth"
  head "https://github.com/ajg/synth.git"
  url "https://github.com/ajg/synth/releases/download/v1.0.0/archive.tar.gz"
  sha1 "41971a1090b5a766959170efe77671996002ef03"
  version "1.0.0"

  depends_on "scons"  => :build
  depends_on "python" => :optional

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
