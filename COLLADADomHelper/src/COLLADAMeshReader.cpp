/*
    Copyright (c) 2008 NetAllied Systems GmbH

    This file is part of COLLADADomHelper.

    Licensed under the MIT Open Source License, 
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/

#include "HelperStableHeaders.h"
#include "COLLADAMeshReader.h"

#include "COLLADASWConstants.h"

#include "dom/domName_array.h"


namespace COLLADADomHelper
{

    // --------------------------------------------
    COLLADAFW::Mesh* MeshReader::createMeshObject ( domMeshRef& meshRef )
    {
        // Set the current domMesh.
        mMeshRef = meshRef;

        // Create a COLLADAFramework::Mesh element with 
        // the dom from the collada document data.
        mMesh = new COLLADAFW::Mesh ();

        // TODO Fill the mesh source elements
        domSource_Array domSourceArray = mMeshRef->getSource_array ();
        size_t sourceArraySize = domSourceArray.getCount ();

        // Create a new source array with the required size
        COLLADAFW::SourceArray sourceArray;
        sourceArray = new COLLADAFW::Source [ sourceArraySize ];

        for ( size_t n=0; n<sourceArraySize; ++n )
        {
            // The dom source element
            domSourceRef sourceRef = domSourceArray.get ( n );
            xsNCName sourceName = sourceRef->getName ();
            xsID sourceId = sourceRef->getId ();

            // The current source array to fill in.
            COLLADAFW::Source source = sourceArray [ n ];

            // Fill the array element of the existing type.
            fillNameArrayElement ( sourceRef, source );
            fillIntArrayElement ( sourceRef, source );
            fillFloatArrayElement ( sourceRef, source );
            fillBoolArrayElement ( sourceRef, source );
            fillIDREFArrayElement ( sourceRef, source );

            domSource::domTechnique_commonRef techniqueCommonRef;
            techniqueCommonRef = sourceRef->getTechnique_common ();

            domAccessorRef accessorRef = techniqueCommonRef->getAccessor ();
            accessorRef->getStride ();
            accessorRef->getSource ();
            accessorRef->getOffset ();
            accessorRef->getCount ();

            domTechnique_Array techniqueArray = sourceRef->getTechnique_array ();
            size_t techniqueCount = techniqueArray.getCount ();

        }


        // TODO Fill the mesh vertex element.
        mMeshRef->getVertices ();

        // Get the array of polylists and write it in the COLLADAFW::Mesh object.
        domPolylist_Array polylistArray = mMeshRef->getPolylist_array ();
        if ( polylistArray.getCount () != 0 )
        {
            fillMeshPolylistArray ( polylistArray );
        }

        // Get the array of polygons and write it in the COLLADAFW::Mesh object.
        domPolygons_Array polygonsArray = mMeshRef->getPolygons_array ();
        if ( polygonsArray.getCount () != 0 )
        {
            createMeshFromPolygons (  polygonsArray  );
        }

        // TODO
        meshRef->getTriangles_array ();
        meshRef->getTrifans_array ();
        meshRef->getTristrips_array ();

        return mMesh;
    }

    // --------------------------------------------
    void MeshReader::fillMeshPolylistArray ( domPolylist_Array& polylistArray ) 
    {
        // Fill the COLLADAFramework data

        // The number of vertices and polygons.
        size_t numVertices = 0, numPolygons = 0;

        // One vertices input must specify semantic="POSITION" to establish the 
        // topological identity of each vertex in the mesh.
        domSourceRef positionsRef = getPositionsRef ();

        domFloat_arrayRef positionsArray = positionsRef->getFloat_array ();
        domListOfFloats positions = positionsArray->getValue ();
        domAccessorRef posAccessorRef = positionsRef->getTechnique_common ()->getAccessor ();
        domUint posAccessorCount = posAccessorRef->getCount ();
        domUint posAccessorStride = posAccessorRef->getStride ();

        // The number of polylist elements in the current mesh
        size_t polylistCount = polylistArray.getCount ();

        // Fill the COLLADAFramework polylist data
        COLLADAFW::Polylist* polylistList = new COLLADAFW::Polylist [ polylistCount ]; 
        mMesh->setPolylistArray ( polylistList, polylistCount );

        for ( size_t i=0; i<polylistCount; ++i )
        {
            // Create the polylist element.
            COLLADAFW::Polylist polylist;

            // Push the polylist element in the list of polylists.
            polylistList [ i ] = polylist;

            // Get the current polylist element
            domPolylistRef polylistRef = polylistArray.get ( i );
            numPolygons = ( size_t ) polylistRef->getCount ();
            polylist.setCount ( numPolygons );

            // Assign the material.
            xsNCName materialName = polylistRef->getMaterial ();
            polylist.setMaterial ( materialName );

            // Fill the COLLADAFramework pList data
            domPRef pRef = polylistRef->getP ();
            domListOfUInts domPList = pRef->getValue ();
            size_t pListSize = domPList.getCount ();
            COLLADAFW::Polylist::PArray pList = new int [ pListSize ];
            for ( size_t m=0; m<pListSize; ++m )
            {
                pList [ m ] = ( int ) domPList.get ( m );
            }
            polylist.setPArray ( pList, pListSize );

            // Fill the COLLADAFramework vCountList data
            domPolylist::domVcountRef vCountRef = polylistRef->getVcount ();
            domListOfUInts domVCountList = vCountRef->getValue ();
            size_t vCountListSize = domVCountList.getCount ();
            COLLADAFW::Polylist::VCountArray vCountList = new int [ vCountListSize ];
            for ( size_t m=0; m<vCountListSize; ++m )
            {
                vCountList [ m ] = ( int ) domVCountList.get ( m );
            }
            polylist.setVCountArray ( vCountList, vCountListSize );

            // TODO Fill the COLLADAFramework input list elements.
            domInputLocalOffset_Array polylistInputArray;
            polylistInputArray = polylistRef->getInput_array ();
            size_t polylistInputCount = polylistInputArray.getCount ();

            COLLADAFW::InputArray inputArray = new COLLADAFW::Input [ polylistInputCount ];
            polylist.setInputArray ( inputArray, polylistInputCount );

            for ( size_t n=0; n<polylistInputCount; ++n )
            {
                // Get the current COLLADAFramework input element.
                COLLADAFW::Input input = inputArray [ n ];

                // Get the current dom input element.
                domInputLocalOffsetRef polylistInputRef;
                polylistInputRef = polylistInputArray.get ( n );

                xsNMTOKEN semantic = polylistInputRef->getSemantic ();
                input.setSemantic ( COLLADAFW::Input::getSemanticFromString ( semantic ) );
                domURIFragmentType domUri = polylistInputRef->getSource ();
                input.setSource ( COLLADASW::URI::nativePathToUri ( domUri.originalStr () ) );
                unsigned int offset = ( unsigned int ) polylistInputRef->getOffset ();
                input.setOffset ( offset );
                unsigned int set = ( unsigned int ) polylistInputRef->getSet ();
                input.setSet ( set );
            }
        }
    }

    // --------------------------------------------
    domInputLocal_Array MeshReader::fillVerticesInputArray ()
    {
        // Get the vertices (there is only one per mesh!)
        domVerticesRef verticesRef = mMeshRef->getVertices ();

        xsNCName verticesName = verticesRef->getName ();
        //mMesh->set
        xsID verticesId = verticesRef->getID ();

        domInputLocal_Array verticesInputArray = verticesRef->getInput_array ();

        return verticesRef->getInput_array ();
    }

    // --------------------------------------------
    domSourceRef MeshReader::getPositionsRef ()
    {
        // One vertices input must specify semantic="POSITION" to establish the 
        // topological identity of each vertex in the mesh.

        // Get the vertices input array (there is only one per mesh).
        domInputLocal_Array verticesInputArray = fillVerticesInputArray ();

        // Get the point positions of the vertexes
        domURIFragmentType positionUri ( * ( mDoc.getDAE () ) );
        size_t numOfInputs = verticesInputArray.getCount ();
        for ( size_t j=0; j<numOfInputs; ++j )
        {
            domInputLocalRef vertexInputRef = verticesInputArray.get ( j );
            xsNMTOKEN semantic = vertexInputRef->getSemantic ();
            if ( COLLADASW::Utils::equalsIgnoreCase ( semantic, COLLADASW::CSWC::CSW_SEMANTIC_POSITION ) )
            {
                positionUri = vertexInputRef->getSource ();
            }
        }
        if ( positionUri.getState() != daeURI::uri_success )
        {
            positionUri.setContainer ( mDoc.getDomRoot() );
            positionUri.resolveElement ();
        }

        return daeSafeCast<domSource> ( positionUri.getElement() );
    }

    // --------------------------------------------
    void MeshReader::createMeshFromPolygons ( domPolygons_Array& polygonsArray )
    {
        // Fill the required attributes to create the current mesh.

        // The number of vertices and polygons.
        size_t numVertices = 0, numPolygons = 0;

        // One vertices input must specify semantic="POSITION" to establish the 
        // topological identity of each vertex in the mesh.
        domSourceRef positionsRef = getPositionsRef ();

        // Go through all poygons of the current mesh.
        size_t numPolygonsArrays = polygonsArray.getCount ();
        for ( size_t i=0; i<numPolygonsArrays; ++i )
        {
            // Get a reference to the current polygons element.
            domPolygonsRef polygonsRef = polygonsArray.get ( i );
            numPolygons = ( size_t ) polygonsRef->getCount ();

            // TODO Assign the material.
            xsNCName materialName = polygonsRef->getMaterial ();

            // Fill the array of vertex counts for each polygon.
            getVertexArray ( positionsRef );

            // Get the polylists offset of the vertices 
            // and the maximum offset in the polygons list.
            size_t vertexOffset, vertexSet, maxOffset = 0;
            getPolygonsOffsetValues ( polygonsRef, vertexOffset, vertexSet, maxOffset );

            // Fill the list with the count of vertices for every polygon and 
            // calculate the number of polygons and the sum of vertices for all polygons.
            getVertexCountsPerPolygon ( polygonsRef, maxOffset, 
                numVertices );

        }

    }

    // --------------------------------------------
    void MeshReader::getVertexCountsPerPolygon ( 
        const domPolygonsRef polygonsRef, 
        const size_t numInputElements, 
        size_t& numVertices )
    {
        // Contains a list of polygon lists. Each polygon list contains a list of UInts 
        // that specifies the vertex attributes (indices) for an individual polygon.
        domP_Array pArray = polygonsRef->getP_array ();

        // Get the number of polygons in the current polygons element.
        size_t numPolygons = pArray.getCount ();

        // Go through each polygon and count the vertices 
        // per polygon and the sum of all vertices.
        for ( size_t n=0; n<numPolygons; ++n )
        {
            // Get the list of vertices of the current polygon.
            domPRef pRef = pArray.get ( n );
            domListOfUInts pValues = pRef->getValue ();

            // Get the number of vertices of the current polygon
            size_t numPolygonVertices = pValues.getCount () / numInputElements;

            // Count the number of all vertices.
            numVertices += numPolygonVertices;

//             // Write the number of vertices of the current polygon in the list.
//             vertexCountsPerPolygon.append ( ( int ) numPolygonVertices );
        }
    }

    // --------------------------------------------
    void MeshReader::getVertexCountsPerPolygon ( 
        const domPolylistRef polylistRef, 
//        MIntArray& vertexCountsPerPolygon, 
        size_t& numVertices )
    {
        // Contains a list of integers, each specifying the number of
        // vertices for one polygon described by the <polylist> element.
        domPolylist::domVcountRef vcountRef = polylistRef->getVcount ();
        domListOfUInts vCountList = vcountRef->getValue ();

        // Get the number of polygons in the current polylist.
        size_t numPolygons = vCountList.getCount ();

        // Go through each polygon and count the vertices 
        // per polygon and the sum of all vertices.
        for ( size_t n=0; n<numPolygons; ++n )
        {
            // Get the number of vertices of the current polygon
            size_t numPolygonVertices = ( size_t ) vCountList.get ( n );

            // Count the number of all vertices.
            numVertices += numPolygonVertices;

//             // Write the number of vertices of the current polygon in the list.
//             vertexCountsPerPolygon.append ( (int) numPolygonVertices );
        }
    }

    // --------------------------------------------
    void MeshReader::getPolygonsOffsetValues ( 
        const domPolygonsRef polygonsRef, 
        size_t& vertexOffset, 
        size_t& vertexSet, 
        size_t& maxOffset )
    {
        domInputLocalOffset_Array polygonsInputArray;
        polygonsInputArray = polygonsRef->getInput_array ();
        size_t polylistInputCount = polygonsInputArray.getCount ();
        for ( size_t n=0; n<polylistInputCount; ++n )
        {
            domInputLocalOffsetRef polygonsInputRef;
            polygonsInputRef = polygonsInputArray.get ( n );
            size_t currentOffset = ( size_t ) polygonsInputRef->getOffset ();
            xsNMTOKEN semantic = polygonsInputRef->getSemantic ();
            if ( COLLADASW::Utils::equalsIgnoreCase ( semantic, COLLADASW::CSWC::CSW_SEMANTIC_VERTEX ) )
            {
                vertexOffset = currentOffset;
                vertexSet = ( size_t ) polygonsInputRef->getSet ();
            }
            if ( currentOffset > maxOffset ) maxOffset = currentOffset; 
        }
    }

    // --------------------------------------------
    void MeshReader::getPolygonsOffsetValues ( 
        const domPolylistRef polylistRef, 
        size_t &vertexOffset, 
        size_t &vertexSet, 
        size_t &maxOffset )
    {
        domInputLocalOffset_Array polylistInputArray;
        polylistInputArray = polylistRef->getInput_array ();
        size_t polylistInputCount = polylistInputArray.getCount ();
        for ( size_t n=0; n<polylistInputCount; ++n )
        {
            domInputLocalOffsetRef polylistInputRef;
            polylistInputRef = polylistInputArray.get ( n );
            size_t currentOffset = ( size_t ) polylistInputRef->getOffset ();
            xsNMTOKEN semantic = polylistInputRef->getSemantic ();
            if ( COLLADASW::Utils::equalsIgnoreCase ( semantic, COLLADASW::CSWC::CSW_SEMANTIC_VERTEX ) )
            {
                vertexOffset = currentOffset;
                vertexSet = ( size_t ) polylistInputRef->getSet ();
            }
            if ( currentOffset > maxOffset ) maxOffset = currentOffset; 
        }
    }

    // --------------------------------------------
    void MeshReader::getVertexArray ( 
        const domSourceRef positionsRef )
    {
        // Get informations about the current positions array.
        domFloat_arrayRef positionsArray = positionsRef->getFloat_array ();
        domListOfFloats positions = positionsArray->getValue ();
        domAccessorRef posAccessorRef = positionsRef->getTechnique_common ()->getAccessor ();
        domUint posAccessorCount = posAccessorRef->getCount ();
        domUint posAccessorStride = posAccessorRef->getStride ();

        // This should include all the vertices in the mesh, and no extras.
        size_t positionIndex = 0;
        for ( size_t n=0; n<posAccessorCount; ++n )
        {
            domFloat one, two, three;
            // TODO different accessor strides!
            if ( posAccessorStride == 3 ) 
            {
                positions.get3at ( positionIndex, one, two, three );
                //vertexArray.append ( (float) one, (float) two, (float) three, 0.0f );
                positionIndex += ( size_t ) posAccessorStride;
            }
        }
    }

    // --------------------------------------------
    void MeshReader::fillNameArrayElement ( const domSourceRef& sourceRef, COLLADAFW::Source& source )
    {
        domName_arrayRef nameArrayRef = sourceRef->getName_array ();
        if ( nameArrayRef != 0 )
        {
            COLLADAFW::NameArrayElement arrayElement;
            arrayElement.setArrayCount ( nameArrayRef->getCount () );
            arrayElement.setArrayId ( nameArrayRef->getId () );
            arrayElement.setArrayName ( nameArrayRef->getName () );

            domListOfNames domNameValues = nameArrayRef->getValue ();
            size_t nameCount = domNameValues.getCount ();
            String* nameArray = new String [ nameCount ];
            for ( size_t m=0; m<nameCount; ++m )
            {
                domName& name = domNameValues.get ( m );
                nameArray [ m ] = name;
            }
            arrayElement.setArrayValues ( nameArray, nameCount );

            source.setNameArrayElement ( arrayElement );
        }
    }

    // --------------------------------------------
    void MeshReader::fillBoolArrayElement ( const domSourceRef& sourceRef, COLLADAFW::Source& source )
    {
        domBool_arrayRef arrayRef = sourceRef->getBool_array ();
        if ( arrayRef != 0 )
        {
            COLLADAFW::BoolArrayElement arrayElement;
            arrayElement.setArrayCount ( arrayRef->getCount () );
            arrayElement.setArrayId ( arrayRef->getId () );
            arrayElement.setArrayName ( arrayRef->getName () );

            domListOfBools domValues = arrayRef->getValue ();
            size_t valuesCount = domValues.getCount ();
            bool* valuesArray = new bool [ valuesCount ];
            for ( size_t m=0; m<valuesCount; ++m )
            {
                domBool& val = domValues.get ( m );
                valuesArray [ m ] = val;
            }
            arrayElement.setArrayValues ( valuesArray, valuesCount );

            source.setBoolArrayElement ( arrayElement );
        }
    }

    // --------------------------------------------
    void MeshReader::fillIntArrayElement ( const domSourceRef& sourceRef, COLLADAFW::Source& source )
    {
        domInt_arrayRef arrayRef = sourceRef->getInt_array ();
        if ( arrayRef != 0 )
        {
            COLLADAFW::IntArrayElement arrayElement;
            arrayElement.setArrayCount ( arrayRef->getCount () );
            arrayElement.setArrayId ( arrayRef->getId () );
            arrayElement.setArrayName ( arrayRef->getName () );

            domListOfInts domValues = arrayRef->getValue ();
            size_t valuesCount = domValues.getCount ();
            int* valuesArray = new int [ valuesCount ];
            for ( size_t m=0; m<valuesCount; ++m )
            {
                domInt& val = domValues.get ( m );
                valuesArray [ m ] = val;
            }
            arrayElement.setArrayValues ( valuesArray, valuesCount );

            source.setIntArrayElement ( arrayElement );
        }
    }

    // --------------------------------------------
    void MeshReader::fillFloatArrayElement ( const domSourceRef& sourceRef, COLLADAFW::Source& source )
    {
        domFloat_arrayRef arrayRef = sourceRef->getFloat_array ();
        if ( arrayRef != 0 )
        {
            COLLADAFW::FloatArrayElement arrayElement;
            arrayElement.setArrayCount ( arrayRef->getCount () );
            arrayElement.setArrayId ( arrayRef->getId () );
            arrayElement.setArrayName ( arrayRef->getName () );

            domListOfFloats domValues = arrayRef->getValue ();
            size_t valuesCount = domValues.getCount ();
            float* valuesArray = new float [ valuesCount ];
            for ( size_t m=0; m<valuesCount; ++m )
            {
                domFloat& val = domValues.get ( m );
                valuesArray [ m ] = val;
            }
            arrayElement.setArrayValues ( valuesArray, valuesCount );

            source.setFloatArrayElement ( arrayElement );
        }
    }

    // --------------------------------------------
    void MeshReader::fillIDREFArrayElement ( const domSourceRef& sourceRef, COLLADAFW::Source& source )
    {
        domIDREF_arrayRef arrayRef = sourceRef->getIDREF_array ();
        if ( arrayRef != 0 )
        {
            COLLADAFW::IDREFArrayElement arrayElement;
            arrayElement.setArrayCount ( arrayRef->getCount () );
            arrayElement.setArrayId ( arrayRef->getId () );
            arrayElement.setArrayName ( arrayRef->getName () );

            xsIDREFS domValues = arrayRef->getValue ();
            size_t valuesCount = domValues.getCount ();
            String* valuesArray = new String [ valuesCount ];
            for ( size_t m=0; m<valuesCount; ++m )
            {
                daeIDRef& val = domValues.get ( m );
                // TODO Is this that i want to have???
                valuesArray [ m ] = val.getID ();
            }
            arrayElement.setArrayValues ( valuesArray, valuesCount );

            source.setIDREFArrayElement ( arrayElement );
        }
    }

}