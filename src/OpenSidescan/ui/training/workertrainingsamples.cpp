﻿/*
* Copyright 2019 © Centre Interdisciplinaire de développement en Cartographie des Océans (CIDCO), Tous droits réservés
*/

 /*
 * \author Christian Bouchard
 */

#include <set>

#include <fstream>

#include <QDebug>


#include <QPixmap>

#include <QFileInfo>


#include "opencv2/core.hpp"
#include "../thirdParty/opencv/apps/createsamples/utility.hpp"

#include "utilities/qthelper.h"

#include "workertrainingsamples.h"


// For ordering the set
struct compare
{
    bool operator()( const std::pair<int,int> & lhs, const std::pair<int,int> & rhs ) const
    {

        if ( lhs.first > rhs.first )
            return false;

        if ( lhs.first < rhs.first )
            return true;

        // First is equal, look at second

        if ( lhs.second <= rhs.second )
            return true;
        else
            return false;
    }
};


WorkerTrainingSamples::WorkerTrainingSamples( Project * project, const int numberOfObjects,
                                              const ParameterscvCreateTrainingSamples & parameters,

                                              const QString & folderOriginalObjectImages,
                                              const QString & folderOutputPositiveSamples,
                                              const QString & folderBackground,

                                              std::ofstream & outFile,
                                              BoolWithMutex * continueWhatYourDoing  )
    : project( project ),
      numberOfObjects( numberOfObjects ),
      parameters( parameters ),
      folderOriginalObjectImages( folderOriginalObjectImages ),
      folderOutputPositiveSamples( folderOutputPositiveSamples ),
      folderBackground( folderBackground ),
      outFile( outFile ),
      continueWhatYourDoing( continueWhatYourDoing )
{

}


void WorkerTrainingSamples::computeMaxDimensions(SidescanFile & file){
    // j is an iterator to a (SidescanImage* )
    for(auto j=file.getImages().begin();j!=file.getImages().end();j++){

        // k is an iterator to (GeoreferencedObject *)
        for(auto k=(*j)->getObjects().begin();k!=(*j)->getObjects().end();k++){

            if ( (*k)->getPixelWidth() > maxObjectWidth ){
                maxObjectWidth = (*k)->getPixelWidth();
            }

            if ( (*k)->getPixelHeight() > maxObjectHeight ){
                maxObjectHeight = (*k)->getPixelHeight();
            }
        }
    }
}

void WorkerTrainingSamples::saveBackgrounds(SidescanFile & file){

    for(auto j=file.getImages().begin();j!=file.getImages().end();j++){

        if ( continueWhatYourDoing->getValue() == false ){
            emit done();
            return;
        }

        int imageOverallWidth = (*j)->getImage().cols;
        int imageOverallHeight = (*j)->getImage().rows;

        if ( imageOverallWidth < minBackgroundWidth  || imageOverallHeight < minBackgroundHeight) {
            continue;
        }

        if ( (*j)->getObjects().size() == 0 ) {

            // no objects, save the entire image as background
            saveBackgroundImage( *j, folderBackground, outFile, 0, imageOverallHeight - 1 );

        }
        else{
            // Image has objects

            std::vector< std::pair <int,int> > objectsVerticalPositions;

            computeObjectsVerticalOccupancy( *j, objectsVerticalPositions );

            int backgroundTop = 0;

            for ( unsigned int count = 0; count < objectsVerticalPositions.size(); count++ ){
                int backgroundBottom = objectsVerticalPositions[ count ].first - 1;

                if ( backgroundBottom - backgroundTop + 1 >= minBackgroundHeight) {
                    saveBackgroundImage( *j, folderBackground, outFile, backgroundTop, backgroundBottom );
                }

                backgroundTop = objectsVerticalPositions[ count ].second + 1;
            }

            // Background from the last object to the bottom of the overall image
            if ( backgroundTop < imageOverallHeight - 1
                    &&
                 ( imageOverallHeight - 1 ) - backgroundTop + 1 >= minBackgroundHeight )
            {
                saveBackgroundImage( *j, folderBackground, outFile, backgroundTop, imageOverallHeight - 1 );
            }

        }

    }


}

