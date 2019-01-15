/**
 * @file roadmap.h
 * @author Anson Jacob
 * @author Niyaz Murshed
 * @author Minita Dabhi
 * @date 20/11/2016
 * @brief Roadmap API Documentation
 */

#ifndef ROADMAP_H
#define ROADMAP_H

/** Maximum number of character for Vertex Name. */
#define VERTEX_NAME_SIZE		(20)
/** Maximum number of character for Edge Name. */
#define EDGE_NAME_SIZE			((2*VERTEX_NAME_SIZE) + 1)
/** Maximum number of character for Road Name. */
#define ROAD_NAME_SIZE			(3*VERTEX_NAME_SIZE)
/** Arbitary filename for saving and retreiving map. */
#define FILENAME			"graphInfo.txt"

/** Edge Direction */
enum edge_type {
    ONE_WAY,			/**< One-way edge / road */
    TWO_WAY				/**< Two-way edge / road */
};

/** Vertex Types */
enum vertex_type {
    POINT_OF_INTEREST,		/**< Point of Interest. */
    INTERSECTION,			/**< Intersection. */
    MAX_VERTEX_TYPE			/**< Maximum number of vertex types. */
};

/** Events at Edge */
enum edge_event {
    EVENT_NORMAL,			/**< Edge is functional. */
    EVENT_ACCIDENT,			/**< Edge has an accident. */
    EVENT_ROADBLOCK,		/**< Edge has a road block. */
    MAX_EDGE_EVENT			/**< Maximum numver of edge events. */
};

/**
 * @brief Adds a Vertex the Map.
 *
 * Usage:
 * @code
 * addVertex(POINT_OF_INTEREST, "DC");
 * addVertex(POINT_OF_INTEREST, "MC");
 * addVertex(INTERSECTION, "SLC");
 * @endcode
 * @param type Type of Vertex.
 * @param name Pointer to a character array which holds the name of the vertex.
 * @return Success: Total number of vertex in the Map.
 * @return Failure: -1.
 * @see addEdge
 * @see freeGraph
 */
int addVertex(enum vertex_type type, char* name);

/**
 * @brief Adds an Edge to the Map.
 *
 * Usage:
 * @code
 * addEdge("DC", "MC", ONE_WAY, 30, 6.0);
 * addEdge("MC", "SLC", TWO_WAY, 35, 4.2);
 * @endcode
 * @param vertex1 Character pointer to the name of the first edge.
 * @param vertex2 Character pointer to the name of the second edge.
 * @param directional 0: For single and 1: bi-directional edge.
 * @param speed Maximum Speed limit on the edge in KM/Hr.
 * @param length Distance between two edges in KM.
 * @return Success: 0.
 * @return Failure: -1.
 * @see addVertex
 * @see freeGraph
 *
 * By default, Edge event is set to EVENT_NORMAL.
 */
int addEdge(char* vertex1, char* vertex2, int directional, double speed, double length);

/**
 * @brief Adds an edge event on the Map.
 *
 * Usage:
 * @code
 * edgeEvent("DC-RR", EVENT_NORMAL);
 * edgeEvent("SLC-MC", EVENT_ROADBLOCK); // One-way roadblock
 * @endcode
 * @param edgename Name of edge formed by concatenating two vertex names by a '-'.
 * @param event type.
 * @return Success: 0.
 * @return Failure: -1.
 * @see freeGraph
 */
int edgeEvent(char* edgename, enum edge_event event);

/**
 * @brief Adds a Vertex the Map.
 *
 * Usage:
 * @code
 * #define NUM_OF_EDGES 1
 *
 * char roadInfo[NUM_OF_EDGES][EDGE_NAME_SIZE];
 * char roadname[ROAD_NAME_SIZE] = "Ring Road";
 *
 * memcpy(roadInfo[0], "DC-RR", EDGE_NAME_SIZE);
 *
 * road(roadInfo, NUM_OF_EDGES, roadname);
 * @endcode
 * @param edges A 2-D char array of Name's of edges formed by concatenating two vertex names by a '-'.
 * @param edgescount Number of rows in the 2-D array.
 * @param roadname Pointer to a character array which holds the name of the road.
 * @return Success: Returns the number of road names added.
 * @return Failure: -1.
 * @see freeGraph
 */
int road(char edges[][EDGE_NAME_SIZE], int edgescount, char* roadname);

/**
 * @brief Calculates the shortest feasible path between two vertex by using
 * <a href ="https://en.wikipedia.org/wiki/Dijkstra's_algorithm">
 * Dijkstra's Algorithm.</a>
 *
 * Usage:
 * @code
 * char* src;
 * char* dest;
 * int hops = 0;
 * int* route = NULL;
 *
 * src = vertex("DC");
 * dest = vertex("SLC");
 *
 * trip(src, dest, &hops, &route);
 *
 * if(route != NULL)
 *		free(route);
 * @endcode
 * @param vertex1 Pointer to a character array which holds the name of the source vertex
 * @param vertex2 Pointer to a character array which holds the name of the destination vertex
 * @param hops Pointer to an integer which returns the number of hops required
 * @param route Adress of an integer array which returns the vertex id's of the route
 * @return Success: 0.
 * @return Failure: -1.
 * @see freeGraph
 */
int trip(char* vertex1, char* vertex2, char** route, int* size);

/**
 * @brief Returns the Map identifier for the closest match of the vertex name
 * using <a href="https://en.wikipedia.org/wiki/Levenshtein_distance">
 * Levenshtein distance. </a>
 *
 * Usage:
 * @code
 * char* ver = NULL;
 *
 * ver = vertex("DC");
 * if(ver == NULL)
 *	printf("Vertex not found\n")
 *
 * @endcode
 * @param vertex Pointer to a character array which holds the name of the vertex
 * @return Success: Returns the pointer to vertex name.
 * @return If Vertex is not found: NULL.
 * @see freeGraph
 */
char* vertex(char* vertex);

/**
 * @brief Store the current state of the map into a text file in a custom csv format.
 *
 * Usage:
 * @code
 * store(FILENAME);
 * @endcode
 * @param filename Pointer to a character array which holds the filename.
 * @return Success: 0.
 * @return Failure: -1.
 * @see retrieve
 * @see freeGraph
 */
int store(char* filename);

/**
 * @brief Retrieves a Map saved into a file using store().
 *
 * Usage:
 * @code
 * retrieve(FILENAME);
 * @endcode
 * @param filename Pointer to a character array which holds the filename.
 * @return Success: 0.
 * @return Failure: -1.
 * @see store
 * @see freeGraph
 *
 * If there are errors while adding a certain object, an error message is
 * displayed, but the process continues until the whole file is parsed.
 */
int retrieve(char* filename);

/**
 * @brief Prints the Map info to stdout.
 *
 * Usage:
 * @code
 * printGraph();
 * @endcode
 */
void printGraph();

/**
 * @brief Call this API to clear all the data structures used by the Map.
 *
 * Usage:
 * @code
 * freeGraph();
 * @endcode
 */
void freeGraph();

#endif
