#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include "roadmap.h"

#define VERTEX_NAME			"Vertex"
#define EDGE_NAME			"Edge"
#define FILE_READ_MAX_LINE		(1024)
#define FILE_DELIMITERS			",\n"
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

struct dkInfo {
    int visited;
    int parent;
    int hops;
    double time;
};

struct edge {
    int dest;
    double speed;
    int edgecount;
    double distance;
    int direction;
    char edgename[EDGE_NAME_SIZE];
    char roadname[ROAD_NAME_SIZE];
    enum edge_event event;
    struct edge* next;
};

struct vertex {
    int vertexno;
    char name[VERTEX_NAME_SIZE];
    enum vertex_type type;
    struct edge* head;
};

struct edgeInfo {
    char src[VERTEX_NAME_SIZE];
    char dest[VERTEX_NAME_SIZE];
    char edgename[EDGE_NAME_SIZE];
    char roadname[ROAD_NAME_SIZE];
    double speed;
    double distance;
    enum edge_event event;
};

struct vertexInfo {
    enum vertex_type type;
    char name[VERTEX_NAME_SIZE];
};

struct roadInfo {
    int found;
    int src;
    int dest;
    char roadname[ROAD_NAME_SIZE];
    double speed;
    double distance;
};

struct Graph {
    struct vertex* array;
};

const char* NOT_FOUND = "NOTFOUND";

static int counter = 0;
static int edgecounter = 0;
static struct Graph graph;

static int compare_strings(char *str1, char *str2)
{
    unsigned int s1len, s2len, x, y, lastdiag, olddiag,temp;
    
    s1len = strlen(str1);
    s2len = strlen(str2);
    unsigned int column[s1len+1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x-1; y <= s1len; y++) {
            olddiag = column[y];
            
            if(str2[x-1]>='A' && str2[x-1] <= 'Z') {
                if(str1[y-1] == str2[x-1]) {
                    temp = 0;
                } else {
                    if(str1[y-1] == (str2[x-1]+32)) {
                        temp = 0;
                    } else {
                        temp = 1;
                    }
                }
                
            } else {
                if(str1[y-1] == str2[x-1]) {
                    temp = 0;
                } else {
                    if(str1[y-1] == (str2[x-1]-32)) {
                        temp = 0;
                    } else {
                        temp = 1;
                    }
                }
            }
            //column[y] = MIN3(column[y] + 1, column[y-1] + 1,
            //lastdiag + (str1[y-1] == str2[x-1] ? 0 : 1));
            
            column[y] = MIN3(column[y] + 1, column[y-1] + 1,
                             lastdiag + temp);
            
            lastdiag = olddiag;
        }
    }
    return(column[s1len]);
}

static const char* getName(int v)
{
    int i = 0;
    for(i = 0; i < counter; i++) {
        if(graph.array[i].vertexno == v)
            return graph.array[i].name;
    }
    fprintf(stderr, "%d Vertex not found\n", v);
    return NOT_FOUND;
}

static int getVertex(char* vertex)
{
    int i = 0;
    
    if(vertex == NULL) {
        return -1;
    }
    
    for(i = 0; i < counter; i++) {
        if(!strcmp(graph.array[i].name, vertex))
            return graph.array[i].vertexno;
    }
    return -1;
}

static int createEdge(int src, int dest, int directional, int speed, int length)
{
    struct edge* pedge = graph.array[src].head;
    struct edge* newedge = NULL;
    while (pedge) {
        if(pedge->dest == dest) {
            fprintf(stderr, "Edge from %s to %s already exist\n", getName(src), getName(dest));
            return -1 ;
        }
        pedge = pedge->next;
    }
    
    newedge = malloc(sizeof(struct edge));
    if(newedge == NULL) {
        fprintf(stderr, "Malloc failed\n");
        return -1;
    }
    newedge->dest = dest;
    strncpy(newedge->edgename, getName(src), VERTEX_NAME_SIZE);
    strcat(newedge->edgename,"-");
    strncat(newedge->edgename, getName(dest), VERTEX_NAME_SIZE);
    memset(newedge->roadname, 0, ROAD_NAME_SIZE);
    strncpy(newedge->roadname, newedge->edgename, EDGE_NAME_SIZE);
    newedge->distance = length;
    newedge->event = EVENT_NORMAL;
    newedge->speed = speed;
    newedge->edgecount = edgecounter;
    newedge->direction = directional;
    newedge->next = graph.array[src].head;
    
    graph.array[src].head = newedge;
    edgecounter++;
    
    return 0;
}

