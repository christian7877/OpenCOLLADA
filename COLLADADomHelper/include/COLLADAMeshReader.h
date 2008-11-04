/*
    Copyright (c) 2008 NetAllied Systems GmbH

    This file is part of COLLADADomHelper.

    Licensed under the MIT Open Source License, 
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/

#ifndef __COLLADA_MESH_READER_H__
#define __COLLADA_MESH_READER_H__

#include "HelperPrerequisites.h"
#include "COLLADAFWMesh.h"
#include "COLLADAReader.h"

#include "dom/domMesh.h"


namespace COLLADADomHelper
{

    /** Reads the geometry of the dom and creates a COLLADAFW::Mesh object. */
    class MeshReader : Reader
    {

    private:

        /** A pointer to the current mesh object to import. */
        domMeshRef mMeshRef;

        /** The COLLADAFramework mesh object. */
        COLLADAFW::Mesh* mMesh;

    public:

        /** Constructor. */
        MeshReader ( daeDocument* daeDoc ) : Reader ( *daeDoc ) {}

        /** Destructor. */
        virtual ~MeshReader () {}

        /** 
         * Reads the data of the domMesh and creates and fills a new  COLLADAFramework mesh object. 
         */
        COLLADAFW::Mesh* createMeshObject ( domMeshRef& meshRef );

    private:

        /**
        * Fills the COLLADAFramework name array, if the name array in the domSourceRef exists.
        * @param sourceRef Pointer to the domSourceRef.
        * @param source The COLLADAFramework name array to fill.
        */
        void fillNameArrayElement ( const domSourceRef& sourceRef, COLLADAFW::Source& source );
        void fillBoolArrayElement ( const domSourceRef& sourceRef, COLLADAFW::Source& source );
        void fillIntArrayElement ( const domSourceRef& sourceRef, COLLADAFW::Source& source );
        void fillFloatArrayElement ( const domSourceRef& sourceRef, COLLADAFW::Source& source );
        void fillIDREFArrayElement ( const domSourceRef& sourceRef, COLLADAFW::Source& source );

        /** Get the vertices input array (there is only one per mesh) */
        domInputLocal_Array fillVerticesInputArray ();

        /** One vertices input must specify semantic="POSITION" to establish the 
        topological identity of each vertex in the mesh. */
        domSourceRef getPositionsRef ();

        /** Create the mesh from the current polylist. */
        void fillMeshPolylistArray ( domPolylist_Array& polylistArray );

        /** Create the mesh from the current polygons array. */
        void createMeshFromPolygons ( domPolygons_Array& polygonsArray );

        /** Fill the array of vertex counts for each polygon. */
        void getVertexArray ( const domSourceRef positionsRef/*, MFloatPointArray &vertexArray*/ );

        /**
        * Fill the list with the count of vertices for every polygon and calculate 
        * the number of polygons and the sum of vertices for all polygons.
        * @param polylistRef Pointer to a polylist element in the collada document.
        * @param vertexCountsPerPolygon List of vertex counts per polygon.
        * @param numVertices Variable for the sum of all existing vertices in all polygons.
        */
        void getVertexCountsPerPolygon ( 
            const domPolylistRef polylistRef, 
            //MIntArray& vertexCountsPerPolygon, 
            size_t& numVertices );

        /**
        * Fill the list with the count of vertices for every polygon and calculate 
        * the number of polygons and the sum of vertices for all polygons.
        * @param polygonsRef Pointer to a polygons element in the collada document.
        * @param numInputElements The number of input elements in the current polygon element.
        * @param vertexCountsPerPolygon List of vertex counts per polygon.
        * @param numVertices Variable for the sum of all existing vertices in all polygons.
        */
        void getVertexCountsPerPolygon ( 
            const domPolygonsRef polygonsRef, 
            const size_t numInputElements, 
            //MIntArray& vertexCountsPerPolygon, 
            size_t& numVertices );

        /**
        * Get the vertex offset and the vertex set of the current polygons vertex input element.
        * Also establish the the maximum offset value of the current polygons input elements.
        * @param polylistRef Pointer to the current polylist element.
        * @param vertexOffset Variable for the vertex offset.
        * @param vertexSet Variable for the vertex set.
        * @param maxOffset Variable for the maximum offset value of the polygons input elements.
        */
        void getPolygonsOffsetValues ( 
            const domPolylistRef polylistRef, 
            size_t &vertexOffset, 
            size_t &vertexSet, 
            size_t &maxOffset );

        /**
        * Get the vertex offset and the vertex set of the current polygons vertex input element.
        * Also establish the the maximum offset value of the current polygons input elements.
        * @param polygonsRef Pointer to the current polygons element.
        * @param vertexOffset Variable for the vertex offset.
        * @param vertexSet Variable for the vertex set.
        * @param maxOffset Variable for the maximum offset value of the polygons input elements.
        */
        void getPolygonsOffsetValues ( 
            domPolygonsRef polygonsRef, 
            size_t& vertexOffset, 
            size_t& vertexSet, 
            size_t& maxOffset );

    };
}

#endif // __COLLADA_MESH_READER_H__