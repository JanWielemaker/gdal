/******************************************************************************
 *
 * Project:  AmigoCloud Translator
 * Purpose:  Implements OGRAmigoCloudResultLayer class.
 * Author:   Victor Chernetsky, <victor at amigocloud dot com>
 *
 ******************************************************************************
 * Copyright (c) 2015, Victor Chernetsky, <victor at amigocloud dot com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include "ogr_amigocloud.h"


/************************************************************************/
/*                          OGRAmigoCloudResultLayer()                     */
/************************************************************************/

OGRAmigoCloudResultLayer::OGRAmigoCloudResultLayer(
    OGRAmigoCloudDataSource* poDSIn,
    const char * pszRawQueryIn ) :
    OGRAmigoCloudLayer(poDSIn)
{
    osBaseSQL = pszRawQueryIn;
    SetDescription( "result" );
    poFirstFeature = nullptr;
}

/************************************************************************/
/*                       ~OGRAmigoCloudResultLayer()                       */
/************************************************************************/

OGRAmigoCloudResultLayer::~OGRAmigoCloudResultLayer()

{
    delete poFirstFeature;
}

/************************************************************************/
/*                          GetLayerDefnInternal()                      */
/************************************************************************/

OGRFeatureDefn * OGRAmigoCloudResultLayer::GetLayerDefnInternal(json_object* poObjIn)
{
    if( poFeatureDefn != nullptr )
        return poFeatureDefn;

    EstablishLayerDefn("result", poObjIn);

    return poFeatureDefn;
}

/************************************************************************/
/*                           GetNextRawFeature()                        */
/************************************************************************/

OGRFeature  *OGRAmigoCloudResultLayer::GetNextRawFeature()
{
    if( poFirstFeature )
    {
        OGRFeature* poRet = poFirstFeature;
        poFirstFeature = nullptr;
        return poRet;
    }
    else
        return OGRAmigoCloudLayer::GetNextRawFeature();
}

/************************************************************************/
/*                                IsOK()                                */
/************************************************************************/

int  OGRAmigoCloudResultLayer::IsOK()
{
    CPLErrorReset();
    poFirstFeature = GetNextFeature();
    return CPLGetLastErrorType() == 0;
}

/************************************************************************/
/*                             GetSRS_SQL()                             */
/************************************************************************/

CPLString OGRAmigoCloudResultLayer::GetSRS_SQL(const char* pszGeomCol)
{
    CPLString osSQL;
    CPLString osLimitedSQL;

    size_t nPos = osBaseSQL.ifind(" LIMIT ");
    if( nPos != std::string::npos )
    {
        osLimitedSQL = osBaseSQL;
        size_t nSize = osLimitedSQL.size();
        for(size_t i = nPos + strlen(" LIMIT "); i < nSize; i++)
        {
            if( osLimitedSQL[i] == ' ' && osLimitedSQL[i-1] == '0')
            {
                osLimitedSQL[i-1] = '1';
                break;
            }
            osLimitedSQL[i] = '0';
        }
    }
    else
        osLimitedSQL.Printf("%s LIMIT 1", osBaseSQL.c_str());

    /* Assuming that the SRID of the first non-NULL geometry applies */
    /* to geometries of all rows. */
    osSQL.Printf("SELECT srid, srtext FROM spatial_ref_sys WHERE srid IN "
                "(SELECT ST_SRID(%s) FROM (%s) ogr_subselect)",
                OGRAMIGOCLOUDEscapeIdentifier(pszGeomCol).c_str(),
                osLimitedSQL.c_str());

    return osSQL;
}
