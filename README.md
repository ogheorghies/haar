Haar transform implementation (personal experiment)

```
$ echo "1 1 1 1" | ./bin/haar-transform
0.000000 0.000000 0.000000 2.000000
```
```
echo "1 1" | ./bin/haar-transform
0.000000 1.414214
```

The order of the output is reversed from the one given in [Zhu04](https://cs.nyu.edu/media/publications/zhu_yunyue.pdf), page 59.
To get the same order as in [Zhu04](https://cs.nyu.edu/media/publications/zhu_yunyue.pdf), use:

```
$ echo "1 3 5 11 12 13 0 1" | ./bin/haar-transform | tr ' ' '\n' | tail -r | tr '\n' ' '
16.263456 -2.121320 12.000000 -6.000000 -0.707107 -0.707107 -4.242641 -1.414214
```

# Run individual tests

```
$ grep INPUT ./transform/tests/data/0040.in.txt | cut -c9-
1 1 1 1

$ grep INPUT ./transform/tests/data/0040.in.txt | cut -c9- | ./bin/haar-transform
0.000000 0.000000 0.000000 2.000000

$ grep INPUT ./transform/tests/data/0040.in.txt | cut -c9- | ./bin/haar-transform | tr ' ' '\n' | tail -r | tr '\n' ' '
2.000000 0.000000 0.000000 0.000000
```

# Development

```
cmake .
cmake --build .
./transform/tests/suite.sh
```