void WorkerTrainingSamples::saveSamples(SidescanFile & file){

    for(auto j=file.getImages().begin();j!=file.getImages().end();j++){

        for(auto k=(*j)->getObjects().begin();k!=(*j)->getObjects().end();k++){

            if ( continueWhatYourDoing->getValue() == false ) {
                emit done();
                return;
            }

            // Copy the part of the cv::Mat with the object into a new cv::Mat
            cv::Mat objectMat((*k)->getPixelHeight(),(*k)->getPixelWidth(),CV_8UC1);
            (*j)->getImage()( cv::Rect( (*k)->getX(), (*k)->getY(), (*k)->getPixelWidth(), (*k)->getPixelHeight() ) ).copyTo( objectMat );


            // Find filename that does not already exist
            QString objectName = QString::fromStdString( (*k)->getName() );

            QString fileExtension = "png";

            QString objectImageFileName = objectName + "." + fileExtension;

            QString objectImageFileNameWithPath = folderOriginalObjectImages
                                                    + "/" + objectImageFileName;

            QFileInfo fileInfo( objectImageFileNameWithPath );

            int count = 0;

            while ( fileInfo.exists() ) {

                objectImageFileName = objectName + "_" + QString::number( count ) + "." + fileExtension;
                objectImageFileNameWithPath = folderOriginalObjectImages
                                                + "/" + objectImageFileName;
                fileInfo.setFile( objectImageFileNameWithPath );
                count++;
            }

            // Save pixmap
            cv::imwrite(objectImageFileNameWithPath.toStdString(),objectMat);

            int showsamples = 0;
            int width;
            int height;

            cv::setRNGSeed( parameters.rngseed );


            if ( parameters.useOriginalObjectImageWidthAsBasis ) {
                width = (*k)->getPixelWidth() + parameters.nbPixelsChangeFromObjectImageWidth;

                if ( width <= 0 )
                    width = (*k)->getPixelWidth(); // TODO: display a warning?

            } else {
                width = parameters.width;
            }

            if ( parameters.useOriginalObjectImageHeightAsBasis ) {
                height = (*k)->getPixelHeight() + parameters.nbPixelsChangeFromObjectImageHeight;

                if ( height <= 0 )
                    height = (*k)->getPixelHeight(); // TODO: display a warning?

            } else {
                height = parameters.height;
            }



            std::string imagenameString = objectImageFileNameWithPath.toStdString();
            std::string bgfilenameString = std::string( folderBackground.toStdString() + "/bg.txt" );
//                std::string vecnameString = std::string( objectImageFileNameWithPath.toStdString() + ".vec" );
            std::string vecnameString = std::string(  ( folderOutputPositiveSamples + "/" + objectImageFileName
                                                        + QString::number( width ) + "x" + QString::number( height )
                                                        + ".vec" ).toStdString() );



            const char * imagename = imagenameString.c_str();
            const char * bgfilename = bgfilenameString.c_str();
            const char * vecname = vecnameString.c_str();

            cvCreateTrainingSamples( vecname, imagename,
                                     parameters.bgcolor,
                                     parameters.bgthreshold, bgfilename,
                                     parameters.num,
                                     parameters.invert,
                                     parameters.maxintensitydev,
                                     parameters.maxxangle,
                                     parameters.maxyangle,
                                     parameters.maxzangle,
                                     showsamples, width, height );

            countObjects++;
            progress( countObjects + 2 );
        }
    }
}

