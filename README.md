# Bit-Genie 

[![Latest Github release](https://img.shields.io/badge/Latest--Release-v5-blue)](https://github.com/Aryan1508/Bit-Genie/releases/latest)

[Bit-Genie](https://github.com/Aryan1508/Bit-Genie "Bit-Genie") is an open-source [chess engine](https://en.wikipedia.org/wiki/Chess_engine "chess engine") written in C++ by [Aryan Parekh](https://github.com/Aryan1508 "Aryan Parekh"). It has support for the [UCI](https://en.wikipedia.org/wiki/Universal_Chess_Interface) protocol.

# About
**TLDR; I got addicted to chess programming**

With  all the lockdowns in 2020, I decided that it would be a good time to start working on a new project. I have lots of love for programming; I have lots of love for chess, so a chess software seemed like a good start. I ended up creating a small & simple chess GUI which although wasn't easy, didn't satisfy me. I thought it would be interesting if I could write a program that could play *good* chess  and possibly beat me. And that's how I started writing Bit-Genie.  What I didn't know was chess programming is very addictive , and it would turn out to be more than a small project.  What inspired me the most was the huge community of open-source chess engines like [Ethereal](https://github.com/AndyGrant/Ethereal "Ethereal")

As of March 2020, Bit-Genie isn't a top-tier chess engine, but we're getting somewhere.

# Elo progression

The estimated strength over the different versions

### CCRL Blitz (2'+1") 

|   Version       |  Elo   |
| -------------   | ------ |
| Bit-Genie 1     |  1771  |
| Bit-Genie 2     |  1879  |
| Bit-Genie 4     |  2217  |


[Bit-Genie on the CCRL](https://ccrl.chessdom.com/ccrl/404/cgi/compare_engines.cgi?family=Bit-Genie&print=Rating+list&print=Results+table&print=LOS+table&print=Ponder+hit+table&print=Eval+difference+table&print=Comopp+gamenum+table&print=Overlap+table&print=Score+with+common+opponents)

# Usage
Bit-Genie is **not** a complete chess software, but a chess engine which still needs a GUI if you don't prefer writing and reading [UCI](https://en.wikipedia.org/wiki/Universal_Chess_Interface "UCI") commands through the terminal. There are plenty of free softwares available online, here are two common ones
- [Cute Chess](https://github.com/cutechess/cutechess "Cute Chess")
- [Arena Chess GUI](http://www.playwitharena.de/)


# Build 
Bit-Genie comes with a [makefile](https://github.com/Aryan1508/Bit-Genie/blob/master/Bit-Genie/makefile "makefile") that you can use to build your own binaries. 

```
git clone --single-branch --branch master https://github.com/Aryan1508/Bit-Genie
cd Bit-Genie/Bit-Genie
make 
./Bit-Genie
```

# Contributing

If you have any ideas/questions, feel free to reach out to me, I'm ready to help. 

-------------

# Acknowledgements

The development wouldn't have been possible if not for the huge resources available online like the [Chess programming Wiki](https://www.chessprogramming.org/Main_Page "Chess Programming Wiki"). I would like the following chess engines and their authors for their support. Their works have influenced the progress of Bit-Genie greatly
 
 - [Weiss](https://github.com/TerjeKir/weiss) (aiding in eval.cpp with tuned evaluation scores)
 - [Ethereal](https://github.com/AndyGrant/Ethereal)
 - [Halogen](https://github.com/KierenP/Halogen)
 - [Koivisto](https://github.com/Luecx/Koivisto)
 - [Drofa](https://github.com/justNo4b/Drofa)
 - [Stockfish](https://github.com/official-stockfish/Stockfish)
