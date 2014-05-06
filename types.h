#ifndef _DROW_ENGINE_TYPES_H
#define _DROW_ENGINE_TYPES_H


/**
 * Standard file buffer
 */
#define STD_FILE_BUF 4096


typedef struct HE_edge HE_edge;
typedef struct HE_vert HE_vert;
typedef struct HE_face HE_face;

/**
 * Represents a half-edge.
 */
struct HE_edge {
	/**
	 * Start-vertex of the halfe-edge.
	 */
	HE_vert *vert;
	/**
	 * Oppositely oriented adjacent half-edge.
	 */
	HE_edge *pair;
	/**
	 * Face the half-edge borders.
	 */
	HE_face *face;
	/**
	 * Next half-edge around the face.
	 */
	HE_edge *next;
};

/**
 * Represents a vertex with a random attached
 * hald-edge.
 */
struct HE_vert {
	float x;
	float y;
	float z;

	/**
	 * One of the half-edges emanating from the vertex.
	 */
	HE_edge *edge;
};

/**
 * Represents a face
 */
struct HE_face {
	/**
	 * One of the half-edges bordering the face.
	 */
	HE_edge *edge;
};


#endif /* _DROW_ENGINE_TYPES_H */
