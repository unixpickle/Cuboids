Cuboids
=======

This is a general XxYxZ cuboid solver. Shapeshifting for strangely proportional cuboids will not be supported: turns will be limited to double turns on non-square faces. The solver itself will use heuristic pruning tables in order to find optimal solutions.

Structure
=========

Here are the primary source directories and their significance:

 * `representation/` the primary cuboid data representation. This includes slice and face turns.
 * `stickers/` a lightweight library which reads and writes sticker data to/from a cuboid.
 * `test/` the unit tests for the entire project.
 * `algebra/` this is used to detect if various parts of the cube are solved. It also serves for algebra operations like inverses, and algebraic structures like cosets and groups.
 * `notation/` this is my implementation of an extended WCA move notation.
 * `search/` the *search engine* for exploring cube permutations on multiple threads.
 * `arguments/` a command-line argument parser which will be useful throughout this project.
 * `saving/` the serialization system which will be used to pause/resume searches.
 * `heuristic/` the main system for heuristic lookups and indexing.
 * `pieces/` this is a system for reporting piece by piece info (e.g. orientation).
 * `indexer/` this is the executable CLI for the heuristic indexer.
 * `solver/` this is the executable CLI for the solver.

General Todos
=============

Here is my current to-do list:

 * Develop more solvers for various reduction tasks on big cubes and cuboids
 * Implement a small suite of programs for manipulating algorithms (i.e. rotating them, taking inverses, etc.)
 * *(Potentially)* a program which can identify impossible algorithms on a given shapeshifting cuboid and which allows for algorithms to be filtered accordingly.
