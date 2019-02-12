//
// test for neural net classes
//

// system headers
#include <assert.h>
#include <string>

// local headers
#include "BaseNN.H"

int
main(int argc, char **argv)
{
    assert(argc == 2);
    string topology_file_path(argv[1]);

    NNTopology topology;
    topology.load_from_file(topology_file_path);

    cout << "Topology: " << endl;
    cout << topology << endl;

    cout << "Neural Net: " << endl;
    NeuralNet nn(topology);

    for (long max_layers = nn.number_of_layers(), layer = 0;
         layer < max_layers;
         ++layer)
    {
        cout << "Layer: " << layer << endl
             << nn[layer] << endl;
    }

    return 0;
}
