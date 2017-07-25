/*________________________________________*
  |	
  |	* File Name     : main.c
  |	* Creation Date : 24-10-2016
  |  * Last Modified : Lun 26 déc 13:02:16 2016
  |	* Created by    :	    Claire
 *________________________________________**/

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>
# include <unistd.h>
# include <gtk/gtk.h>

# include "img_process.h" 
# include "backprop.h"
# include "image_to_net.h"
//# include "pixel_operations.h"


GtkWidget *g_label_text;
char *path_to_image;
GtkWidget *image_label;
char *text= "text";

void 		bpnn_init_training(	BPNN *net, 
		char *netname, 
		int epochs, 
		int nb_train, 
		double **inputs_vect_list, 
		double **target_list) 
{

	int 	index;
	double 	sumerr;
	double 	hid_err;
	double 	out_err;
	double 	**outputs_vect_list;

	//	list_errors = 0;
	sumerr 		= 1.0;
	outputs_vect_list = malloc(nb_train * sizeof(double *));

	for (int i = 0; i < nb_train; i++)
		outputs_vect_list[i] = malloc (62 * sizeof(double));

	for (int epoch = 1; epoch <= epochs && sumerr > 0.00003; epoch++)
	{
		index = (int)rand() % (nb_train);

		net->inputs_vect = inputs_vect_list[index];
		net->target = target_list[index];
		if (net->inputs_vect == NULL || net->target == NULL )
			puts("NULL values morron");
		bpnn_train(net, 0.01, 0.1, &out_err, &hid_err);
		sumerr = (out_err + hid_err);
		for (int oi = 0; oi < 62; oi++)
			outputs_vect_list[index][oi] = net->outputs_vect[oi];

		if(epoch % 100 == 0)
		{
			bpnn_save(net, netname);
			printf("error = %f\n", sumerr);
		}

	}
	printf("Final error : %f\n", sumerr);
	bpnn_save(net, netname);


	printf("Final List :\n");
	for (int i = 0; i < nb_train; i++)
	{
		for (int k = 0; k < 28 * 28; k++)
		{
			if (k % 28 == 0)
				printf("\n");

			if ((int) inputs_vect_list[i][k])
				printf("%d ", (int) inputs_vect_list[i][k]);
			else
				printf("  ");

		}
		puts("Target vectors");
		for(int l = 0; l < 62; l++)
		{
			printf("%f |", target_list[i][l]);
		}
		puts(" ");
		printf(" corresponding to char \t %c\n ", target_to_char(target_list[i]));

		puts("Outputs vectors");
		for(int j = 0; j < 62; j++)
		{
			printf("%f |", outputs_vect_list[i][j]);
		}
		puts(" ");
		printf(" corresponding to char \t %c\n ", target_to_char(outputs_vect_list[i]));
	}
}

void			img_to_train( char **params)
{
	puts("First argument is the name of the bpnn to create/load.");
	puts("2nd is the number of epochs");

	BPNN 		*net 		= malloc (1 * sizeof(BPNN));
	char 		*netname 	= strdup(params[2]);


	if ( (access(netname, F_OK)) != -1 )
	{
		net = bpnn_read(netname);
		puts("net loaded for training");
	}
	else
	{
		net = bpnn_create(28 * 28, 15, 62);
		puts("net created for training");
	}
	if (!(net))
	{
		puts("MAIN : error null bpnn");
		return;
	}

	/*========================INITIALIZATION================================*/

	int 		epochs;
	int 		k;
	int			nb_train;
	double 		**training_list;
	double 		**targets_vector_list;
	SDL_Surface *img;

	epochs 			= atoi(params[3]);
	nb_train 		= 26 * 2 * 8;/* (5188 + 4816)*/;
	k				= 0;
	training_list 	= malloc ((nb_train + 1) * sizeof(double *));
	for (int i = 0; i <= nb_train; i++)
	{
		training_list[i] = malloc ((1 + 28 * 28) * sizeof(double)); 
	}
	targets_vector_list = malloc ((nb_train + 1) * sizeof(double *));
	for (int i = 0; i <= nb_train; i++)
	{
		targets_vector_list[i] = calloc(62, sizeof(double));
	}


	/*=============================TRAINING=============================== */

	puts("loading and processing images");
	char *lup;
	lup = strdup("upper");

	for (char c = 'A'; c <= 'z'; c++)
	{
		if ((int)c == 91) //Passe de Z à a
		{
			c = 'a';
			lup = strdup("lower"); //On change le chemin d'accès
		}

		for (int i = 0; i < 200 ; i++) //200
		{
			char *img_name = malloc(100 * sizeof(char));
			sprintf(img_name, "/Users/utilisateur/Documents/2020-s3-project/morin_j/training_ipsum/%s/%c/%d.bmp", lup, c, i);
			//if (strcmp(lup, "lower") == 0)
			//	sprintf(img_name, "/home/helme__c/.ssh/GitRep/morin_j/training/%s/%c/%d.bmp", lup, c - 32, i);

			if( access( img_name, F_OK  ) != -1  )
			{
				img = image_binarize(img_name);
				build_training_list(img, training_list[k]);
				build_target_list(c, targets_vector_list[k]);
				k++;
			}
			free(img_name);
		}
	}
	printf("%d\n", k);
	epochs = char_to_int(params[3]);
	printf("Done.\nStarting taining with %d image_vect\n", nb_train);

	bpnn_init_training(net, netname, epochs, nb_train, training_list, targets_vector_list);

	printf("Training done\n");

}

