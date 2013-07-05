Entering a cuboid
=================

All solvers in this project with prompt you to enter a scramble. This process can be confusing to newcomers, but is quite standardized and straight forward.

Cuboid entry hinges completely on stickers. A number 1-6 is assigned to each color. It is my suggestion that you use the following color scheme: 1 = white, 2 = yellow, 3 = blue, 4 = green, 5 = red, 6 = orange. Alternatively, let 1 = front, 2 = back, 3 = up, 4 = down, 5 = right, 6 = left.

The solver's goal is to make the front side all 1, back side all 2, etc. However, the solver applies all dimension-invariant rotations to the cube to check for a goal state. For this reason, entering an even cube such as a 4x4x4 is quite arbitrary: the resulting cube may be any of the 24 rotations applied to the standard 1 front, 3 up color scheme (i.e. 3 front 2 up).

When entering a cube, you can pick any arbitrary rotation of the 1 front, 3 up color scheme. For example, you could enter 2 for the top center and 3 for the front center (with 4 back, 5 right, 6 left, and 1 down). While this is nice for cubes, entry is more strict for rectangular cuboids. For example, you may wish to enter a 3x2x3. In this case, you can only choose 1, 2, 5, or 6 for the front sticker color, and the top/bottom must only contain 3 and 4 stickers since the cuboid can only be rotated with &lt;x2, y, z2&gt; (or `212`) in order to maintain its dimensions.

The number of stickers on each face is something to note. On a cuboid of dimensions XxYxZ, the front and back faces are XxY, the top and bottom faces are XxZ, and the right and left faces are ZxY.

Enter the stickers on a face from left to right, top to bottom (like reading an English book). For example, the front face of a superflipped 3x3x3 could be entered as follows: 131615141.

The cuboid entry prompt will ask you to enter face after face in the following order: F, B, U, D, R, L. Enter the front face as you might expect. Then, perform a y2 to enter the back. Next, perform a y2 x' and enter the top face (which is now in the F position). Next, use an x2 for the bottom face, then an x' y for the right face, and finally a y2 for the left face.

Compilation
===========

If you don't know how to use a terminal, you are not going to be able nor do you deserve to use this program. In all seriousness, however, you can compile this project on a standard UNIX or Linux environment by running a simple `make`. In addition, a `make test` will compile all of the test binaries in the `test/` directory. 

Running the solver
==================

You may execute the `solver` binary in the `solver/` source directory through a standard command-line interface. Running the binary with no arguments will display usage information. As the first argument, you may pass the name of one of several solvers (i.e. sub-programs), a *resume* directive, or a *help* directive.

The solvers may take their own arguments in addition to the general solver arguments. To view these arguments, use the help directive as follows: `./solver help <solver>`. For example:

	$ ./solver help eo
	Usage: solver eo --axis=<num> --centers

While the *help* directive lists the various arguments for each solver, this document will include a more detailed description of each solver and its arguments.

The *resume* directive takes a file argument. This allows you to resume a solve session which you stopped mid-way through. If one or more heuristic files were in use during this session, the files located at those paths must exist when the solve is resumed.

Creating heuristic databases
============================

Heuristic databases, or *indexes*, make it possible to accelerate the solving process. You must either generate index files yourself or download pre-made indexes. The indexing system is designed to be easily expandable, allowing for easy cusomization.

The `indexer` binary located in the `indexer/` directory makes it easy to generate indexes. Running the binary with no arguments will display usage information.

What the usage information leaves out is the elaborate symmetry system which the indexer utilizes. Because the solver and indexer both support even cuboids and slice turns, these solvers allow for solutions which leave the cube in one of multiple orientations. Usually, the solver rotates each node in all possible orientations and looks up the heuristic value for each orientation.

While the rotate+lookup model works for symmetry movesets, suppose you wish to index with a moveset which is asymmetric. For example, imagine the moveset <F,U,L,D,B>, lacking any R turns. In this case, the moveset is only symmetric under x rotations. To handle this, the indexer allows you to enter the rotations under which the moveset is symmetric. The indexer will then index enough different orientations of the cuboid such that the resulting heuristic will never return an excessive heuristic value.

The moveset symmetries may be entered through the `--symmetries` argument. This argument encodes a tuple, <x^a, y^b, z^c>, as a 3-digit base 3 number. For example, `102` encodes to the rotation group generated by <x, z2>. For the moveset in the above paragraph, the appropriate tuple would be <x>, or `100`.

Another thing which you may wish to note: the `--sharddepth` argument is a very useful tool for different index types. While I will refrain from detailing how my heuristic indexer deals with the data returned from an index type, it is best to tweak the shard depth depending on the index type. For edge orientation data, I recommend a depth of 2 at most, whereas I would suggest a higher shard depth such as 5 for the `dedgepair` index type.

As a full usage example, this is how I would index the corners of any cube:

	./indexer corners output.anc3 --dimensions 2x2x2 --sharddepth=4 --maxdepth=9

Using a solver
==============

Here is how I would solve a 3x3x3 using a corners index and two edge index files:

	$ mkdir tables; cd indexer
	$ ./indexer corners ../tables/corners.anc3 --dimensions 2x2x2
	--sharddepth=4 --maxdepth=9
	…
	$ ./indexer dedges ../tables/edge_1-6.anc3 --dimensions 3x3x3
	--sharddepth=4 --dedges "111111000000"
	…
	$ ./indexer dedges ../tables/edge_7-12.anc3 --dimensions 3x3x3
	--sharddepth=4 --dedges "000000111111"
	…
	$ cd ../solver
	$ ./solver standard --heuristic ../tables/corners.anc3 --heuristic ../tables/edge_1-6.anc3 --heuristic ../tables/edge_7-12.anc3
     Enter front face: 143613122
      Enter back face: 253524452
       Enter top face: 614134465
    Enter bottom face: 633142546
     Enter right face: 116552561
      Enter left face: 265263433
    Exploring depth of 0...
    Exploring depth of 1...
    Exploring depth of 2...
    Exploring depth of 3...
    Exploring depth of 4...
    Exploring depth of 5...
    Exploring depth of 6...
    Exploring depth of 7...
    Exploring depth of 8...
    Exploring depth of 9...
    Exploring depth of 10...
    Exploring depth of 11...
    Found solution: D R2 F U2 L' D U2 L U R' D2