static int edgeAddRoad(char* edgename, char* roadname)
{
    int i = 0;
    
    if(edgename == NULL || roadname == NULL) {
        fprintf(stderr, "Edgename or Roadname Invalid\n");
        return -1;
    }
    
    for(i = 0; i < counter; i++) {
        struct edge* pedge = graph.array[i].head;
        while (pedge) {
            if(!strcmp(pedge->edgename, edgename)) {
                memset(pedge->roadname, 0, ROAD_NAME_SIZE);
                strncpy(pedge->roadname, roadname, ROAD_NAME_SIZE);
                return 0;
                
            }
            pedge = pedge->next;
        }
    }
    
    fprintf(stderr, "Edgename (%s) not found\n", edgename);
    return -1;
}

static int printRoad(int* route, int hops, char** path, int* size)
{
    int i = 0;
    int j = 0;
    struct roadInfo road;
    int maxSize = 0;
    int sz = 0;
    char *p = NULL;
    char *q = NULL;
    
    memset(&road, 0, sizeof(road));
    
    if(route == NULL || hops <= 1 || size == NULL || path == NULL) {
        fprintf(stderr, "Invalid Input\n");
        return -1;
    }
    
    maxSize = hops * (sizeof(road.roadname) + sizeof(road.speed) + sizeof(road.distance));
    maxSize += sizeof(sz) + 1;
    
    printf("Size %d\n", sz);
    
    q = calloc(maxSize, 1);
    if(q == NULL) {
        printf("Malloc failed\n");
        return -1;
    }
    
    sz = sizeof(sz);
    
    for(i = 0; i < hops-1; i++) {
        struct edge* pedge = graph.array[route[i]].head;
        while (pedge) {
            if(pedge->dest == route[i+1]) {
                if(i == 0) {
                    road.src = route[i];
                } else if(i != 0 &&
                          (strcmp(pedge->roadname, road.roadname) != 0 ||
                           pedge->speed != road.speed)) {
                              
                              printf("Take road \"%s\" MAX SPEED: %.2f and in %.2f KM\n",
                                     road.roadname, road.speed, road.distance);
                              
                              strncpy(&q[sz], road.roadname, sizeof(road.roadname));
                              sz += sizeof(road.roadname);
                              
                              p = &road.speed;
                              for(j = 0; j < sizeof(road.speed); j++) {
                                  q[sz++] = p[j];
                              }
                              
                              p = &road.distance;
                              for(j = 0; j < sizeof(road.distance); j++) {
                                  q[sz++] = p[j];
                              }
                              
                              memset(road.roadname, 0, ROAD_NAME_SIZE);
                              
                              road.src = route[i];
                              road.speed = 0;
                              road.distance = 0;
                          }
                
                road.found = 1;
                road.dest = route[i+1];
                road.speed = pedge->speed;
                road.distance += pedge->distance;
                strncpy(road.roadname, pedge->roadname, ROAD_NAME_SIZE);
            }
            pedge = pedge->next;
        }
    }
    
    // print the last road info
    if(road.found && i+1 == hops) {
        printf("Take road \"%s\" MAX SPEED: %.2f and in %.2f KM destination reached\n",
               road.roadname, road.speed, road.distance);
        
        strncpy(&q[sz], road.roadname, sizeof(road.roadname));
        sz += sizeof(road.roadname);
        
        p = &road.speed;
        for(j = 0; j < sizeof(road.speed); j++) {
            q[sz++] = p[j];
        }
        
        p = &road.distance;
        for(j = 0; j < sizeof(road.distance); j++) {
            q[sz++] = p[j];
        }
        
    }
    
    *size = sz;
    
    p = &sz;
    for(j = 0; j < sizeof(sz); j++) {
        q[j] = p[j];
    }
    
    *path = q;
    
    return 0;
}

