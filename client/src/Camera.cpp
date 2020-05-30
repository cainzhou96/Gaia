//
//  Camera.cpp
//  Gaia
//
//  Created by Cain on 4/25/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "Camera.hpp"
#define SENS 0.05f

Camera::Camera(glm::vec3 eyePos, glm::vec3 lookAtPos) {
    this->eyePos = eyePos;
    this->lookAtPos = lookAtPos;
    sensitivity = SENS;
    
    glm::vec3 front = glm::normalize(lookAtPos - eyePos);
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::normalize(glm::cross(right, front));
    
    this->frontVector = front;
    this->upVector = up;
    this->view = glm::lookAt(eyePos, eyePos + frontVector, upVector);

}

Camera::~Camera() {
    
}

void Camera::setLookAt(glm::vec3 pos) {
    this->lookAtPos = pos; 
    glm::vec3 front = glm::normalize(pos - eyePos);
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::normalize(glm::cross(right, front));
    
    this->frontVector = front;
    this->upVector = up;
    this->view = glm::lookAt(eyePos, eyePos + frontVector, upVector);
}

glm::mat4 Camera::getView() {
    return view;
}

glm::vec3 Camera::getPos() {
    return eyePos;
}

void Camera::setPos(glm::vec3 pos) {
    this->eyePos = pos;
    this->view = glm::lookAt(eyePos, eyePos + frontVector, upVector);
}

void Camera::rotateAround(float xAngle, float yAngle) {
    xAngle *= sensitivity;
    yAngle *= sensitivity;
    
    glm::vec3 front = glm::normalize(eyePos - lookAtPos);
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = upVector;
    
    front = glm::rotate(-xAngle, up) * glm::vec4(front, 0);
    front = glm::rotate(yAngle, right) * glm::vec4(front, 0);
    if (glm::dot(front, glm::vec3(0, 1, 0)) > 0.96) { // too close to up
        right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
        front = glm::rotate(-0.3f, right) * glm::vec4(0, 1, 0, 0); // rotate back
    }
    if (glm::dot(front, glm::vec3(0, -1, 0))  > 0.96) { // too close to opposite up
        right = glm::normalize(glm::cross(front, glm::vec3(0.0f, -1.0f, 0.0f)));
        front = glm::rotate(-0.3f, right) * glm::vec4(0, -1, 0, 0); // rotate back
    }
    //eyePos = lookAtPos + front * glm::length(eyePos - lookAtPos);

    eyePos = lookAtPos + front * 30.0f;
    
    front = glm::normalize(lookAtPos - eyePos);
    right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, front));
    
    this->frontVector = front;
    this->upVector = up;
    this->view = glm::lookAt(eyePos, eyePos + frontVector, upVector);
}

glm::vec3 Camera::getLookAtPos() {
    return lookAtPos;
}
