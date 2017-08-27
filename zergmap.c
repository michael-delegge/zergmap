#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <byteswap.h>
#include <getopt.h>

#include "zergmap.h"
#include "math_functions.h"

int main(int argc, char **argv)
{
	pcap_file_header file_head;
	ethernet_frame ethernet;
	ipv4_header ipv4;
	udp_header udp;
	zerg_header zerg_head;
	status stat;
	gps_data gps;
	ends* node_ends;
	matrix* m;

	FILE *fp;

	int type;
	int length;
	int i;
	int j = 1;
	int node_count = 0;
	int health_default = 10;
	int flag;
	int files;

	node_ends = initiate_ends();

	//Checks if any files were entered
	if (argc < 2)
	{
		printf("Please enter any number of valid .pcap files.\n");
		exit(1);
	}
	//Check for flags on the command line	
	
	else
	{
		while((flag = getopt(argc, argv, "h:ar")) != -1)
		{
			switch(flag)
			{
				case ('h'):
					health_default = atoi(optarg);
					break;
				case ('a'):
					printf("Not implemented.\n");
					break;
				case ('r'):
					printf("Not implemented.\n");
					break;
				default:
					return 1;
			}
		}
		//Iterate through each pcap
		for (i = 1; i < argc; i++)
		{
			fp = fopen(argv[i], "rb");
			if (!fp) //If there is an invalid pcap
			{
				printf("File could not be read.\n");
				continue;
			}
			//Get length of pcap
			fseek(fp, 0, 2);
			length = ftell(fp);
			rewind(fp);
			
			fread(&file_head, sizeof(file_head), 1, fp);

			while(!feof(fp)) //Update while the file is still read
			{
				if (packet_header(fp) == 1)
				{
					break;
				}
				printf("***** Packet %d *****\n", j);
				fread(&ethernet, sizeof(ethernet), 1, fp);
				
				fread(&ipv4, sizeof(ipv4), 1, fp);
				if (ipv4.version == 6)
				{
					fseek(fp, 20, SEEK_CUR); //fseek forward in the pcap if it is ipv6
				}
				
				fread(&udp, sizeof(udp), 1, fp);
				type = read_zerg_header(fp);
				if (type == 1)
				{
					status_of_zerg(fp);
				}
				else if (type == 3)
				{
					fread(&gps, sizeof(gps), 1, fp);

					get_zerg_id(ntohs(zerg_head.source_zerg), node_ends, &gps);
					get_gps(&gps);
					initiate_gps(ntohs(zerg_head.source_zerg), &gps, node_ends);
				}
				else
				{
					printf("Packet type not status or gps.\n");
				}
				j++;
			}
			fclose(fp);
		}
		node* temp = node_ends->head;
		while (temp)
		{
			node_count = (node_count + 1);
			temp = temp->next;
		}
		m = initiate_matrix(node_ends);
		print_matrix(m->mat, node_count); //Prints the matrix
		return(0);
		
	}
}

//Reads packet header to determine end of file
int packet_header(FILE *fp)
{
	pcap_packet_header packet_head;
	fread(&packet_head, sizeof(packet_head), 1, fp);	
	if (feof(fp))
	{
		return(1);
	}
	else
	{
		return(0);
	}
}

//Reads zerg header file and returns the type of pcap it is
int read_zerg_header(FILE *fp)
{
	zerg_header zerg_head;
	fread(&zerg_head, sizeof(zerg_head), 1, fp);
	printf("Zerg : %.4d\n", ntohs(zerg_head.source_zerg));
	return (zerg_head.version & 0x0f);
}

//Sets initial value of hp
void status_of_zerg(FILE *fp)
{
	status stat;
	unsigned int hp;
	unsigned int max_hp;

	fread(&stat, sizeof(stat), 1, fp);

	hp = swap24(stat.hit_points);
	max_hp = swap24(stat.max_hit_points);

	printf("HP : %d / %d\n", hp, max_hp);
}

//Gets and prints initial gps data
gps_data* get_gps(gps_data* gps)
{
	double lat;
	double lon;
	float alt;

	lat = convert64(swap64(gps->lat));
	lon = convert64(swap64(gps->lon));
	alt = binary32(gps->alt);

	printf("Latitude : %f\n", lat);
	printf("Longitude : %f\n", lon);
	printf("Altitude : %6.4f\n\n", alt);

	return(gps);
}

//Allocates memory for head of file
ends* initiate_ends()
{
	ends* temp_ends = malloc(sizeof(ends));
	temp_ends->head = NULL;
	return(temp_ends);
}