static int dk(int src, int des, char** path, int* size)
{
    int i = 0;
    int count = 0;
    int curNode = 0;
    int hops = 0;
    int* route = NULL;
    
    if(size == NULL || path == NULL) {
        printf("Invalid Input\n");
        return -1;
    }
    
    *path = NULL;
    *size = 0;
    
    if(src == des) {
        printf("You are already at %s\n", getName(src));
        return 0;
    }
    
    struct dkInfo* nodeInfo = malloc(sizeof(*nodeInfo) * counter);
    if(nodeInfo == NULL) {
        printf("malloc failed\n");
        return -1;
    }
    
    for(i = 0; i < counter; i++) {
        nodeInfo[i].time = DBL_MAX;
        nodeInfo[i].parent = -1;
        nodeInfo[i].visited = 0;
        nodeInfo[i].hops = 0;
    }
    
    nodeInfo[src].time = 0;
    nodeInfo[src].parent = -1;
    
    while(count < counter) {
        double min = DBL_MAX;
        for(i = 0; i < counter; i++) {
            //printf("Node: %s Time: %f\n",
            //		getname(mgraph, curNode), nodeInfo[i].time);
            if(nodeInfo[i].visited == 0 && nodeInfo[i].time < min) {
                min = nodeInfo[i].time;
                curNode = i;
            }
        }
        
        nodeInfo[curNode].visited = 1;
        struct edge* pedge = graph.array[curNode].head;
        
        while (pedge != NULL) {
            double time = nodeInfo[curNode].time +
            (pedge->distance/pedge->speed);
            //printf("<<<cur: %s, dest: %s, time: %f\n",
            //		getName(curNode), getName(pedge->dest), time);
            if(pedge->event == EVENT_NORMAL &&
               nodeInfo[pedge->dest].visited == 0 &&
               time < nodeInfo[pedge->dest].time) {
                nodeInfo[pedge->dest].time = time;
                nodeInfo[pedge->dest].parent = curNode;
                nodeInfo[pedge->dest].hops = nodeInfo[curNode].hops + 1;
                //printf(">>>parent:%s, child: %s, time: %f\n",
                //			getName(curNode), getName(pedge->dest), time);
            }
            pedge = pedge->next;
        }
        
        count++;
    }
    
    hops = nodeInfo[des].hops + 1;
    curNode = des;
    
    if(nodeInfo[des].time == DBL_MAX) {
        printf("No route from %s to %s\n", getName(src), getName(des));
        return -1;
    } else {
        //printf("Hops %d\n", hops);
        route = malloc(sizeof(*route) * (hops));
        if(route == NULL) {
            fprintf(stderr, "Malloc failed\n");
            return -1;
        }
        
        i = hops;
        while(i > 0) {
            route[--i] = curNode;
            curNode = nodeInfo[curNode].parent;
        }
    }
    
#if 0
    printf("\n---------------------------------------------------\n");
    printf("Shortest Path from %s to %s\n", getName(src), getName( des));
    printf("Travel Vertex");
    for(i = 0; i < hops; i++) {
        printf(": %s ", getName(route[i]));
    }
    printf("\n");
    
    printf("Total Time: %.2f Hr\n", nodeInfo[des].time);
    printf("---------------------------------------------------\n");
#endif
    
    printRoad(route, hops, path, size);
    
    if(route != NULL)
        free(route);
    
    if(nodeInfo != NULL)
        free(nodeInfo);
    
    return nodeInfo[des].time;
}

int addVertex(enum vertex_type type, char* name)
{
    if(type >= MAX_VERTEX_TYPE) {
        fprintf(stderr, "Vertex Type (%d) not found\n", type);
        return -1;
    }
    
    if(name == NULL) {
        fprintf(stderr, "Vertex Name invalid\n");
        return -1;
    }
    
    if(getVertex(name) != -1) {
        fprintf(stderr, "Vertex(%s) already added in the system\n", name);
        return -1;
    }
    
    struct vertex* newvertex = NULL;
    if(counter == 0) {
        graph.array = malloc(sizeof(struct vertex));
        if(graph.array == NULL) {
            fprintf(stderr, "Malloc failed\n");
            return -1;
        }
    } else {
        newvertex = realloc(graph.array, (counter + 1)*sizeof(struct vertex));
        if(newvertex == NULL) {
            fprintf(stderr, "Malloc failed\n");
            return -1;
        } else {
            graph.array = newvertex;
        }
    }
    
    graph.array[counter].head = NULL;
    graph.array[counter].vertexno = counter;
    strncpy(graph.array[counter].name, name, VERTEX_NAME_SIZE);
    graph.array[counter].type = type;
    counter++;
    
    return counter-1;
}

