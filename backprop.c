/*________________________________________*
  |	
  |	* File Name      : backprop.c
  |	* Creation Date  : 14-10-2016
  |  * Last Modified : Sam 20 mai 18:21:19 2017
  |	* Created by     :	    Claire
 *________________________________________**/

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <fcntl.h>
# include <unistd.h>

# include "backprop.h"

#define fastcopy(to,from,len)\
{\
	register char *_to,*_from;\
	register int _i,_l;\
	_to = (char *)(to);\
	_from = (char *)(from);\
	_l = (len);\
	for (_i = 0; _i < _l; _i++) *_to++ = *_from++;\
}

/* Return a random number between 0.0 and 1.0 */
double double_rand()
{
	return ((double) random()/(double)RAND_MAX);
}

/* Return a random number between -1.0 and 1.0 */
double double_rand_abs1()
{
	return ((double_rand() * 2.0) - 1.0);
}

/* Sigmoid function */
double sigmoid_func(double z)
{
	return (1.0 / (1.0 + exp(-z)));
}

/* Allocate a one dimension array of doubles ***/
double 		*alloc_1dim_dbl(int size)
{
	double 	*new;

	new = (double *) malloc ((unsigned) (size * sizeof (double)));
	if (new == NULL)
	{
		printf("ALLOC_1DIM_DBL: Couldn't allocate array of doubles\n");
		return (NULL);
	}
	return (new);
}

/* Allocate a two dimension array of doubles */
double 		**alloc_2dim_dbl(int size_c, int size_r)
{
	double 	**new;
	new = (double **) malloc ((unsigned) (size_c * sizeof (double *)));
	if (new == NULL)
	{
		printf("ALLOC_2DIM_DBL : Couldn't allocate array of doubles\n");
		return (NULL);
	}
	for(int c = 0; c < size_c; c++)
	{
		new[c] = alloc_1dim_dbl(size_r);
	}
	return (new);
}

/* Fill weight matrix with random number between -1.0 and 1.0*/
void bpnn_randomize_weights(double **w, int size_c, int size_r)
{
	for (int c = 0; c <= size_c; c++)
	{
		for (int r = 0; r <= size_r; r++)
		{
			w[c][r] = double_rand_abs1();
		}
	}
}

/*Fill weight matrix with 0.0*/
void bpnn_zero_weights(double **w, int size_c, int size_r)
{
	for (int c = 0; c <= size_c; c++)
	{
		for (int r = 0; r <= size_r; r++)
		{
			w[c][r] = 0.0;
		}
	}
}

BPNN 		*bpnn_internal_create(int nb_inputs, int nb_hidden, int nb_outputs)
{
	BPNN 	*newnet;

	newnet = (BPNN*) malloc (sizeof(BPNN));
	if (newnet == NULL)
	{
		printf("BPNN_CREATE: Couldn't allocate neural network\n");
		return (NULL);
	}
	newnet->nb_inputs = nb_inputs;
	newnet->nb_outputs = nb_outputs;
	newnet->nb_hidden = nb_hidden;

	newnet->inputs_vect = alloc_1dim_dbl(nb_inputs + 1);
	newnet->outputs_vect = alloc_1dim_dbl(nb_outputs + 1);
	newnet->hidden_layer = alloc_1dim_dbl(nb_hidden + 1);

	newnet->hidden_delta = alloc_1dim_dbl(nb_hidden + 1);
	newnet->output_delta = alloc_1dim_dbl(nb_outputs + 1);

	newnet->input_weight_mat = alloc_2dim_dbl(nb_inputs + 1, nb_hidden + 1);
	newnet->hidden_weight_mat = alloc_2dim_dbl(nb_hidden + 1, nb_outputs +1);

	newnet->input_prev_weights = alloc_2dim_dbl(nb_inputs +1, nb_hidden + 1);
	newnet->hidden_prev_weights = alloc_2dim_dbl(nb_hidden + 1, nb_outputs +1);

	newnet->target = alloc_1dim_dbl(nb_outputs +1);

	return (newnet);
}


void 	bpnn_free(BPNN *net)
{
	int n1;
	int n2;
	int i;

	n1 = net->nb_inputs;
	n2 = net->nb_hidden;

	free((char *) net->inputs_vect);
	free((char *) net->hidden_layer);
	free((char *) net->outputs_vect);

	free((char *) net->hidden_delta);
	free((char *) net->output_delta);
	free((char *) net->target);

	for (i = 0; i <= n1; i++)
	{
		free((char *) net->input_weight_mat[i]);
		free((char *) net->input_prev_weights[i]);
	}
	free((char *) net->input_weight_mat);
	free((char *) net->input_prev_weights);

	for (i = 0; i <= n2; i++) {
		free((char *) net->hidden_weight_mat[i]);
		free((char *) net->hidden_prev_weights[i]);
	}
	free((char *) net->hidden_weight_mat);
	free((char *) net->hidden_prev_weights);

	free((char *) net);
}


