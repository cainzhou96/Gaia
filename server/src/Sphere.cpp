//
//  Sphere.cpp
//  Gaia
//
//  Created by Wenlin Mao on 4/18/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "Sphere.h"
#include "PhysicsConstant.h"
#define AIRBOURNE 0

Sphere::Sphere(){
}

Sphere::Sphere(float m, float r) : Primitive(m), radius(r) {
    mass = m;
    I0 = glm::mat3(0.0f);
    I0[0][0] = 2 * mass * radius * radius / 5;
    I0[1][1] = 2 * mass * radius * radius / 5;
    I0[2][2] = 2 * mass * radius * radius / 5;
}

Sphere::Sphere(float mass, const glm::vec3& pos, const glm::vec3& vel, float r) : Primitive(mass, pos, vel), radius(r) {
    this->mass = mass;
    I0 = glm::mat3(0.0f);
    I0[0][0] = 2 * mass * radius * radius / 5;
    I0[1][1] = 2 * mass * radius * radius / 5;
    I0[2][2] = 2 * mass * radius * radius / 5;
}

Sphere::~Sphere(){
}

void Sphere::computeAreoForce(Wind* wind){
    
}

void Sphere::setRadius(float r){
    radius = r;
}

/*
 * a, b, c forms a triangle and n is normal. Return a vector to translate the sphere to just
 * resolve intersection. Return vec3(0) if no collision.
 */
bool Sphere::checkCollision(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 n, float elapsedTime) {
    position = (glm::vec3)model[3];
    if (a == b || a == c || b == c) {
        return false; 
    }

    float t = -(glm::dot(a, n) - glm::dot(position, n));
    if (t < -(radius + EPSILON) || t >(radius + EPSILON)) {
        return false; 
    }

    glm::vec3 P = position + t * (-n);

    glm::vec3 e1 = b - a;
    glm::vec3 e2 = c - b;
    glm::vec3 e3 = a - c;
    glm::vec3 A = P - a;
    glm::vec3 B = P - b;
    glm::vec3 C = P - c;

    //if P is inside triangle
    if (glm::dot(n, glm::cross(e1, A)) >= 0 &&
        glm::dot(n, glm::cross(e2, B)) >= 0 &&
        glm::dot(n, glm::cross(e3, C)) >= 0) {
        glm::vec3 result;
        glm::vec3 pointPos;
        if (t > 0) {
            result = P - (position + radius * glm::normalize(P - position));
            pointPos = position + radius * glm::normalize(P - position);
        }
        else {
            result = P - (position + radius * glm::normalize(position - P));
            pointPos = position + radius * glm::normalize(position - P);
        }
        // collision
        glm::mat3 I = glm::mat3(model) * I0 * glm::transpose(glm::mat3(model));
        glm::vec3 omega = glm::inverse(I) * angMomentum;
        glm::vec3 velocity = (momentum + moveMomentum) / mass;
        glm::vec3 r = pointPos - position;
        glm::vec3 vr = velocity + glm::cross(omega, r);
        float e = RESTITUTION;
        glm::vec3 impulse = (1 + e) * (fmax(glm::dot(vr, -n), 0.0f) / (1 / mass + glm::dot(glm::inverse(I) * (glm::cross(glm::cross(r, n), r)), n))) * n; // with bounce
        //glm::vec3 impulse = (fmax(glm::dot(vr, -n), 0.0f) / (1/mass + glm::dot(glm::inverse(I) * (glm::cross(glm::cross(r, n), r)), n))) * n; // without bounce
        // glm::vec3 impulse = fmax(glm::dot(vr, -n), 0.0f) * mass * n;
        glm::vec3 reactionForce = impulse / elapsedTime;
        // std::cout << "reaction force: " << glm::to_string(reactionForce) << std::endl;
        applyForce(reactionForce, pointPos);

        // friction
        if (glm::length(vr) < EPSILON && glm::length(vr) > -EPSILON) { // is 0
            move(position + result); // move to right position
            return true; 
        }
        float ud = UD;
        float us = US;
        float fd = ud * glm::length(reactionForce);
        float fs = us * glm::length(reactionForce);

        glm::vec3 t;
        if (glm::dot(vr, n) > EPSILON || glm::dot(vr, n) < -EPSILON) { // not 0
            t = vr - glm::dot(vr, n) * n;
            if (glm::length(t) > 0) {
                t = normalize(t);
            }
        }
        else if (glm::dot(force, n) > EPSILON || glm::dot(force, n) < -EPSILON) { // not 0
            t = glm::normalize(force - glm::dot(force, n) * n);
            if (glm::length(t) > 0) {
                t = normalize(t);
            }
        }
        else {
            t = glm::vec3(0);
        }
        glm::vec3 ff;
        if ((glm::dot(vr, t) < EPSILON && glm::dot(vr, t) > -EPSILON) && glm::dot(force, t) <= fs) {
            // vr*t is 0 and force*t <= fs
            ff = -glm::dot(force, t) * t;
        }
        else {
            ff = -fd * t;
        }
        if (glm::length(ff * elapsedTime / mass) > glm::length(vr - glm::dot(vr, n) * n)) {
            ff = -(vr - glm::dot(vr, n) * n) * mass / elapsedTime;
        }
        //std::cout << glm::to_string(ff) << std::endl;
        applyForce(ff, pointPos);
        
        move(position + result); // move to right position

        return true;
    }
    else {
        return false;
    }
}

