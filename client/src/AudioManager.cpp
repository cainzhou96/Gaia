//
//  AudioManager.cpp
//  Gaia
//
//  Created by Marino Wang on 5/15/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "AudioManager.hpp"

#define SOUND_DIR_PATH "audio/sound/"


const std::string AudioManager::BACKGROUND_MUSIC1 = "audio/music/background1.wav";
const std::string AudioManager::SF1POINTS = "audio/sound/points.wav";
const std::string AudioManager::SF2HIT = "audio/sound/hit.wav";



AudioManager::AudioManager(){
    volumeControl = 1;
    defaultSFVolume = 100.0f;
    defaultMusicVolume = 0.0f;

    
    for(int i=0; i<128; i++){
        soundList.push_back(sf::Sound());
    }
    
    // Load SFX to memory
    LoadSounds();
    
    if(!music.openFromFile(BACKGROUND_MUSIC1)){
        std::cerr << "Failed to load background music file" << std::endl;
    }
    music.setVolume(defaultMusicVolume);
    music.setLoop(true);
    
    music.play();
    
}


void AudioManager::PlaySounds(int index){
    switch(index){
        case 0:
            soundList[index].setBuffer(soundBuffer[SF1POINTS]);
            soundList[index].setVolume(defaultSFVolume);
            VolumeControl();
            soundList[index].setLoop(false);
            soundList[index].play();
            break;
        case 1:
            soundList[index].setBuffer(soundBuffer[SF2HIT]);
            soundList[index].setVolume(defaultSFVolume);
            VolumeControl();
            soundList[index].setLoop(false);
            soundList[index].play();
            break;
    }
}

void AudioManager::LoadSounds() {
    // Load sound effects into memory
    boost::filesystem::path path = boost::filesystem::path(SOUND_DIR_PATH);
    
    for (auto &entry : boost::make_iterator_range(boost::filesystem::directory_iterator(path), {})) {
        std::string filename = SOUND_DIR_PATH + entry.path().filename().string();
        if (!soundBuffer[filename].loadFromFile(filename)) {
            std::cerr << "AudioManager: Cannot open " << filename << std::endl;
        }
    }
}

void AudioManager::VolumeControl(){
    switch(volumeControl){
        case 0:
            music.setVolume(0.0f);
            for(int i=0; i<soundList.size(); i++){
                soundList[i].setVolume(0.0f);
            }
            break;
            
        case 1:
            music.setVolume(defaultMusicVolume);
            for(int i=0; i<soundList.size(); i++){
                soundList[i].setVolume(defaultSFVolume);
            }
            break;
            
    }
}

