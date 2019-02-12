//
// base NN classes
//

//
// system includes
//
#include <assert.h>
#include <numeric>
#include <sstream>

//
// includes
//
#include "BaseNN.H"
#include "StdStringUtils.H"
#include "RegexMatch.H"

//
// NN topology for NN
//
NNTopology::NNTopology() : _number_of_layers(0),
                           _number_of_neurons(0),
                           _neurons_per_layer(),
                           _weights(),
                           _offsets()
{
}

NNTopology::NNTopology(long number_of_layers, 
                       const NeuronsPerLayer &neurons_per_layer,
                       const Weights &weights) :
                           _number_of_layers(number_of_layers),
                           _number_of_neurons(0),
                           _neurons_per_layer(),
                           _weights(),
                           _offsets()
{
    _neurons_per_layer.insert(_neurons_per_layer.begin(), 
                              neurons_per_layer.begin(),
                              neurons_per_layer.end());

    _weights.insert(_weights.begin(), 
                    weights.begin(),
                    weights.end());
    
    _number_of_neurons = accumulate(_neurons_per_layer.begin(),
                                    _neurons_per_layer.end(), 0);

    long total = 0;
    _offsets.push_back(total);

    NeuronsPerLayerCIt it    = _neurons_per_layer.begin();
    NeuronsPerLayerCIt itend = _neurons_per_layer.end();
    for ( ; it!=itend; ++it)
    {
        total += *it;
        total += 1; // one bias weight per layer
        _offsets.push_back(total);
    }
}
                       
NNTopology::NNTopology(const NNTopology &src) :
                           _number_of_layers(src._number_of_layers),
                           _number_of_neurons(src._number_of_neurons),
                           _neurons_per_layer(),
                           _weights(),
                           _offsets()
{
    _neurons_per_layer.insert(_neurons_per_layer.begin(), 
                              src._neurons_per_layer.begin(),
                              src._neurons_per_layer.end());

    _weights.insert(_weights.begin(), 
                    src._weights.begin(),
                    src._weights.end());

    _offsets.insert(_offsets.begin(), 
                    src._offsets.begin(),
                    src._offsets.end());
}

void
NNTopology::load_from_file(const string &file_path)
{
    static PRegexMatch re_comment("^[ \t]*#");
    static PRegexMatch re_ws_only("^[ \t\r\n]*$");

    _neurons_per_layer.clear();
    _weights.clear();
    _offsets.clear();

    string contents;
    read_file(file_path, contents);

    vector<string> lines;
    split(contents, lines, "\r\n", true);
    assert(lines.size() > 0);

    vector<string>::iterator it    = lines.begin();
    vector<string>::iterator itend = lines.end();

    States state = ReadNumberOfLayers;
    long neurons_per_layer_to_read = 0;
    long weights_to_read = 0;

    for ( ; (it!=itend) && (state!=End); ++it)
    {
        if (re_comment(it->c_str()) || re_ws_only(it->c_str()))
        {
            continue; // skip line
        }
        string value(trim(*it));

        switch (state)
        {
        case ReadNumberOfLayers:
        {
            istringstream(value) >> _number_of_layers;
            assert(_number_of_layers > 0);

            state = ReadNumberOfNeuronsPerLayer;
            neurons_per_layer_to_read = _number_of_layers;
            continue;
        }
        case ReadNumberOfNeuronsPerLayer:
        {
            long number_of_neurons = 0;
            istringstream(value) >> number_of_neurons;

            assert(number_of_neurons > 0);
            _neurons_per_layer.push_back(number_of_neurons);

            if (--neurons_per_layer_to_read <= 0)
            {
                weights_to_read = 0;

                NeuronsPerLayerCIt nit    = _neurons_per_layer.begin();
                NeuronsPerLayerCIt nitend = _neurons_per_layer.end();
                for (long neurons = *nit++; nit!=nitend; ++nit)
                {
                    weights_to_read += (neurons+1)*(*nit);
                    neurons = *nit;
                }

                state = ReadWeightsPerLayer;
            }
            continue;
        }
        case ReadWeightsPerLayer:
        {
            double weight = 0.0;
            istringstream(value) >> weight;
            _weights.push_back(weight);

            if (--weights_to_read <= 0)
            {
                state = End;
            }
            continue;
        }
        case End:
        {
            continue;
        }
        default:
        {
            assert(0);
        }
        }
    }

    _number_of_neurons = accumulate(_neurons_per_layer.begin(),
                                    _neurons_per_layer.end(), 0);

    long total = 0;
    _offsets.push_back(total);

    NeuronsPerLayerCIt nit    = _neurons_per_layer.begin();
    NeuronsPerLayerCIt nitend = _neurons_per_layer.end();
    for ( ; nit!=nitend; ++nit)
    {
        total += *nit;
        total += 1; // one bias weight per layer
        _offsets.push_back(total);
    }
}

NNTopology::~NNTopology()
{
}

NNTopology &
NNTopology::operator=(const NNTopology &rhs)
{
    if (this != &rhs)
    {
        _number_of_layers = rhs._number_of_layers;
        _number_of_neurons = rhs._number_of_neurons;

        _neurons_per_layer.clear();
        _weights.clear();
        _offsets.clear();

        _neurons_per_layer.insert(_neurons_per_layer.begin(), 
                                  rhs._neurons_per_layer.begin(),
                                  rhs._neurons_per_layer.end());

        _weights.insert(_weights.begin(), 
                        rhs._weights.begin(),
                        rhs._weights.end());

        _offsets.insert(_offsets.begin(), 
                        rhs._offsets.begin(),
                        rhs._offsets.end());
    }

    return *this;
}

//
// base neural net neuron
//
NNNeuron::NNNeuron() :
    _weights()
{
}

NNNeuron::NNNeuron(const NNNeuron &src) :
    _weights()
{
    _weights.insert(_weights.begin(), 
                    src._weights.begin(), 
                    src._weights.end());
}

NNNeuron::~NNNeuron()
{
    _weights.clear();
}

NNNeuron &
NNNeuron::operator=(const NNNeuron &rhs)
{
    if (this != &rhs)
    {
        _weights.clear();
        _weights.insert(_weights.begin(), 
                        rhs._weights.begin(), 
                        rhs._weights.end());
    }
    return *this;
}

//
// base neural net layer class
//
NNLayer::NNLayer() :
    _neurons()
{
}

NNLayer::NNLayer(const NNLayer &src) :
    _neurons()
{
    _neurons.insert(_neurons.begin(), 
                    src._neurons.begin(), 
                    src._neurons.end());
}

NNLayer::~NNLayer()
{
    _neurons.clear();
}

NNLayer &
NNLayer::operator=(const NNLayer &rhs)
{
    if (this != &rhs)
    {
        _neurons.clear();
        _neurons.insert(_neurons.begin(), 
                        rhs._neurons.begin(), 
                        rhs._neurons.end());
    }
    return *this;
}

//
// base neural net class
//
NeuralNet::NeuralNet(const NNTopology &topology) :
    _topology(topology),
    _layers()
{
}

NeuralNet::NeuralNet(const NeuralNet &src) :
    _topology(src._topology)
{
    _layers.insert(_layers.begin(), 
                    src._layers.begin(), 
                    src._layers.end());
}

NeuralNet::~NeuralNet()
{
    _layers.clear();
}

NeuralNet &
NeuralNet::operator=(const NeuralNet &rhs)
{
    if (this != &rhs)
    {
        _layers.clear();
        _layers.insert(_layers.begin(), 
                       rhs._layers.begin(), 
                       rhs._layers.end());
    }
    return *this;
}

