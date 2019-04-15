#ifndef CAMERA_H
#define CAMERA_H

#include <stdbool.h>
#include "math_linear.c"

typedef struct camera {
    vec3 position;
    vec3 right, up, forward;

    float last_x, last_y, yaw, pitch;
    float sensitivity, invert_x, invert_y;
    float enabled;

    mat44 view;
} camera;

API void camera_create(camera *cam, float sensitivity, bool invert_x, bool invert_y);
API void camera_enable(camera *cam, bool enabled);

API void camera_fps(camera *cam, vec3 keyboard, vec2 mouse);
API void camera_orbit(camera *cam, float distance, vec2 mouse);

#endif


#ifdef CAMERA_C
#pragma once

void camera_create(camera *cam, float sensitivity, bool invert_x, bool invert_y) {
    camera z = {0};
    *cam = z;

    cam->position = vec3(0, 0, -3);

    cam->right = vec3(1, 0, 0);
    cam->up = vec3(0, 1, 0);
    cam->forward = vec3(0, 0, 1);

    cam->yaw = 90;

    cam->sensitivity = sensitivity;
    cam->invert_x = invert_x ? -1 : 1;
    cam->invert_y = invert_y ? -1 : 1;
    cam->enabled = 1;

    identity44(cam->view);
}

void camera_enable(camera *cam, bool enabled) {
    cam->enabled = enabled;
}

void camera_fps(camera* cam, vec3 keyboard, vec2 mouse) {
    // look: update angles
    float offset_x = ( mouse.x - cam->last_x ) * cam->sensitivity * cam->invert_x;
    float offset_y = ( mouse.y - cam->last_y ) * cam->sensitivity * cam->invert_y;

    cam->last_x = mouse.x;
    cam->last_y = mouse.y;

    if( cam->enabled ) {
        cam->yaw += offset_x;
        cam->pitch += offset_y;

        // look: limit pitch angle [-89..89]
        cam->pitch = cam->pitch > 89 ? 89 : cam->pitch < -89 ? -89 : cam->pitch;

        // look: update forward vector
        cam->forward = norm3( vec3(
            cos(rad(cam->yaw)) * cos(rad(cam->pitch)),
            sin(rad(cam->pitch)), 
            sin(rad(cam->yaw)) * cos(rad(cam->pitch))
        ) );

        // look: recompute right vector
        cam->right = norm3(cross3(cam->forward, cam->up));

        // move: offset position
        float right = keyboard.x, top = keyboard.y, front = keyboard.z;
        cam->position = add3(cam->position, mul3(cam->forward, vec3(front, front, front)));
        cam->position = add3(cam->position, mul3(cam->up, vec3(top, top, top)));
        cam->position = add3(cam->position, mul3(cam->right, vec3(right, right, right)));
    }

    // compute view matrix
    lookat44(cam->view, cam->position, add3(cam->position, cam->forward), cam->up);
}

void camera_orbit( camera *cam, float distance, vec2 mouse ) {
    // look: update angles
    float offset_x = ( mouse.x - cam->last_x ) * cam->sensitivity * cam->invert_x;
    float offset_y = ( mouse.y - cam->last_y ) * cam->sensitivity * cam->invert_y;

    cam->last_x = mouse.x;
    cam->last_y = mouse.y;

    if( cam->enabled ) {
        cam->yaw += offset_x;
        cam->pitch += offset_y;

        // look: limit pitch angle [-89..89]
        cam->pitch = cam->pitch > 89 ? 89 : cam->pitch < -89 ? -89 : cam->pitch;
    }

    // compute view matrix
    float x = rad(cam->yaw), y = rad(cam->pitch), cx = cosf(x), cy = cosf(y), sx = sinf(x), sy = sinf(y);
    lookat44(cam->view, vec3( cx*cy*distance, sy*distance, sx*cy*distance ), vec3(0,0,0), vec3(0,1,0) );
}

// ideas

void camera_lerp();

#endif
