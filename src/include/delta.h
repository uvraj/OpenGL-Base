#pragma once

double sqr(double in) {
    return pow(in, 2.0); 
}

class DeltaKinematics
{
    public:
        DeltaKinematics(
            float baseTriSize,
            float travelTriSize,
            float activeArmLength,
            float passiveArmLength
        ) : 
            baseTriSize(baseTriSize),
            travelTriSize(travelTriSize),
            activeArmLength(activeArmLength),
            passiveArmLength(passiveArmLength)
        {

        }

        void moveToTCP(glm::vec3 inTCP) {
            TCP = inTCP;

            float E1proj_y = TCP.y - travelTriSize / (2 * sqrt(3));
            float F1_y = -baseTriSize / (2 * sqrt(3));

            float a = (-sqr(passiveArmLength) + sqr(TCP.x) + sqr(activeArmLength) + sqr(E1proj_y) + sqr(TCP.z) - sqr(F1_y)) / (2 * TCP.z);
            float b = (F1_y - E1proj_y) / TCP.z;

            float J1_y = (F1_y - a * b + sqrt(-sqr(F1_y * b + a) + sqr(activeArmLength) + sqr(activeArmLength) * sqr(b))) / (1 + sqr(b));
            float J1_z = a + b * J1_y;
        
            robotAngles.x = atan(J1_z / (F1_y - J1_y));
        }

        void executeRobotMovement() {
            // TODO: MSP430 or arduino bullshit here...
            std::cout << robotAngles.x;
            return;
        }

    private:
        glm::vec3 TCP {0};
        glm::vec3 robotAngles {0};

        float baseTriSize {0}; // f
        float travelTriSize {0}; // e
        float activeArmLength {0}; // r_f
        float passiveArmLength {0}; // r_e
};