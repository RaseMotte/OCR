/*________________________________________*
 |	
 |	* File Name     : main.c
 |	* Creation Date : 05-12-2016
 |  * Last Modified : Lun  5 déc 00:46:42 2016
 |	* Created by    :	    Claire
 *________________________________________**/

#include "img_proc.h"
#include "pixel_operations.h"
int main(int argc, char **argv) 
{ 
	if (argc < 2)
		return 1;

	rotation(argv[1]);
	return 0; 
}
