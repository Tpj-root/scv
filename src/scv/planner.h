#ifndef SCV_PLANNER_H
#define SCV_PLANNER_H

#include <vector>
#include "vec3.h"

namespace scv {

    // A constant jerk portion of the trajectory, defined by an initial pos/vel/acc, and a jerk with duration
    struct segment {

        vec3 pos;
        vec3 vel;
        vec3 acc;
        vec3 jerk;
        double duration;

        bool toDelete;

        segment() {
            toDelete = false;
        }
    };

    enum cornerBlendMethod {
        CBM_NONE,
        CBM_CONSTANT_JERK_SEGMENTS,
        CBM_INTERPOLATED_MOVES
    };

    enum cornerBlendType {
        CBT_NONE,
        CBT_MIN_JERK,
        CBT_MAX_JERK
    };

    // A single point to point movement
    struct move {
        vec3 src;
        vec3 dst;

        // constraints on the movement
        double vel;
        double acc;
        double jerk;
        cornerBlendType blendType;
        scv_float blendClearance;

        std::vector<segment> segments;

        float duration;
        float scheduledTime;
        int traversal_segmentIndex;
        scv_float traversal_segmentTime;

        move() {
            src = vec3_zero;
            dst = vec3_zero;
            vel = 0;
            acc = 0;
            jerk = 0;
            blendType = CBT_MAX_JERK;
            blendClearance = -1; // none
        }
    };

    class planner
    {
    public: // Typically these would be private, they are public here for convenience in the visualizer
        cornerBlendMethod blendMethod;
        vec3 posLimitLower;
        vec3 posLimitUpper;
        vec3 velLimit;
        vec3 accLimit;
        vec3 jerkLimit;

        std::vector<move> moves;
        std::vector<segment> segments;

        int traversal_segmentIndex;
        scv_float traversal_segmentTime;

        vec3 traversal_pos;
        scv_float traversal_time;

        void calculateMove(move& m);
        void calculateSchedules();
        void blendCorner(move& m0, move& m1, bool isFirst, bool isLast);
        void collateSegments();
        void getSegmentState(segment& s, double t, vec3* pos, vec3* vel, vec3* acc, vec3* jerk );
        //void getSegmentPosition(segment& s, double t, scv::vec3* pos);
        std::vector<segment>& getSegments();

        bool advanceTraverse_constantJerkSegments(scv_float dt, vec3* p);
        bool advanceTraverse_interpolatedMoves(scv_float dt, vec3* p);

        scv_float getTraverseTime_constantJerkSegments();
        scv_float getTraverseTime_interpolatedMoves();

    // The actual public part would normally start from here
    public:
        planner();

        void clear();

        void setCornerBlendMethod(cornerBlendMethod m);

        void setPositionLimits(scv_float lx, scv_float ly, scv_float lz, scv_float ux, scv_float uy, scv_float uz);
        void setVelocityLimits(scv_float x, scv_float y, scv_float z);
        void setAccelerationLimits(scv_float x, scv_float y, scv_float z);
        void setJerkLimits(scv_float x, scv_float y, scv_float z);

        void appendMove( move& l );
        bool calculateMoves();
        bool getTrajectoryState_constantJerkSegments(scv_float time, int *segmentIndex, vec3* pos, vec3* vel, vec3* acc, vec3* jerk );
        bool getTrajectoryState_interpolatedMoves(scv_float time, int *segmentIndex, vec3* pos, vec3* vel, vec3* acc, vec3* jerk );

        scv_float getTraverseTime();

        void resetTraverse();
        bool advanceTraverse(scv_float dt, vec3* p);

        void printConstraints();    // print global limits for each axis
        void printMoves();          // print input parameters for each point to point move
        void printSegments();       // print calculated parameters for each segment
    };

} // namespace

#endif
