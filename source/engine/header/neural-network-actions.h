#ifndef NEURAL_NETWORK_ACTIONS_H
#define NEURAL_NETWORK_ACTIONS_H

bool train_network_stcast(int layerAmount, const int layerSizes[], const int layerActivs[], float*** weights, float** biases, float learnRate, float momentum, float* inputs, float* targets);

void stcast_weibia_deltas(float*** weightDeltas, float** biasDeltas, int layerAmount, const int layerSizes[], const int layerActivs[], float*** weights, float** biases, float learnRate, float momentum, float* inputs, float* targets);

void frwrd_create_derivs(float*** weightDerivs, float** biasDerivs, int layerAmount, const int layerSizes[], const int layerActivs[], float*** weights, float** biases, float* inputs, float* targets);

void mean_weibia_derivs(float*** meanWeightDerivs, float** meanBiasDerivs, int layerAmount, const int layerSizes[], const int layerActivs[], float*** weights, float** biases, float** inputs, float** targets, int batchSize);

void minbat_weibia_deltas(float*** weightDeltas, float** biasDeltas, int layerAmount, const int layerSizes[], const int layerActivs[], float*** weights, float** biases, float learnRate, float momentum, float** inputs, float** targets, int batchSize);

bool train_network_minbat(int layerAmount, const int layerSizes[], const int layerActivs[], float*** weights, float** biases, float learnRate, float momentum, float** inputs, float** targets, int batchSize);

bool frwrd_network_inputs(float* outputs, int layerAmount, const int layerSizes[], const int layerActivs[], float*** weights, float** biases, float* inputs);

#endif