void WorkerTrainingSamples::doWork() {

    if ( continueWhatYourDoing->getValue() == false ) {
        emit done();
        return;
    }

    // Find maximum height and width for positive sample objects

    maxObjectWidth = 0;
    maxObjectHeight = 0;

    project->walkFiles(this,&WorkerTrainingSamples::computeMaxDimensions);

    // Compute min background width and height based on object size
    // and ParameterscvCreateTrainingSamples parameters

    minBackgroundWidth = 0;
    minBackgroundHeight = 0;

    if ( parameters.useOriginalObjectImageWidthAsBasis ) {
        minBackgroundWidth = maxObjectWidth + parameters.nbPixelsChangeFromObjectImageWidth;

        if ( minBackgroundWidth <= 0 )
            minBackgroundWidth = maxObjectWidth; // Display warning?

    } else {
        minBackgroundWidth = parameters.width;
    }

    if ( parameters.useOriginalObjectImageHeightAsBasis ) {
        minBackgroundHeight = maxObjectHeight + parameters.nbPixelsChangeFromObjectImageHeight;

        if ( minBackgroundHeight <= 0 )
            minBackgroundHeight = maxObjectHeight; // Display warning?

    } else {
        minBackgroundHeight = parameters.height;
    }



    if ( continueWhatYourDoing->getValue() == false ) {
        emit done();
        return;
    }

    progress( 1 );


    // Go through each image and save images of background region large enough
    project->walkFiles(this,&WorkerTrainingSamples::saveBackgrounds);


    if ( continueWhatYourDoing->getValue() == false )
    {
        emit done();
        return;
    }

    progress( 2 );


    // Save samples

    countObjects = 0;

    project->walkFiles(this,&WorkerTrainingSamples::saveSamples);

    emit done();
}


void WorkerTrainingSamples::computeObjectsVerticalOccupancy( SidescanImage * image, std::vector< std::pair <int,int> > & verticalPositions)
{

    std::multiset< std::pair<int,int>, compare> set;

    std::pair <int,int> beginEnd;

    // k is an iterator to (GeoreferencedObject *)
    for( auto k = image->getObjects().begin(); k != image->getObjects().end(); k++ ){
        beginEnd = std::make_pair( (*k)->getY(), (*k)->getY() + (*k)->getPixelHeight() -1 );
        set.insert( beginEnd );
    }

    // Combine object overlaps
    verticalPositions.clear();

    unsigned long setSize = set.size();

    // TODO: check if the size of the set is zero

    int beginning = set.begin()->first;
    int end = set.begin()->second;

    std::multiset< std::pair <int,int> >::iterator it = set.begin();
    it++;

    for ( unsigned int count = 1; count < setSize; count++ )
    {
        if ( end >= it->first - 1 ) // With integers, if one object ends at 100 and the next object starts at 101, assume they are continuous
        {
            end = it->second;
        }
        else
        {
            std::pair <int,int> newPair = std::make_pair( beginning, end );

            verticalPositions.push_back( newPair );

            beginning = it->first;
            end = it->second;
        }

        ++it;
    }

    std::pair <int,int> newPair = std::make_pair( beginning, end );
    verticalPositions.push_back( newPair );
}


void WorkerTrainingSamples::saveBackgroundImage( SidescanImage * image,
                                                 const QString & folder,
                                                 std::ofstream & outFile,
                                                int backgroundTop, int backgroundBottom )
{
    int height = backgroundBottom - backgroundTop + 1;

    // Copy the part of the cv::Mat with the object into a new cv::Mat
    cv::Mat objectMat(height,image->getImage().cols,CV_8UC1);
    image->getImage()( cv::Rect( 0, backgroundTop, image->getImage().cols, height ) ).copyTo( objectMat );

    // Find filename that does not already exist
    QString objectName = QString::fromStdString( "Background" );

    QString fileExtension = "png";

    QString objectImageFileName = objectName + "." + fileExtension;

    QString objectImageFileNameWithPath = folder + "/" + objectImageFileName;

    QFileInfo fileInfo( objectImageFileNameWithPath );

    int count = 0;

    while ( fileInfo.exists() ) {

        objectImageFileName = objectName + "_" + QString::number( count ) + "." + fileExtension;
        objectImageFileNameWithPath = folder + "/" + objectImageFileName;

        fileInfo.setFile( objectImageFileNameWithPath );

        count++;
    }

    // Save pixmap
    cv::imwrite(objectImageFileNameWithPath.toStdString(),objectMat);

    if ( outFile.is_open() ) {
        // Image's path written in the file must be relative to outFile's location
        outFile << objectImageFileName.toStdString() << std::endl;
    }
}