BPNN 		*bpnn_create(int nb_inputs, int nd_hidden, int nb_outputs)
{

	BPNN 	*newnet;

	newnet = bpnn_internal_create(nb_inputs, nd_hidden, nb_outputs);
	//#ifdef INITZERO
	//	bpnn_zero_weights(newnet->input_weight_mat, nb_inputs, nd_hidden);
	//#else
	bpnn_randomize_weights(newnet->input_weight_mat, nb_inputs, nd_hidden);
	//#endif
	bpnn_randomize_weights(newnet->hidden_weight_mat, nd_hidden, nb_outputs);
	bpnn_zero_weights(newnet->input_prev_weights, nb_inputs, nd_hidden);
	bpnn_zero_weights(newnet->hidden_prev_weights, nd_hidden, nb_outputs);

	return (newnet);
}

/*Compute the weighted sum then the sigmoid func on it*/
/* between a first layer (l1) and a second layer (l2)*/
void 		bpnn_layerforward(double *l1, double *l2, double **weight_mat, int size_l1, int size_l2)
{
	double 	sum;

	l1[size_l1] = 1.0;//treshold value used to be + 1
	for (int i = 0; i < size_l2; i++)
	{
		sum = 0.0;
		for (int j = 0; j <= size_l1; j++) //used to be <=
		{
			sum += weight_mat[j][i] * l1[j];
		}
		l2[i] = sigmoid_func(sum);
	}
}

/*void 		bpnn_softmax_output(double *output, double **weight_mat, int size_out, int size_hid)
{
	double 	sum;

	sum = 0.0;
	for (int i = 0; i < size_out; i++)
	{
		for (int j = 0; j <= size_hid; j++) //used to be <=
		{
			sum += exp(weight_mat[j][i]);
		}
		output[i] = (exp(output[i] + 1.0) / sum);
		printf(" outputs value during  backprop%f\n", output[i]);
	}
}*/
void 		bpnn_output_error(double *delta, double *target, double *output, int size_output, double *error)
{
	double 	o;
	double 	t;
	double 	errsum;

	errsum = 0.0;
	for (int i = 0; i < size_output; i++)
	{
		o = output[i];
		t = target[i];
		delta[i] = o * (1.0 - o) * (t - o);
		errsum += fabs(delta[i]);
	}
	*error = errsum;
}

/* who = weigth from hidden to output*/
void 		bpnn_hidden_error(double *delta_h, int size_hidden, double *delta_o, int size_output, double **who, double *hidden, double *error)
{
	double 	h;
	double 	sum;
	double 	errsum;

	errsum = 0.0;
	for (int i = 0; i < size_hidden; i++)
	{
		h = hidden[i];
		sum = 0.0;
		for (int j = 0; j < size_output; j++)
		{
			sum += delta_o[j] * who[i][j];
		}
		delta_h[i] = h * (1.0 - h) * sum;
		errsum += fabs(delta_h[i]);
	}
	*error = errsum;
}

void 		bpnn_adjust_weights(double *delta, int ndelta, double *lj, int nlj, double **w, double **oldw, double eta, double momentum)
{
	double 	new_dw;

	lj[nlj] = 1.0; //use to be + 1
	for (int i = 0; i < ndelta; i++)
	{
		for (int j = 0; j < nlj; j++)
		{
			new_dw = ((eta * delta[i] * lj[j]) + (momentum * oldw[j][i]));
			w[j][i] += new_dw;
			oldw[j][i] = new_dw;
		}
	}
}

void 	bpnn_feedforward(BPNN *net)
{
	int in;
	int hid;
	int out;

	in = net->nb_inputs;
	hid = net->nb_hidden;
	out = net->nb_outputs;

	/* Feed forward input activation */
	bpnn_layerforward(net->inputs_vect, net->hidden_layer, net->input_weight_mat, in, hid);
	//bpnn_softmax_output(net->outputs_vect, net->hidden_weight_mat, out, hid);
	bpnn_layerforward(net->hidden_layer, net->outputs_vect, net->hidden_weight_mat, hid, out);

}