int addEdge(char* vertex1, char* vertex2, int directional, double speed, double length)
{
    int ret = 0;
    int src = -1;
    int dest = -1;
    
    if(vertex1 == NULL || vertex2 == NULL) {
        fprintf(stderr, "Vertex1 or Vertex2 NULL\n");
        return -1;
    }
    
    src = getVertex(vertex1);
    dest = getVertex(vertex2);
    
    if(src < 0 || src > counter) {
        fprintf(stderr, "Vertex1 (%s) not found\n", vertex1);
        fprintf(stderr, "Add Vertex to the system by calling addVertex\n");
        return -1;
    }
    
    if(dest < 0 || dest > counter) {
        fprintf(stderr, "Vertex2 (%s) not found\n", vertex2);
        fprintf(stderr, "Add Vertex to the system by calling addVertex\n");
        return -1;
    }
    
    if(directional < 0 || speed <= 0 || length <= 0) {
        if(directional < 0) {
            fprintf(stderr, "Directional can't be negative\n");
        } else if (speed <= 0) {
            fprintf(stderr, "Speed can't be negative or zero\n");
        } else {
            fprintf(stderr, "Length can't be negative or zero\n");
        }
        return -1;
    }
    
    if(directional >= 1) {
        directional = 1;
        ret = createEdge(dest, src, directional, speed, length);
        if(ret != 0) {
            fprintf(stderr, "Adding Edge %s from %s failed\n", vertex2, vertex1);
            return -1;
        }
    }
    
    ret = createEdge(src, dest, directional, speed, length);
    if(ret != 0) {
        fprintf(stderr, "Adding Edge %s from %s failed\n", vertex1, vertex2);
        return -1;
    }
    
    return 0;
}

int edgeEvent(char* edgename, enum edge_event event)
{
    int i = 0;
    
    if(edgename == NULL) {
        fprintf(stderr, "Edgename NULL\n");
        return -1;
    }
    
    if(event >= MAX_EDGE_EVENT) {
        fprintf(stderr, "Edge event (%d) not found\n", event);
        return -1;
    }
    
    for(i = 0; i < counter; i++) {
        struct edge* pedge = graph.array[i].head;
        while (pedge) {
            if(!strcmp(pedge->edgename, edgename)) {
                pedge->event = event;
                return 0;
                
            }
            pedge = pedge->next;
        }
    }
    
    fprintf(stderr, "Edgename (%s) not found\n", edgename);
    return -1;
}

int road(char edges[][EDGE_NAME_SIZE], int edgescount, char* roadname)
{
    int err = 0;
    int add = 0;
    int i = 0;
    
    if(edgescount < 0) {
        fprintf(stderr, "Edges count invalid\n");
        return -1;
    }
    
    if(edges == NULL || roadname == NULL) {
        fprintf(stderr, "Road edges or name invalid\n");
        return -1;
    }
    
    for(i = 0; i < edgescount; i++) {
        err = edgeAddRoad(edges[i], roadname);
        if(err != 0) {
            fprintf(stderr, "Road %s for %s failed\n", roadname, edges[i]);
        } else {
            add++;
        }
        
    }
    
    return add;
}

int trip(char* vertex1, char* vertex2, char** route, int* size)
{
    int src = -1;
    int dest = -1;
    
    if(vertex1 == NULL || vertex2 == NULL) {
        fprintf(stderr, "Vertex1 or Vertex2 Invalid\n");
        return -1;
    }
    
    if(route == NULL || size == NULL) {
        printf("Invalid Input\n");
        return -1;
    }
    
    src = getVertex(vertex1);
    if(src < 0 || src > counter) {
        fprintf(stderr, "Vertex1 (%s) not found\n", vertex1);
        fprintf(stderr, "Please check your Source Location\n");
        return -1;
    }
    
    dest = getVertex(vertex2);
    
    if(dest < 0 || dest > counter) {
        fprintf(stderr, "Vertex2 (%s) not found\n", vertex2);
        fprintf(stderr, "Please check your Destination Location\n");
        return -1;
    }
    
    return dk(src, dest, route, size);
}
    char temp123[30];
char* vertex(char* vertex)
{
    int i = 0;
    
    int compare_cnt = 0;
    int tempc=100;
    
    if(vertex == NULL) {
        fprintf(stderr, "Vertex invalid\n");
        return NULL;
    }
    
    for(i = 0; i < counter; i++) {
        
        compare_cnt = compare_strings(graph.array[i].name, vertex);
        if(compare_cnt<tempc){
            strcpy(temp123,graph.array[i].name);
            tempc=compare_cnt;
        }
        }
  //  printf("corrected %s \n",temp);
    return temp123;
}