//Function adds nodes and sets values for gps information
void get_zerg_id(int compare, ends* node_ends, gps_data* gps)
{
	node* list = node_ends->head;
	node* temp_node;
 
	if(!list)
	{
		temp_node = initiate_node();
		temp_node->id = compare;
		node_ends->head = temp_node;
	}
	else
	{
		list = node_ends->head;
		
		while(list)
		{
			if(list->id == compare)
			{
				list->lon = convert64(swap64(gps->lon));
				list->lat = convert64(swap64(gps->lat));
				list-> alt = binary32(gps->alt);
				return;
			}
			list = list->next;
		}
		while(list)
		{
			if(list->next == NULL)
			{
				temp_node = initiate_node();
				temp_node->id = compare;
				list->next = temp_node;
				return;
			}
		list = list->next;
		}
		return;
	}
}

//Sets initial values of nodes to 0
node* initiate_node()
{
	node* temp_node = malloc(sizeof(node));
	temp_node->id = 0;
	temp_node->hp_max = 0;
	temp_node->hp = 0;
	temp_node->lat = 0;
	temp_node->lon = 0;
	temp_node->alt = 0;
	temp_node->next = NULL;
	
	return(temp_node);
}

//Prints data from nodes
void print_zergs (ends* node_ends)
{
	node* temp = node_ends->head;
	
	while(temp)
	{
		printf("ID: %d\n", temp->id);
		printf("HP: %d/%d\n", temp->hp, temp->hp_max);
		printf("Longitude: %lf\n", temp->lon);
		printf("Latitude: %lf\n", temp->lat);
		printf("Altitude: %lf\n\n", temp->alt);
		temp = temp->next;
	}
}

//Function that starts making the graph
matrix* initiate_matrix(ends* node_ends)
{
	node* temp = node_ends->head;
	node* compare = node_ends->head;

	double dist;
	int row = 0;
	int col = 0;
	int nodes = 0;

	matrix* m = malloc(sizeof(matrix));

	while(temp)
	{
		nodes = (nodes + 1);
		temp = temp->next;
	}

	build_matrix(nodes, m);
	temp = node_ends->head;
	m->vert = nodes;
	build_graph(m);
	
	while(temp)
	{
		while(compare)
		{
			dist = haversine(temp, compare);
			if(dist <= 15 && dist >= 1.143)
			{
				m->mat[row][col] = dist;
			}
			col++;
			compare = compare->next;
		}
		row++;
		col = 0;
		temp = temp->next;
		compare = node_ends->head;
	}
	return(m);
}
	
//Set initial values of graph to 0	
void build_graph(matrix* a)
{
	int i = 0;
	int count = 0;
	
	for (i = 0; i <a->vert; i++)
	{
		for (count = 0; count < a->vert; count++)
		{
			a->mat[i][count] = 0.0;
		}
	}
}

//Creates the matrix and allocates memory
void build_matrix(int vert, matrix* a)
{
	int i = 0;
	
	a->vert = vert;
	a->mat = malloc(vert * sizeof(double*));
	
	for(i = 0; i < vert; i++)
	{
		a->mat[i] = (double*)malloc(vert * sizeof(double));
	}
}

//Prints matrix
void print_matrix(double *g[], int v)
{
	int i = 0;
	int count = 0;
	printf("*****Matrix*****\n");
	for(i = 0; i < v; i++)
	{
		for(count = 0; count < v; count++)
		{
			printf(" %.4f ", g[i][count]);
		}
		printf("\n");
	}
}

//Sets values for gps data
void initiate_gps(int compare, gps_data* gps, ends* node_ends)
{
	node* temp = node_ends->head;
	
	while(temp)
	{
		if(temp->id == compare)
		{
			temp->lon = convert64(swap64(gps->lon));
			temp->lat = convert64(swap64(gps->lat));
			if (gps->alt == 0)
			{
				temp->alt = 0;
			} 
			else
			{
				temp->alt = binary32(gps->alt);
			}
		}
		temp = temp->next;
	}
}

//Haversine formula to calculate distance between zerg
double haversine(node* a, node* b)
{
		const double radius = 6371.8;
		const double rad = (3.14159265 / 180);
        double longa = a->lon;
        double lata = a->lat;
        double alta = a->alt;

        double longb = b->lon;
        double latb = b->lat;
        double altb = b->alt;

        double dlog = rad * ((longb - longa) / 2);
        double dlat = rad * ((latb - lata) / 2);

        longa *= rad;
        longb *= rad;
        lata *= rad;
        latb *= rad;

        double A = 0.5 * (1 - cos(2 * dlat));
        double B = cos(lata) * cos(latb);
        double C = 0.5 * (1 - cos(2 * dlog));

        double linearD = 2 * radius * asin(sqrt(A + B * C));

        double heightD = (alta - altb) / 1000;

        return 1000 * sqrt(linearD * linearD + heightD * heightD);
}