void 		bpnn_train(BPNN *net, double eta, double momentum, double *eo, double *eh)
{
	int 	in;
	int 	hid;
	int 	out;
	double 	out_err;
	double 	hid_err;

	in = net->nb_inputs;
	hid = net->nb_hidden;
	out = net->nb_outputs;

	bpnn_feedforward(net);

	bpnn_output_error(net->output_delta, net->target, net->outputs_vect, out, &out_err);
	bpnn_hidden_error(net->hidden_delta, hid, net->output_delta, out, net->hidden_weight_mat, net->hidden_layer, &hid_err);

	*eo = out_err;
	*eh = hid_err;

	bpnn_adjust_weights(net->output_delta, out, net->hidden_layer, hid, net->hidden_weight_mat, net->hidden_prev_weights, eta, momentum);
	bpnn_adjust_weights(net->hidden_delta, hid, net->inputs_vect, in, net->input_weight_mat, net->input_prev_weights, eta, momentum);

}

void 		bpnn_save(BPNN *net, char *filename)
{
	int 	fd, n1, n2, n3, memcnt;
	double 	dvalue;
	double 	**w;
	char 	*mem;

	memcnt = 0;
	fd = creat(filename, 0644);
	if (fd == -1)
	{
		printf("BPNN_SAVE : Connot creat '%s'\n", filename);
		return;
	}
	n1 	= net->nb_inputs;
	n2 	= net->nb_hidden;
	n3 	= net->nb_outputs;

	printf("Saving %dx%dx%d neural net to %s \n", n1, n2, n3, filename);
	fflush(stdout);

	write(fd, (char *) &n1, sizeof(int));
	write(fd, (char *) &n2, sizeof(int));
	write(fd, (char *) &n3, sizeof(int));

	w = net->input_weight_mat;
	mem = (char *) malloc ((n1 + 1) * (n2 + 1) * sizeof(double));
	for (int i = 0; i  <= n1 ; i++)
	{
		for (int j = 0; j <= n2 ; j++)
		{
			dvalue = w[i][j];
			fastcopy(&mem[memcnt], &dvalue, sizeof(double));
			memcnt += sizeof(double);
		}
	}
	write(fd, mem, (n1 +1) * (n2 + 1) * sizeof(double));
	free(mem);

	memcnt = 0;
	w = net->hidden_weight_mat;

	mem = (char *) malloc ((unsigned) ((n2+1) * (n3+1) * sizeof(double)));
	for (int i = 0; i <= n2; i++) 
	{
		for (int j = 0; j <= n3; j++)
		{
			dvalue = w[i][j];
			fastcopy(&mem[memcnt], &dvalue, sizeof(double));
			memcnt += sizeof(double);

		}

	}
	write(fd, mem, (n2+1) * (n3+1) * sizeof(double));
	close(fd);
	return;
}

BPNN 	*bpnn_read(char *filename)
{
	char 	*mem;
	BPNN 	*new;
	int 	fd, n1, n2, n3, memcnt;

	if ((fd = open(filename, 0, 0644)) == -1) 
	{
		return (NULL);
	}

	//printf("Reading '%s'\n", filename);
	fflush(stdout);

	read(fd, (char *) &n1, sizeof(int));
	read(fd, (char *) &n2, sizeof(int));
	read(fd, (char *) &n3, sizeof(int));
	new = bpnn_internal_create(n1, n2, n3);

	//printf("'%s' contains a %dx%dx%d network\n", filename, n1, n2, n3);
	//printf("Reading input weights...");
	fflush(stdout);

	memcnt = 0;
	mem = (char *) malloc ((unsigned) ((n1+1) * (n2+1) * sizeof(double)));
	read(fd, mem, (n1+1) * (n2+1) * sizeof(double));
	for (int i = 0; i <= n1; i++) 
	{
		for (int j = 0; j <= n2; j++) 
		{
			fastcopy(&(new->input_weight_mat[i][j]), &mem[memcnt], sizeof(double));
			memcnt += sizeof(double);
		}

	}
	free(mem);

	//printf("Done\nReading hidden weights...");
	fflush(stdout);

	memcnt = 0;
	mem = (char *) malloc ((unsigned) ((n2+1) * (n3+1) * sizeof(double)));
	read(fd, mem, (n2+1) * (n3+1) * sizeof(double));
	for (int i = 0; i <= n2; i++) 
	{
		for (int j = 0; j <= n3; j++) 
		{
			fastcopy(&(new->hidden_weight_mat[i][j]), &mem[memcnt], sizeof(double));
			memcnt += sizeof(double);

		}

	}
	free(mem);
	close(fd);

	//printf("Done\n");
	fflush(stdout);

	bpnn_zero_weights(new->input_prev_weights, n1, n2);
	bpnn_zero_weights(new->hidden_prev_weights, n2, n3);

	return (new);
}
