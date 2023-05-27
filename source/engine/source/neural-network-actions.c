#include "../header/engine-include-header.h"

bool train_network_stcast(int layerAmount, const int layerSizes[], const int layerActivs[], float*** weights, float** biases, float learnRate, float momentum, float* inputs, float* targets)
{
  int maxShape = maximum_layer_shape(layerSizes, layerAmount);

  float*** weightDeltas = create_fmatrix_array(layerAmount - 1, maxShape, maxShape);
  float** biasDeltas = create_float_matrix(layerAmount - 1, maxShape);

  stcast_weibia_deltas(weightDeltas, biasDeltas, layerAmount, layerSizes, layerActivs, weights, biases, learnRate, momentum, inputs, targets);

  addit_elem_fmatarr(weights, weights, weightDeltas, layerAmount - 1, maxShape, maxShape);
  addit_elem_fmatrix(biases, biases, biasDeltas, layerAmount - 1, maxShape);

  free_fmatrix_array(weightDeltas, layerAmount - 1, maxShape, maxShape);
  free_float_matrix(biasDeltas, layerAmount - 1, maxShape);

  return true;
}

void stcast_weibia_deltas(float*** weightDeltas, float** biasDeltas, int layerAmount, const int layerSizes[], const int layerActivs[], float*** weights, float** biases, float learnRate, float momentum, float* inputs, float* targets)
{
	int maxShape = maximum_layer_shape(layerSizes, layerAmount);

  float*** weightDerivs = create_fmatrix_array(layerAmount - 1, maxShape, maxShape);
  float** biasDerivs = create_float_matrix(layerAmount - 1, maxShape);

  frwrd_create_derivs(weightDerivs, biasDerivs, layerAmount, layerSizes, layerActivs, weights, biases, inputs, targets);

  create_weibia_deltas(weightDeltas, biasDeltas, layerAmount, layerSizes, learnRate, momentum, weightDerivs, biasDerivs, NULL, NULL);

  free_fmatrix_array(weightDerivs, layerAmount - 1, maxShape, maxShape);
  free_float_matrix(biasDerivs, layerAmount - 1, maxShape);
}

void frwrd_create_derivs(float*** weightDerivs, float** biasDerivs, int layerAmount, const int layerSizes[], const int layerActivs[], float*** weights, float** biases, float* inputs, float* targets)
{
	int maxShape = maximum_layer_shape(layerSizes, layerAmount);

	float** nodeValues = create_float_matrix(layerAmount, maxShape);

	create_node_values(nodeValues, layerAmount, layerSizes, layerActivs, weights, biases, inputs);

	create_weibia_derivs(weightDerivs, biasDerivs, layerAmount, layerSizes, layerActivs, weights, biases, nodeValues, targets);

	free_float_matrix(nodeValues, layerAmount, maxShape);
}

void mean_weibia_derivs(float*** meanWeightDerivs, float** meanBiasDerivs, int layerAmount, const int layerSizes[], const int layerActivs[], float*** weights, float** biases, float** inputs, float** targets, int batchSize)
{
	int maxShape = maximum_layer_shape(layerSizes, layerAmount);

	float*** weightDerivs = create_fmatrix_array(layerAmount - 1, maxShape, maxShape);
	float** biasDerivs = create_float_matrix(layerAmount - 1, maxShape);

	for(int inputIndex = 0; inputIndex < batchSize; inputIndex += 1)
	{
		frwrd_create_derivs(weightDerivs, biasDerivs, layerAmount, layerSizes, layerActivs, weights, biases, inputs[inputIndex], targets[inputIndex]);

		meanWeightDerivs = addit_elem_fmatarr(meanWeightDerivs, meanWeightDerivs, weightDerivs, layerAmount - 1, maxShape, maxShape);
		meanBiasDerivs = addit_elem_fmatrix(meanBiasDerivs, meanBiasDerivs, biasDerivs, layerAmount - 1, maxShape);
	}

	free_fmatrix_array(weightDerivs, layerAmount - 1, maxShape, maxShape);
	free_float_matrix(biasDerivs, layerAmount - 1, maxShape);

	meanWeightDerivs = multi_scale_fmatarr(meanWeightDerivs, meanWeightDerivs, layerAmount - 1, maxShape, maxShape, 1.0f / batchSize);
  	meanBiasDerivs = multi_scale_fmatrix(meanBiasDerivs, meanBiasDerivs, layerAmount - 1, maxShape, 1.0f / batchSize);
}

void minbat_weibia_deltas(float*** weightDeltas, float** biasDeltas, int layerAmount, const int layerSizes[], const int layerActivs[], float*** weights, float** biases, float learnRate, float momentum, float** inputs, float** targets, int batchSize)
{
	int maxShape = maximum_layer_shape(layerSizes, layerAmount);

	float*** meanWeightDerivs = create_fmatrix_array(layerAmount - 1, maxShape, maxShape);
	float** meanBiasDerivs = create_float_matrix(layerAmount - 1, maxShape);

	mean_weibia_derivs(meanWeightDerivs, meanBiasDerivs, layerAmount, layerSizes, layerActivs, weights, biases, inputs, targets, batchSize);

	create_weibia_deltas(weightDeltas, biasDeltas, layerAmount, layerSizes, learnRate, momentum, meanWeightDerivs, meanBiasDerivs, NULL, NULL);

	free_fmatrix_array(meanWeightDerivs, layerAmount - 1, maxShape, maxShape);
	free_float_matrix(meanBiasDerivs, layerAmount - 1, maxShape);
}

bool train_network_minbat(int layerAmount, const int layerSizes[], const int layerActivs[], float*** weights, float** biases, float learnRate, float momentum, float** inputs, float** targets, int batchSize)
{
  int maxShape = maximum_layer_shape(layerSizes, layerAmount);

  float*** weightDeltas = create_fmatrix_array(layerAmount - 1, maxShape, maxShape);
  float** biasDeltas = create_float_matrix(layerAmount - 1, maxShape);

  minbat_weibia_deltas(weightDeltas, biasDeltas, layerAmount, layerSizes, layerActivs, weights, biases, learnRate, momentum, inputs, targets, batchSize);

  weights = addit_elem_fmatarr(weights, weights, weightDeltas, layerAmount - 1, maxShape, maxShape);
  biases = addit_elem_fmatrix(biases, biases, biasDeltas, layerAmount - 1, maxShape);

  free_fmatrix_array(weightDeltas, layerAmount - 1, maxShape, maxShape);
  free_float_matrix(biasDeltas, layerAmount - 1, maxShape);

  return true;
}

bool frwrd_network_inputs(float* outputs, int layerAmount, const int layerSizes[], const int layerActivs[], float*** weights, float** biases, float* inputs)
{
  int maxShape = maximum_layer_shape(layerSizes, layerAmount);

  float** nodeValues = create_float_matrix(layerAmount, maxShape);

	create_node_values(nodeValues, layerAmount, layerSizes, layerActivs, weights, biases, inputs);

	outputs = copy_float_vector(outputs, nodeValues[layerAmount - 1], layerSizes[layerAmount - 1]);

	free_float_matrix(nodeValues, layerAmount, maxShape);

  return true;
}