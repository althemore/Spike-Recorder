//
//  HDFRecorder.cpp
//  SpikeRecorder
//
//  Created by Stanislav on 06/05/2022.
//  Copyright © 2022 BackyardBrains. All rights reserved.
//

#include "HDFRecorder.h"
#include "WavTxtRecorder.h"

#include <iostream>
#include <string>




const H5std_string FILE_NAME("h5tutr_dset.h5");
const H5std_string DATASET_NAME_AUDIO("signal");
const H5std_string DATASET_NAME_EVENTS("events");

namespace BackyardBrains {

    HDFRecorder::HDFRecorder(RecordingManager &manager): _manager(manager) {
       
        fileHandle = 0; 
        
    }

    HDFRecorder::~HDFRecorder() {
       
    }

    void HDFRecorder::initBuffer(int sampleRate, int numberOfChannels)
    {
        mainBuffer = new short[sampleRate];
    }

    bool HDFRecorder::start(const std::string &filename) {
        
        //get position of buffer
        _oldPos = _manager.pos();
        _startPos = _oldPos;
        
        int i, _nchan;
        
        size_t dotpos = filename.find_last_of('.');
        _filename = filename.substr(0,dotpos) + ".h5";
        
        _nchan = 0;
        for( i = 0; i < _manager.virtualDevices().size(); i++) {
            if(_manager.virtualDevices()[i].bound)
                _nchan++;
        }

        _numberOfChannels = _nchan;
        initBuffer(_manager.sampleRate(), _nchan);
        // Create a new file. If file exists its contents will be overwritten.
        fileHandle = new H5File(_filename.c_str(), H5F_ACC_TRUNC);

        
        
        //Create the data space with unlimited dimensions.
        int RANK = 2;
        hsize_t   dims[2]    = {1, 1}; // dataset dimensions at creation
        dims[0] = _numberOfChannels;
        
        hsize_t   maxdims[2] = {1, H5S_UNLIMITED};
        maxdims[0] = _numberOfChannels;
        DataSpace dataSpaceDefinition(RANK, dims, maxdims);

        //Modify dataset creation properties, i.e. enable chunking.
        DSetCreatPropList cparms;

        //set chunk size
        hsize_t chunk_dims[2];
        chunk_dims[0] = _numberOfChannels;
        chunk_dims[1] = _manager.sampleRate();
        cparms.setChunk(RANK, chunk_dims);

        // Set fill value
        short fill_val = 0;
        cparms.setFillValue(PredType::NATIVE_SHORT, &fill_val);

        // Create a new dataset within the file using cparms creation properties.
        audioDataset = new DataSet(fileHandle->createDataSet(DATASET_NAME_AUDIO, PredType::NATIVE_SHORT, dataSpaceDefinition, cparms));
        
        
        //eventDataset = new DataSet(fileHandle->createDataSet(DATASET_NAME_EVENTS, PredType::NATIVE_SHORT, dataSpaceDefinition, cparms));
        
        
        return true;
    }

    void HDFRecorder::stop(const MetadataChunk *meta) {
        

        
        delete audioDataset;
        delete fileHandle;
        //delete eventDataset;
        audioDataset = 0;
        fileHandle = 0;
        delete[] mainBuffer;
        //meta->markers
   /*     std::list<std::pair<std::string, int64_t> >::const_iterator it;
        for(it = meta->markers.begin(); it != meta->markers.end(); it++)
        {
            if(it->second - _startPos >= 0)
            {
                // it->first.c_str()
                //(it->second-_startPos)/(float)_manager.sampleRate());
                edfwrite_annotation_latin1(currentHandle, (int)(((it->second-_startPos)/(float)_manager.sampleRate())*10000), -1, it->first.c_str());
            }
        }
        edfclose_file(currentHandle);
        currentHandle=-1;
        delete[] mainBuffer;*/
    }



    const std::string &HDFRecorder::filename() const {
        return _filename;
    }

    std::string HDFRecorder::eventTxtFilename(const std::string &filename) {

        return filename;
    }

    int HDFRecorder::writeMarkerTextFile(const std::string &filename, const std::list<std::pair<std::string, int64_t> > &markers) const {
        return 0;
    }

    void HDFRecorder::parseMarkerTextFile(std::list<std::pair<std::string, int64_t> > &markers, const std::string &filename, int sampleRate)
    {
        
    }

    int HDFRecorder::parseMetadataStr(MetadataChunk *meta, const char *str, RecordingManager &manager) {
        return 0;
    }

    bool HDFRecorder::recording() const {
        return ((long)fileHandle)>0;
    }

    float HDFRecorder::recordTime() const {
        return ((float)(_manager.pos()  - _startPos))/(float)_manager.sampleRate();
    }

    void HDFRecorder::advance()
    {
       
        if(!recording())
            return;

        int64_t len = _manager.pos() - _oldPos;

        if(len<_manager.sampleRate()) {
            return;
        }
        DataSpace currentSpace = audioDataset->getSpace();
        int RANK = 2;
        hsize_t currentSize[RANK];

         currentSpace.getSimpleExtentDims(currentSize);
         hsize_t lenghtOfExistingData = currentSize[1];
         hsize_t size[RANK];
         size[0] = _numberOfChannels;
         size[1] = lenghtOfExistingData+ _manager.sampleRate();
         audioDataset->extend(size);
        
        
        for(unsigned int i = 0; i < _manager.virtualDevices().size(); i++) {
            if(_manager.virtualDevices()[i].bound) {
                _manager.getData(i, _oldPos, _manager.sampleRate(),mainBuffer);
                //this always records one second
                //edfwrite_digital_short_samples(currentHandle, mainBuffer);
                
                
                
                hsize_t dims1[RANK]; // data1 dimensions
                dims1[0] = 1;
                dims1[1] = _manager.sampleRate();
                DataSpace memDataSpace(RANK, dims1);
                // Select a hyperslab.
                hsize_t offset[RANK];
                offset[0]= i;
                offset[1]=lenghtOfExistingData;
                DataSpace fileDataSpace = audioDataset->getSpace();
                fileDataSpace.selectHyperslab(H5S_SELECT_SET, dims1, offset);

               
                 // Write the data to the hyperslab.
               
               
                audioDataset->write(mainBuffer, PredType::NATIVE_SHORT, memDataSpace, fileDataSpace);
            }
        }

        _oldPos = _oldPos+_manager.sampleRate();
        
        
        
        
        
        
        
        
        
        
        
        

        
        
        
        
        
        
        
    }

}//namespace BYB
