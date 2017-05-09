#ifndef BACKPROP_H
#define BACKPROP_H


# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>

typedef struct BPNN BPNN;
struct BPNN
{
	int nb_hidden;					//number of sigmoid in hidden layer
	int nb_inputs;					//784  data image dimension 24 * 24
	int nb_outputs;					//0 to 9 -> 10 outputs

	double *inputs_vect;			//inputs
	double *outputs_vect;			//outputs pre-set
	double *hidden_layer;			//hidden layer

	double *hidden_delta;			//storrage for hidden value error
	double *output_delta;			//storage for output value error

	double **input_weight_mat;		//weights from input to hidden layer
	double **hidden_weight_mat;		//weights from hidden layer to output

	double **input_prev_weights;	//previous change on input to hidden weights
	double **hidden_prev_weights;	//previous change on hidden to output weights

	double *target;					//?????????????????
	//float learning_rate = 0.01;
	//ERRORS
};

/* Functions of BPNN */
BPNN *bpnn_create(int nb_inputs, int nd_hidden, int nb_outputs);
void bpnn_free(BPNN *net);

void bpnn_train(BPNN *net, double eta, double momentum, double *eo, double *eh);
void bpnn_feedforward(BPNN *net);

void bpnn_save(BPNN *net, char *filename);
BPNN *bpnn_read(char *filename);
  
#endif
