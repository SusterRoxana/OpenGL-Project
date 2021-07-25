#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraUpDirection = cameraUp;
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
        // return glm::mat4();
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        switch (direction)
        {
        case MOVE_FORWARD:
            if (cameraPosition.y < 1.0)
            {
                cameraPosition.y = 1.0;
            }
            else
            {
                cameraPosition += cameraFrontDirection * speed;
            }
            
            break;

        case MOVE_BACKWARD:
            cameraPosition -= cameraFrontDirection * speed;
            break;

        case MOVE_RIGHT:
            cameraPosition += cameraRightDirection * speed;
            break;

        case MOVE_LEFT:
            cameraPosition -= cameraRightDirection * speed;
            break;
        }
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        cameraFrontDirection = glm::normalize(front);


    }
}