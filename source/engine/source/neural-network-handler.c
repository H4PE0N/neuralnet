#include "../header/engine-include-header.h"

int maximum_layer_shape(const int layerSizes[], int layerAmount)
{
  int maxShape = 0;

  for(int lIndex = 0; lIndex < layerAmount; lIndex += 1)
  {
    if(layerSizes[lIndex] > maxShape) maxShape = layerSizes[lIndex];
  }
  return maxShape;
}

bool create_weibia_deltas(float*** weightDeltas, float** biasDeltas, int layerAmount, const int layerSizes[], float learnRate, float momentum, float*** weightDerivs, float** biasDerivs, float*** oldWeightDeltas, float** oldBiasDeltas)
{
  int maxShape = maximum_layer_shape(layerSizes, layerAmount);

  multi_scale_fmatarr(weightDeltas, weightDerivs, layerAmount - 1, maxShape, maxShape, -learnRate);

  if(oldWeightDeltas != NULL)
  {
    float*** tempWeightDeltas = create_fmatrix_array(layerAmount - 1, maxShape, maxShape);

    multi_scale_fmatarr(tempWeightDeltas, oldWeightDeltas, layerAmount - 1, maxShape, maxShape, momentum);

    addit_elem_fmatarr(weightDeltas, weightDeltas, tempWeightDeltas, layerAmount - 1, maxShape, maxShape);

    free_fmatrix_array(tempWeightDeltas, layerAmount - 1, maxShape, maxShape);
  }


  multi_scale_fmatrix(biasDeltas, biasDerivs, layerAmount - 1, maxShape, -learnRate);

  if(oldBiasDeltas != NULL)
  {
    float** tempBiasDeltas = create_float_matrix(layerAmount - 1, maxShape);

    multi_scale_fmatrix(tempBiasDeltas, oldBiasDeltas, layerAmount - 1, maxShape, momentum);

    addit_elem_fmatrix(biasDeltas, biasDeltas, tempBiasDeltas, layerAmount - 1, maxShape);

    free_float_matrix(tempBiasDeltas, layerAmount - 1, maxShape);
  }

  return true;
}

void multi_activ_derivs(float* layerDerivs, float* layerValues, int layerWidth, int layerActive)
{
  float* activDerivs = create_float_vector(layerWidth);

  float* (*layer_derivat_funct)(float*, float*, int);

  if(parse_derivat_funct(&layer_derivat_funct, layerActive))
  {
    layer_derivat_funct(activDerivs, layerValues, layerWidth);
  }
  else printf("Error: parse_derivat_funct\n");

  multi_elem_fvector(layerDerivs, layerDerivs, activDerivs, layerWidth);

  free_float_vector(activDerivs, layerWidth);
}

bool create_node_derivs(float** nodeDerivs, int layerAmount, const int layerSizes[], const int layerActivs[], float*** weights, float** biases, float** nodeValues, float* targets)
{
  cross_entropy_deriv(nodeDerivs[layerAmount - 2], nodeValues[layerAmount - 1], targets, layerSizes[layerAmount - 1]);

  // Note: When this is not here, the program preforms better for some reason!?
  multi_activ_derivs(nodeDerivs[layerAmount - 2], nodeValues[layerAmount - 1], layerSizes[layerAmount - 1], layerActivs[layerAmount - 1]);

  for(int layerIndex = (layerAmount - 1); layerIndex >= 2; layerIndex -= 1)
  {
    int layerHeight = layerSizes[layerIndex];
    int layerWidth = layerSizes[layerIndex - 1];
    
    float** weightTransp = create_float_matrix(layerWidth, layerHeight);

    transp_float_matrix(weightTransp, weights[layerIndex - 1], layerHeight, layerWidth);
    
    dotprod_fmatrix_vector(nodeDerivs[layerIndex - 2], weightTransp, layerWidth, layerHeight, nodeDerivs[layerIndex - 1], layerHeight);

    free_float_matrix(weightTransp, layerWidth, layerHeight);

    multi_activ_derivs(nodeDerivs[layerIndex - 2], nodeValues[layerIndex - 1], layerWidth, layerActivs[layerIndex - 1]);
  }
  return true;
}

bool create_weibia_derivs(float*** weightDerivs, float** biasDerivs, int layerAmount, const int layerSizes[], const int layerActivs[], float*** weights, float** biases, float** nodeValues, float* targets)
{
  int maxShape = maximum_layer_shape(layerSizes, layerAmount);

  float** nodeDerivs = create_float_matrix(layerAmount - 1, maxShape);

  create_node_derivs(nodeDerivs, layerAmount, layerSizes, layerActivs, weights, biases, nodeValues, targets);

  for(int layerIndex = (layerAmount - 1); layerIndex >= 1; layerIndex -= 1)
  {
    int layerHeight = layerSizes[layerIndex];
    int layerWidth = layerSizes[layerIndex - 1];

    dotprod_float_vector(weightDerivs[layerIndex - 1], nodeDerivs[layerIndex - 1], layerHeight, nodeValues[layerIndex - 1], layerWidth);
    
    copy_float_vector(biasDerivs[layerIndex - 1], nodeDerivs[layerIndex - 1], layerHeight);
  }

  free_float_matrix(nodeDerivs, layerAmount - 1, maxShape);

  return true;
}

bool create_node_values(float** nodeValues, int layerAmount, const int layerSizes[], const int layerActivs[], float*** weights, float** biases, float* inputs)
{
  copy_float_vector(nodeValues[0], inputs, layerSizes[0]);

  for(int layerIndex = 1; layerIndex < layerAmount; layerIndex += 1)
  {
    int layerHeight = layerSizes[layerIndex];
    int layerWidth = layerSizes[layerIndex - 1];

    dotprod_fmatrix_vector(nodeValues[layerIndex], weights[layerIndex - 1], layerHeight, layerWidth, nodeValues[layerIndex - 1], layerWidth);

    addit_elem_fvector(nodeValues[layerIndex], nodeValues[layerIndex], biases[layerIndex - 1], layerHeight);


    float* (*layer_activat_funct)(float*, float*, int);

    if(parse_activat_funct(&layer_activat_funct, layerActivs[layerIndex - 1]))
    {
      layer_activat_funct(nodeValues[layerIndex], nodeValues[layerIndex], layerHeight);
    }
    else printf("Error: parse_derivat_funct\n");
  }
  return true;
}