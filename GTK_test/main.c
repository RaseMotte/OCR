# include <gtk/gtk.h>
#include <stdio.h>

#include <string.h>
 GtkWidget *g_label_text;
// GtkFileChooserButton *file_finder;
 char *path_to_image; // in this string we will put the path of the image
 //GtkImage *image_label ;
 GtkWidget *image_label;
 char *text="hello"; 
int main(int argc, char *argv[])
{
    GtkBuilder *gtkBuilder;
    GtkWidget *window;
    gtk_init(&argc, &argv);
    
    gtkBuilder = gtk_builder_new();
    gtk_builder_add_from_file(gtkBuilder, "testGlade.glade", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "mywindow"));
    gtk_builder_connect_signals(gtkBuilder,NULL);
    g_label_text= GTK_WIDGET(gtk_builder_get_object(gtkBuilder,"text"));
    image_label= GTK_IMAGE(gtk_builder_get_object(gtkBuilder,"image_label")); 
    g_object_unref(G_OBJECT(gtkBuilder));
   
  gtk_widget_show(window);
 //  gtk_image_show(image_label); 
  gtk_main();
    
    return 0;
}

/*void on_window_main_destroy()
{
    gtk_main_quit();
}

void browse()
{
  

}*/
void clean_text(){
gtk_label_set_text(GTK_LABEL(g_label_text),"");
}
void run_code()
{
	gtk_label_set_text(GTK_LABEL(g_label_text),text);
 
}

void open_image(GtkFileChooser * file_finder){
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
