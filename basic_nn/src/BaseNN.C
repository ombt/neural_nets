//
// base NN classes
//

//
// system includes
//
#include <assert.h>
#include <numeric>
#include <iostream>
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

    for (long iweight=1 ; (it!=itend) && (state!=End); ++it)
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
                for (long layer=1, neurons = *nit++; nit!=nitend; ++nit, ++layer)
                {
                    long weights_in_layer = (neurons+1)*(*nit);
#ifdef LOCAL_DEBUG
                    cout << "Layer: " << layer 
                         << ", Neurons in Layer + Bias: " << (neurons+1)
                         << ", Neurons in Next Layer: " << *nit
                         << ", Weights in Layer: " << weights_in_layer 
                         << endl;
#endif
                    weights_to_read += weights_in_layer;
                    neurons = *nit;
                }
#ifdef LOCAL_DEBUG
                cout << "Weights to Read: " << weights_to_read << endl;
#endif

                state = ReadWeightsPerLayer;
            }
            continue;
        }
        case ReadWeightsPerLayer:
        {
            double weight = 0.0;
            istringstream(value) >> weight;
            _weights.push_back(weight);

#ifdef LOCAL_DEBUG
            cout << "No: " << iweight++ << ", Weight: " << weight << endl;
#endif

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
    for (long previous=*nit++ ; nit!=nitend; ++nit)
    {
        total += (previous+1)*(*nit); // one bias weight per layer
        _offsets.push_back(total);
        previous=*nit;
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

void 
NNNeuron::append(double weight)
{
    _weights.push_back(weight);
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

void
NNLayer::append(NNNeuron *pnnn)
{
    _neurons.push_back(ExtUseCntPtr<NNNeuron>(pnnn));
}

//
// base neural net class
//
NeuralNet::NeuralNet(const NNTopology &topology) :
    _topology(topology),
    _layers()
{
    load_topology();
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

void
NeuralNet::append(NNLayer *pnnl)
{
    _layers.push_back(ExtUseCntPtr<NNLayer>(pnnl));
}

void
NeuralNet::load_topology()
{
    long max_layers = _topology.number_of_layers();
    for (long layer=0; layer<max_layers; ++layer)
    {
        long neurons_in_layer = _topology.neurons_per_layer()[layer];
        cout << "Layer: " << (layer+1) 
             << ", Neurons: " << neurons_in_layer 
             << endl;

        //
        // new layer
        //
        NNLayer *pl = new NNLayer;

        long next_layer = layer + 1;
        if (next_layer < max_layers)
        {
            //
            // get weights for this layer connecting to next layer
            //
            long neurons_in_next_layer = _topology.neurons_per_layer()[next_layer];
            for (long nil=0; nil<=neurons_in_layer; ++nil)
            {
                //
                // new neuron
                //
                NNNeuron *pn = new NNNeuron;

                for (long ninl=0; ninl<neurons_in_next_layer; ++ninl)
                {
                    double weight = _topology.weight(layer, nil, ninl);
                    if (nil>0)
                    {
                        cout << "Layer: " << (layer+1)
                             << ", This Layer Neuron: " << nil
                             << ", Next Layer Neuron: " << (ninl+1)
                             << ", Weight: " << weight
                             << endl;
                    }
                    else
                    {
                        cout << "Layer: " << (layer+1)
                             << ", This Layer Bias: " << 1
                             << ", Next Layer Neuron: " << (ninl+1)
                             << ", Weight: " << weight
                             << endl;
                    }

                    //
                    // append weight to this neuron
                    //
                    pn->append(weight);
                }

                //
                // append neuron to this layer
                //
                pl->append(pn);
            }
        }
        else
        {
            //
            // output layer - no weights
            //
            for (long nil=0; nil<=neurons_in_layer; ++nil)
            {
                //
                // new neuron
                //
                NNNeuron *pn = new NNNeuron;

                //
                // append neuron to this layer
                //
                pl->append(pn);
            }
        }

        //
        // append layer to this NN
        //
        append(pl);
    }
}

//
// I/O operators
//
ostream &
operator<<(ostream &os, const NNTopology &src)
{
    os << endl << "Number of Layers: " << src._number_of_layers << endl;
    os << endl << "Number of Neurons: " << src._number_of_neurons << endl;

    os << endl << "Neurons per Layer: " << endl;
    NeuronsPerLayerCIt npl_it    = src._neurons_per_layer.begin();
    NeuronsPerLayerCIt npl_itend = src._neurons_per_layer.end();
    for (int layer=1 ; npl_it!=npl_itend; ++npl_it, ++layer)
    {
        os << "Layer: " << layer << ", Number of Neurons: " << *npl_it << endl;
    }

    os << endl << "Weights: " << endl;
    WeightsCIt w_it    = src._weights.begin();
    WeightsCIt w_itend = src._weights.end();
    for ( ; w_it!=w_itend; ++w_it)
    {
        os << "Weight: " << *w_it << endl;
    }

    os << endl << "Layer Offsets: " << endl;
    LayerOffsetsCIt lo_it    = src._offsets.begin();
    LayerOffsetsCIt lo_itend = src._offsets.end();
    for (int layer=1 ; lo_it!=lo_itend; ++layer, ++lo_it)
    {
        os << "Layer: " << layer << ", Layer Offset: " << *lo_it << endl;
    }

    return os;
}

void
NeuralNet::apply(const NNVector &inv, NNVector &outv) const
{
}

ostream &
operator<<(ostream &os, const NNNeuron &src)
{
    return os;
}

ostream &
operator<<(ostream &os, const NNLayer &src)
{
    return os;
}

ostream &
operator<<(ostream &os, const NeuralNet &src)
{
    return os;
}

