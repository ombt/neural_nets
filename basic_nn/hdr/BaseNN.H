#ifndef __BASE_NN_H
#define __BASE_NN_H

//
// system includes
//
#include <vector>
#include <string>
#include <iostream>

//
// local includes
//
#include "ExtUseCntPtr.H"

using namespace std;

//
// NN-related types
//
typedef double Weight;

typedef vector<Weight> Weights;
typedef Weights::iterator WeightsIt;
typedef Weights::const_iterator WeightsCIt;

typedef vector<long> NeuronsPerLayer;
typedef NeuronsPerLayer::iterator NeuronsPerLayerIt;
typedef NeuronsPerLayer::const_iterator NeuronsPerLayerCIt;

typedef vector<long> LayerOffsets;
typedef LayerOffsets::iterator LayerOffsetsIt;
typedef LayerOffsets::const_iterator LayerOffsetsCIt;

//
// base neural net classes
//
class NNTopology
{
protected: // member data types
    enum States
    {
        ReadNumberOfLayers = 1,
        ReadNumberOfNeuronsPerLayer,
        ReadWeightsPerLayer,
        End,
    } ;

public: // member functions
    //
    // ctors and dtor
    //
    NNTopology();
    NNTopology(long number_of_layers, 
               const NeuronsPerLayer &neurons_per_layer, // not including bias
               const Weights &weights);
    NNTopology(const NNTopology &src);
    virtual ~NNTopology();

    //
    // load_from_file - read topology file
    //
    void load_from_file(const string &file_path);

    //
    //
    // assignment operator
    //
    NNTopology &operator=(const NNTopology &rhs);

    //
    // accessors
    //
    inline long number_of_layers() const
    {
        return _number_of_layers;
    }
    inline const NeuronsPerLayer &neurons_per_layer() const
    {
        return _neurons_per_layer;
    }
    inline const Weights &weights() const
    {
        return _weights;
    }
    inline const Weight &weight(long layer, long n, long next_n)
    {
        return _weights[_offsets[layer] +
                        n * _neurons_per_layer[layer + 1] +
                        next_n];
    }

    //
    // i/o operator
    //
    friend ostream &operator<<(ostream &os, const NNTopology &src);

protected: // member data
    long _number_of_layers;
    long _number_of_neurons;
    NeuronsPerLayer _neurons_per_layer;
    Weights _weights;
    LayerOffsets _offsets;
};

class NNNeuron
{
public: // member functions
    //
    // ctors and dtor
    //
    NNNeuron();
    NNNeuron(const NNNeuron &src);
    virtual ~NNNeuron();

    //
    // assignment operator
    //
    NNNeuron &operator=(const NNNeuron &rhs);

    //
    // weight - access a weight
    //
    inline Weight &weight(long neuron)
    {
        return _weights[neuron];
    }

    //
    // i/o operator
    //
    friend ostream &operator<<(ostream &os, const NNNeuron &src);

protected: // member data
    Weights _weights;
};

class NNLayer
{
public: // member functions
    //
    // ctors and dtor
    //
    NNLayer();
    NNLayer(const NNLayer &src);
    virtual ~NNLayer();

    //
    // assignment operator
    //
    NNLayer &operator=(const NNLayer &rhs);

    //
    // neuron - access NN in a layer
    //
    inline NNNeuron &operator[](long neuron)
    {
        return *_neurons[neuron];
    }
    inline NNNeuron &neuron(long neuron)
    {
        return *_neurons[neuron];
    }

    //
    // i/o operator
    //
    friend ostream &operator<<(ostream &os, const NNLayer &src);

protected: // member data
    vector<ExtUseCntPtr<NNNeuron> > _neurons;
};

class NeuralNet
{
public: // member functions
    //
    // ctors and dtor
    //
    NeuralNet(const NNTopology &topology);
    NeuralNet(const NeuralNet &src);
    virtual ~NeuralNet();

    //
    // assignment operator
    //
    NeuralNet &operator=(const NeuralNet &rhs);

    //
    // neuron - access NN in a layer
    // layer - access a layer
    //
    inline NNNeuron &neuron(long layer, long neuron)
    {
        return (*_layers[layer])[neuron];
    }
    inline NNLayer &layer(long layer)
    {
        return *_layers[layer];
    }
    inline NNLayer &operator[](long layer)
    {
        return *_layers[layer];
    }

    //
    // i/o operator
    //
    friend ostream &operator<<(ostream &os, const NeuralNet &src);

private: // not allowed
    NeuralNet();

protected: // member data
    NNTopology _topology;
    vector<ExtUseCntPtr<NNLayer> > _layers;
};

#endif

