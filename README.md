Cuboids
=======

This will be a general XxYxZ cuboid solver. Shapeshifting for strangely proportional cuboids will not be supported: turns will be limited to double turns on non-square faces. The solver itself will use heuristic pruning tables in order to find optimal solutions.

Structure
=========

Here are the primary source directories and their significance:

 * `representation/` the primary cuboid data representation. This includes slice and face turns.
 * `stickers/` a lightweight library which reads and writes sticker data to/from a cuboid.
 * `test/` the unit tests for the entire project.
 * `algebra/` this is used to detect if various parts of the cube are solved. It also serves for algebra operations like inverses.
 * `notation/` this is my implementation of an extended WCA move notation.

General Todos
=============

I plan on implementing these features in the following order:

 * A generalized breadth-first search engine with multithread support, resuming/pausing, and controllable progress reports.
 * A higher-level searcher which uses `Cuboid`s as nodes and `Algorithm`s as paths to expand.
 * A standard cuboid basis generator.
 * A command-line interface to call upon different *solvers* and provide arguments.
 * A heuristic indexer with full support for plugins, symmetries, and control over cuboid operations.
 * A heuristic searcher which will allow easy integration into the cuboid searcher.
 * A small suite of programs for manipulating algorithms (i.e. rotating them, taking inverses, etc.)
 * *(Potentially)* a program which can identify impossible algorithms on a given shapeshifting cuboid and which allows for algorithms to be filtered accordingly.
