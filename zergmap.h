#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <arpa/inet.h>

typedef struct NODE
{
	int hp:24;
	int hp_max:24;
	int id:16;
	double lat;
	double lon;
	double alt; 
	struct NODE* next;
}node;

typedef struct MATRIX
{
	double **mat;
	int edge;
	int vert;
}matrix;

typedef struct LIST
{
	double dist;
	int zid;
	struct LIST *next_list;
}list;

typedef struct ENDS
{
	struct NODE* head;
	struct NODE* tail;
}ends;

typedef struct PCAP_FILE_HEADER
{
	uint32_t file_type;
	uint16_t major_version;
	uint16_t minor_version;
	uint32_t gmt_offset;
	uint32_t accuracy_delta;
	uint32_t capture_length;
	uint32_t link_layer;
}pcap_file_header;

typedef struct PCAP_PACKET_HEADER
{
	uint32_t unix_epoch;
	uint32_t something;
	uint32_t data_len;
	uint32_t untruncated;
}pcap_packet_header;

typedef struct ETHERNET_FRAME
{
	uint8_t dest_mac[6];
	uint8_t source_mac[6];
	uint16_t ethernet_type;
}ethernet_frame;

typedef struct IPV4_HEADER
{
	uint8_t version;
	uint8_t DSCP;
	uint16_t total_length;
	uint16_t identification;
	uint16_t flags;
	uint8_t TTL;
	uint8_t protocol;
	uint16_t header_checksum;
	uint8_t source_ip[4];
	uint8_t destination_ip[4];
}ipv4_header;

typedef struct UDP_HEADER
{
	uint16_t source_port;
	uint16_t destination_port;
	uint16_t length;
	uint16_t checksum;
}udp_header;

typedef struct ZERG_HEADER
{
	uint8_t version;
	uint8_t total_length[3];
	uint16_t dest_zerg;
	uint16_t source_zerg;
	uint32_t sequence;
}zerg_header;

typedef struct MESSAGE_PAYLOAD
{
	uint32_t message;
}message_payload;

typedef struct STATUS
{
	uint8_t hit_points[3];
	uint8_t armor;
	uint8_t max_hit_points[3];
	uint8_t type;
	uint32_t speed;
	uint32_t name;
}status;

typedef struct COMMAND
{
	uint16_t command_num;
}command;

typedef struct GPS_DATA
{
	uint64_t lon;
	uint64_t lat;
	uint32_t alt;
	uint32_t bearing;
	uint32_t speed;
	uint32_t accuracy;
}gps_data;

void read_zerg_message(FILE *fp, unsigned int length);
void status_of_zerg(FILE *fp);
void zerg_type(char*zergs, int type);
void command_instruction(FILE *fp);
void command_number(char *comm, int command);
int packet_header(FILE *fp);
int read_zerg_header(FILE *fp);

void get_zerg_id(int compare, ends* node_ends, gps_data* gps);
void print_zergs (ends* node_ends);
void build_graph(matrix* a);
void build_matrix(int vert, matrix* a);
void print_matrix(double *g[], int v);
void initiate_gps(int compare, gps_data* gps, ends* node_ends);
double haversine(node* a, node* b);

matrix* initiate_matrix(ends* node_ends);
node* initiate_node();
ends* initiate_ends();
gps_data* get_gps(gps_data* gps);