/*===========================RECOGNITION===============================*/
void addSpace()
{
	gtk_label_set_text(GTK_LABEL(g_label_text), strcat((char *) gtk_label_get_text(GTK_LABEL(g_label_text)), " "));
}

char 		bpnn_recognition(double *inputMat)
{
	BPNN 			*net;
	char			*netname;
	char 			result;
	double 			*outputs_vect_list;

	netname 		= strdup("A-Za-z2.bpnn");
	net 			= bpnn_read(netname);

	if (!(net))
	{
		puts("BPNN_REGOGNITION : net shouldn' be null ");
		return 0;
	}
	//puts("loading and processing images");

	net->inputs_vect = inputMat;
	outputs_vect_list = calloc(62, sizeof(double));
	net->target = calloc(62, sizeof(double));
	if (net->inputs_vect == NULL || net->target == NULL)
		puts("NULL values morron");
	bpnn_feedforward(net);
	for (int i = 0; i < 62; i++)
	{
		//printf("At : %d - %f\n", i, net->outputs_vect[i]);
		outputs_vect_list[i] = net->outputs_vect[i];
	}
	result = target_to_char(outputs_vect_list);
	printf("%c\n", result);

	//bpnn_free(net);
	//
	gtk_label_set_text(GTK_LABEL(g_label_text), strcat((char *) gtk_label_get_text(GTK_LABEL(g_label_text)), &result));
	return result;
}

int gtk(int argc, char **argv)
{
	GtkBuilder *gtkBuilder = calloc (1, sizeof(GtkBuilder));
	GtkWidget *window = calloc (1, sizeof(GtkWidget));
	gtk_init(&argc, &argv);

	gtkBuilder = gtk_builder_new();
	gtk_builder_add_from_file(gtkBuilder, "testGlade.glade", NULL);
	window = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "mywindow"));
	gtk_builder_connect_signals(gtkBuilder, NULL);
	g_label_text= GTK_WIDGET(gtk_builder_get_object(gtkBuilder,"text"));
	image_label = GTK_IMAGE(gtk_builder_get_object(gtkBuilder,"image_label")); 
	g_object_unref(G_OBJECT(gtkBuilder));

	gtk_widget_show(window);
	//  gtk_image_show(image_label); 
	gtk_main();

	return 0;


}
void clean_text()
{
	gtk_label_set_text(GTK_LABEL(g_label_text),"");
}
void run_code()
{
	/*char *s = "reco";
	  char **v = malloc(2 * sizeof(char*));
	 *v[0] = *path_to_image;
	 *v[1] = *s;*/
	//printf("%s\n", path_to_image);

	//text = bpnn_recognition(path_to_image);
	//
	SDL_Surface *img = load_image(path_to_image);
	binarize_image(img);
	SDL_Surface* imgORI = copySurface(img);
	SDL_Surface* imgORI2 = copySurface(img);
	SDL_Surface* imgORI3 = copySurface(img);
	//	display_image(img);

	//grayscale_image(img);
	//generateEdgeImage(img);
	//display_image(img);
	TextRegion *TextR = detectTextRegions(img);
	//SegmentTextRegions(TextR, imgORI, 0);
	//	display_image(imgORI);
	//SegmentTextRegions(TextR, imgORI2, 1);
	//	display_image(imgORI2);
	//SegmentTextRegions(TextR, imgORI3, 2);
	//	display_image(imgORI3);
	//
	puts("Done");

}

void open_image(GtkFileChooser * file_finder)
{
	path_to_image = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_finder)); //we save the path
	// gtk_label_set_text(GTK_LABEL(g_label_text),path_to_image); //test
	// gtk_image_set_from_file(image_label,path_to_image);
	gtk_image_set_from_file(GTK_IMAGE(image_label),path_to_image);

}

void save_new_file()
{
	FILE *fp;	
	fp=fopen("OCR.txt","w");

	if(fp!=NULL)
	{
		fputs(text, fp);
		fclose(fp);
	}

}

int				main(int argc, char **argv)
{
	if (argc < 2)
		return 1; 
	if(strcmp(argv[1], "train") == 0)
		img_to_train(argv);
	if(strcmp(argv[1], "gtk") == 0)
		gtk(argc, argv);
	else
		return 0;
	return 0;

}

