# HastyHex : a faster hex dumper

HastyHex is a ***blazing fast*** hex dump utility with optional ANSI
color output. It performs about *one to two orders of magnitude* faster
than your typical implementation of `hexdump` or `od`. It's is written
in plain old ANSI C, so you can run it literally anywhere.

![](https://i.imgur.com/xbr4aMj.png)

## Usage

HastyHex produces color output by default regardless of what's connected
to the output. The `-p` option turns off color.

    usage: hastyhex [-fhlp] [-o FILE]
      -h       print this help message
      -l       force output line-buffered
      -f       force output fully-buffered
      -o FILE  output to file instead of standard output
      -p       do not output color ("plain")

The `less` pager has a `-R` argument that understands ANSI color escape
sequences, making it a great candidate for accepting output from
HastyHex.

    $ hastyhex data.bin | less -FRX

The `-f` option increases the output buffer size which typically
improves performance. Since MSVC doesn't support line-buffering, `-l`
will be equivalent to `-f` on Windows.