// translate to pos
void Sphere::move(const glm::vec3& pos){
    model[3] = glm::vec4(pos, 1);
    position = pos;
}

void Sphere::applyForce(glm::vec3 f, glm::vec3 pos) {
    if (glm::length(f) > 0) {
        force += f;
        glm::vec3 r = pos - position;
        if (glm::length(r) > 0) {
            torque += glm::cross(r, f);
        }
    }
}

void Sphere::applyMoveForce(glm::vec3 f, glm::vec3 pos) {
    if (glm::length(f) > 0) {
        moveForce += f;
        glm::vec3 r = pos - position;
        if (glm::length(r) > 0) {
            torque += glm::cross(r, f);
        }
    }
}

void Sphere::updatePosition(float elapsedTime, bool hit) {
    momentum += force * elapsedTime;
    if (AIRBOURNE) {
        moveMomentum += moveForce * elapsedTime;
    }
    else {
		if (hit) {
			moveMomentum += moveForce * elapsedTime;
		}
    }
    if (glm::dot(momentum, moveMomentum) < 0) {
        moveMomentum += glm::dot(glm::normalize(momentum), moveMomentum) * glm::normalize(moveMomentum);
        momentum -= glm::dot(glm::normalize(momentum), moveMomentum) * glm::normalize(moveMomentum);
    }
    glm::vec3 dis = (momentum / mass) * elapsedTime;

    if (glm::length(moveMomentum) > 0) {
        glm::vec3 temp = SPEED * glm::normalize(moveMomentum) * elapsedTime;
        if (glm::length(temp) >= glm::length(moveMomentum)) {
            moveMomentum = glm::vec3(0);
        }
        else {
            moveMomentum -= temp;
        }
    }
    if (glm::length(moveMomentum) > SPEED) {
        moveMomentum = SPEED * glm::normalize(moveMomentum);
    }
    dis += (moveMomentum / mass) * elapsedTime;
    // std::cout << glm::to_string(dis) << std::endl;

    move(getCenter() + dis);
}

void Sphere::updateOrientation(float elapsedTime) {
    angMomentum += torque * elapsedTime;
    glm::mat3 I = glm::mat3(model) * I0 * glm::transpose(glm::mat3(model));
    glm::vec3 omega = glm::inverse(I) * angMomentum;
    float angle = glm::length(omega) * elapsedTime; // magnitude of omega
    if (angle != 0.0f) {
        glm::vec3 axis = omega;
        axis = glm::normalize(axis);
        // model = glm::rotate(model, angle, axis);
        model = glm::rotate(glm::mat4(1), angle, axis) * model;
        model[3] = glm::vec4(position, 1.0f); 
    }
}
