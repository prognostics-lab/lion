# Documentation
Currently, building the documentation is only supported on Linux.

Building the documentation requires [Doxygen](https://doxygen.nl/index.html) to compile the API reference into XML and man files, [moxygen](https://github.com/sourcey/moxygen) to transpile the XML files into Markdown files, and [mdBook](https://github.com/rust-lang/mdbook) to compile the Markdown documentation (which includes the API reference and more) into HTML files. With these three tools installed, run
```bash
make docs
```
to compile the docs, or
```bash
make docs-serve
```
to compile and open the docs in your browser.