int store(char* filename)
{
    int i = 0;
    FILE* fp = NULL;
    
    if(filename == NULL) {
        fprintf(stderr, "File name NULL\n");
        return -1;
    }
    
    fp = fopen(filename, "w+");
    if(fp == NULL) {
        fprintf(stderr, "File open failed\n");
        return -1;
    }
    
    fprintf(fp, "%s\n", VERTEX_NAME);
    
    for(i = 0; i < counter; i++) {
        fprintf(fp, "%d,%s\n", graph.array[i].type, graph.array[i].name);
    }
    
    fprintf(fp, "%s\n", EDGE_NAME);
    
    for(i = 0; i < counter; i++) {
        struct edge* pedge = graph.array[i].head;
        while(pedge) {
            fprintf(fp, "%s,%s,%s,%s,%d,%0.2f,%0.2f\n",
                    graph.array[i].name, getName(pedge->dest),
                    pedge->edgename, pedge->roadname,
                    pedge->event, pedge->speed, pedge->distance);
            pedge = pedge->next;
        }
    }
    
    fclose(fp);
    
    return 0;
}

int retrieve(char* filename)
{
    int i = 0;
    FILE* fp = NULL;
    char* token = NULL;
    int type = 0;
    struct edgeInfo edge;
    struct vertexInfo vertex;
    char line[FILE_READ_MAX_LINE] = {0};
    
    memset(line, 0, FILE_READ_MAX_LINE);
    
    if(filename == NULL) {
        fprintf(stderr, "File name NULL\n");
        return -1;
    }
    
    fp = fopen(filename, "r");
    if(fp == NULL) {
        fprintf(stderr, "File %s open failed\n", filename);
        return -1;
    }
    
    while(fgets(line, sizeof(line), fp)) {
        //printf("%s", line);
        i = 0;
        token = strtok(line, FILE_DELIMITERS);
        while(token != NULL) {
            if(type == 0 && !strcmp(token, VERTEX_NAME)) {
                type = 1;
                memset(&vertex, 0, sizeof(vertex));
            } else if(type == 1 && !strcmp(token, EDGE_NAME)) {
                type = 2;
                memset(&edge, 0, sizeof(edge));
            }
            
            if(type == 1) {
                // Vertex
                if(i == 0) {
                    vertex.type = atoi(token);
                    i++;
                } else if (i == 1) {
                    strncpy(vertex.name, token, sizeof(vertex.name));
                    
                    // Add
                    addVertex(vertex.type, vertex.name);
                }
            } else if(type == 2) {
                // Edge
                if(i == 0) {
                    strncpy(edge.src, token, sizeof(edge.src));
                    i++;
                } else if (i == 1) {
                    strncpy(edge.dest, token, sizeof(edge.dest));
                    i++;
                } else if (i == 2) {
                    strncpy(edge.edgename, token, sizeof(edge.edgename));
                    i++;
                } else if (i == 3) {
                    strncpy(edge.roadname, token, sizeof(edge.roadname));
                    i++;
                } else if (i == 4) {
                    edge.event = atoi(token);
                    i++;
                } else if (i == 5) {
                    edge.speed = atof(token);
                    i++;
                } else if (i == 6) {
                    edge.distance = atof(token);
                    
                    // Add
                    addEdge(edge.src, edge.dest, 0, edge.speed, edge.distance);
                    edgeEvent(edge.edgename, edge.event);
                    edgeAddRoad(edge.edgename, edge.roadname);
                }
            }
            //printf("%s\n", token);
            token = strtok(NULL, FILE_DELIMITERS);
        }
    }
    
    fclose(fp);
    
    return 0;
}

void printGraph()
{
    int i = 0;
    printf ("EDGE NAME TYPE -> EDGE_NAME,ROAD_NAME,DISTANCE,SPEED,EVENT \n");
    for(i = 0; i < counter; i++) {
        struct edge* pedge = graph.array[i].head;
        printf("%3d %5s %3d\n",
               graph.array[i].vertexno, graph.array[i].name,
               graph.array[i].type);
        while(pedge) {
            printf("\t\t->%s, %s, %d, %0.2f KM, %0.2f KM/hr\n",
                   pedge->edgename, pedge->roadname, pedge->event,
                   pedge->distance, pedge->speed);
            pedge = pedge->next;
        }
    }
}

void freeGraph()
{
    int i = 0;
    struct edge* pedge = NULL;
    struct edge* pfree = NULL;
    for(i = 0; i < counter; i++) {
        pedge = graph.array[i].head;
        while(pedge) {
            pfree = pedge;
            pedge = pedge->next;
            free(pfree);
        }
    }
    
    if(graph.array)
        free(graph.array);
    
    counter = 0;
    edgecounter = 0;
}
