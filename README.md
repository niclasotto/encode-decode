# !DISCLAIMER!
this hasn't been tested thoroughly on every platform. It works on Ubuntu 25.04 but nothing else is guaranteed to work

---

## encode-decode
Simple encoder/decoder written by the vscode copilot/github copilot

---
To compile, just use:
```
g++ encoder.cpp -o encoder -lz
```

or:
```
g++ decoder.cpp -o decoder -lz
```

---
Syntax for usage:

Encoder:
```
./encoder <yourfilename.ending>
```

Decoder:
```
./decoder <yourfilename.ending.cmpfile>
```

---

### How to set both as a global variable?

If you want to access the encoder/decoder from anywhere in your system, execute the move_both file
