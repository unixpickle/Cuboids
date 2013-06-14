Cuboids
=======

This will be a general XxYxZ cuboid solver. Shapeshifting for strangely proportional cuboids will not be supported: turns will be limited to double turns on non-square faces. The solver itself will use heuristic pruning tables in order to find optimal solutions.

Structure
=========

Here are the primary source directories and their significance:

 * `representation/` the primary cuboid data representation. This includes slice and face turns.
 * `stickers/` a lightweight library which reads and writes sticker data to/from a cuboid.
 * `test/` the unit tests for the entire project.