# Machine_Learning

Basic infrastrucure for machine learning built from the ground up.

## Getting Started

Some notes on getting a basic built up.

### Prereqs

This was built and tested with linux, but should work on windows. You'll need the nvcc command to be working on your machine. If that's not available, then look up how to get that up an running. You'll also need an nvidia gpu. If you're on linux, find a ppa for nvidia drivers.

### Initial Build

Unfortunately no scripts are available for building. Instead, just compile and link all the ".cpp" and ".cu" files (except for "hdmem.cpp"). "test.cpp" contains the main() function. Once you have an executable, running it will run a test.

## Usage

The "Brain" class is the top level class in charge of making everything happen. One brain is composed of several neural nets linked together in such a way that there are no loop and only one start point and one end point. Each net then has it's own layers in the traditional sense. A brain is specified by an antisymetric matrix (vector<vector<int>>), a list of "node" sizes (vector<int>), and a list of "layer" sizes (vector<vector<int>>).

### The matrix

The matrix indicates how the networks are attached. A 1 indicates the i'th net feeds into the j'th net, a -1 means the reverse, and a 0 means nets i and j are not attached.

For example, a brain which is a series of 3 nets in a row would be:
{{0, 1, 0}, {-1, 0, 1}, {0, -1, 0}}.

And a brain with the initial node leading into two networks and then those two node feeding back to a single node with their own nets would be:
{{0, 1, 1, 0}, {-1, 0, 0, 1}, {-1, 0, 0, 1}, {0, -1, -1, 0}}.

### Node sizes list

This list specifies the breadth of each of the top level nodes. This is really more so "how many nodes are there for each networks output and input?".

So for the first example in "The matrix" section, we might use {5, 7, 7, 5}. Keep in mind that there is a full network between each of these numbers. For the second one we might use {5, 4, 4, 5}. So the networks for this second example would be two which move from 5 nodes to 4, then another two which move from 4 nodes to 2 and 3.

This last part brings up an important point. When two networks feed into the same node, they add up. So if net 1 has an output of 2 slots and net 2 has an output of 3 slots, then the output node will have 5 slots.

### Layer sizes list

This list specifies the layer sizes of each individual network. This is the traditional layer sizes.

So a brain with only a single network would look something like {{10, 15, 15, 15, 5}} or something like that. If it had two network, it would be something like {{,,,},{,,,}} (with numbers filled in, obviously).

The ordering here is important. It is ordered according to a "top triangle positive matrix". This is the typical way of specifying the matrix from earlier where the top right triangle is all positive and the bottom left triangle is all negative. You then read the positive 1's left to right like you're reading and that is the order in which the networks are specified.

## Future work

Most immediately I hope to incorporate convolutional net support. Also significant testing is still needed to make sure it is working as intended.