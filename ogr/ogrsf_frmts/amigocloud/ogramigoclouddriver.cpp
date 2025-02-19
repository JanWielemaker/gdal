/******************************************************************************
 *
 * Project:  AmigoCloud Translator
 * Purpose:  Implements OGRAMIGOCLOUDDriver.
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


extern "C" void RegisterOGRAmigoCloud();

/************************************************************************/
/*                        OGRAmigoCloudDriverIdentify()                    */
/************************************************************************/

static int OGRAmigoCloudDriverIdentify( GDALOpenInfo* poOpenInfo )
{
    return EQUALN(poOpenInfo->pszFilename, "AMIGOCLOUD:", strlen("AMIGOCLOUD:"));
}

/************************************************************************/
/*                           OGRAmigoCloudDriverOpen()                     */
/************************************************************************/

static GDALDataset *OGRAmigoCloudDriverOpen( GDALOpenInfo* poOpenInfo )

{
    if( !OGRAmigoCloudDriverIdentify(poOpenInfo) )
        return nullptr;

    OGRAmigoCloudDataSource   *poDS = new OGRAmigoCloudDataSource();

    if( !poDS->Open( poOpenInfo->pszFilename, poOpenInfo->papszOpenOptions,
                     poOpenInfo->eAccess == GA_Update ) )
    {
        delete poDS;
        poDS = nullptr;
    }

    return poDS;
}

/************************************************************************/
/*                      OGRAmigoCloudDriverCreate()                        */
/************************************************************************/

static GDALDataset *OGRAmigoCloudDriverCreate( const char * pszName,
                                               CPL_UNUSED int nBands,
                                               CPL_UNUSED int nXSize,
                                               CPL_UNUSED int nYSize,
                                               CPL_UNUSED GDALDataType eDT,
                                               CPL_UNUSED char **papszOptions )

{
    OGRAmigoCloudDataSource   *poDS = new OGRAmigoCloudDataSource();

    if( !poDS->Open( pszName, nullptr, TRUE ) )
    {
        delete poDS;
        return nullptr;
    }

    return poDS;
}

/************************************************************************/
/*                         RegisterOGRAMIGOCLOUD()                         */
/************************************************************************/

void RegisterOGRAmigoCloud()
{
    if( GDALGetDriverByName( "AmigoCloud" ) != nullptr )
        return;

    GDALDriver* poDriver = new GDALDriver();

    poDriver->SetDescription( "AmigoCloud" );
    poDriver->SetMetadataItem( GDAL_DMD_LONGNAME,  "AmigoCloud" );
    poDriver->SetMetadataItem( GDAL_DCAP_VECTOR, "YES" );
    poDriver->SetMetadataItem( GDAL_DCAP_CREATE_LAYER, "YES" );
    poDriver->SetMetadataItem( GDAL_DCAP_DELETE_LAYER, "YES" );
    poDriver->SetMetadataItem( GDAL_DMD_HELPTOPIC, "drivers/vector/amigocloud.html" );
    poDriver->SetMetadataItem( GDAL_DMD_CONNECTION_PREFIX, "AMIGOCLOUD:" );

    poDriver->SetMetadataItem( GDAL_DMD_OPENOPTIONLIST,
    "<OpenOptionList>"
    "  <Option name='AMIGOCLOUD_API_KEY' type='string' description='AmigoCLoud API token'/>"
    "  <Option name='OVERWRITE' type='boolean' description='Whether to overwrite an existing table without deleting it' default='NO'/>"
    "</OpenOptionList>");

    poDriver->SetMetadataItem( GDAL_DMD_CREATIONOPTIONLIST,
                               "<CreationOptionList/>");

    poDriver->SetMetadataItem( GDAL_DS_LAYER_CREATIONOPTIONLIST,
    "<LayerCreationOptionList>"
    "  <Option name='GEOMETRY_NULLABLE' type='boolean' description='Whether the values of the geometry column can be NULL' default='YES'/>"
    "</LayerCreationOptionList>");

    poDriver->SetMetadataItem( GDAL_DMD_CREATIONFIELDDATATYPES,
                               "String Integer Integer64 Real" );
    poDriver->SetMetadataItem( GDAL_DCAP_NOTNULL_FIELDS, "YES" );
    poDriver->SetMetadataItem( GDAL_DCAP_DEFAULT_FIELDS, "YES" );
    poDriver->SetMetadataItem( GDAL_DCAP_NOTNULL_GEOMFIELDS, "YES" );
    poDriver->SetMetadataItem( GDAL_DCAP_Z_GEOMETRIES, "YES" );

    poDriver->pfnOpen = OGRAmigoCloudDriverOpen;
    poDriver->pfnIdentify = OGRAmigoCloudDriverIdentify;
    poDriver->pfnCreate = OGRAmigoCloudDriverCreate;

    GetGDALDriverManager()->RegisterDriver( poDriver );
}
