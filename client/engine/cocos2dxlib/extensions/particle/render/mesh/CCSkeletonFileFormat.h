
#ifndef __SkeletonFileFormat_H__
#define __SkeletonFileFormat_H__

NS_CC_BEGIN

enum SkeletonChunkID {
    SKELETON_HEADER            = 0x1000,
        // char* version           : Version number check
    SKELETON_BONE              = 0x2000,
    // Repeating section defining each bone in the system. 
    // Bones are assigned indexes automatically based on their order of declaration
    // starting with 0.

        // char* name                       : name of the bone
        // unsigned short handle            : handle of the bone, should be contiguous & start at 0
        // Vector3 position                 : position of this bone relative to parent 
        // Quaternion orientation           : orientation of this bone relative to parent 
        // Vector3 scale                    : scale of this bone relative to parent 

    SKELETON_BONE_PARENT       = 0x3000,
    // Record of the parent of a single bone, used to build the node tree
    // Repeating section, listed in Bone Index order, one per Bone

        // unsigned short handle             : child bone
        // unsigned short parentHandle   : parent bone

    SKELETON_ANIMATION         = 0x4000,
    // A single animation for this skeleton

        // char* name                       : Name of the animation
        // float length                      : Length of the animation in seconds

        SKELETON_ANIMATION_TRACK = 0x4100,
        // A single animation track (relates to a single bone)
        // Repeating section (within SKELETON_ANIMATION)
                
            // unsigned short boneIndex     : Index of bone to apply to

            SKELETON_ANIMATION_TRACK_KEYFRAME = 0x4110,
            // A single keyframe within the track
            // Repeating section

                // float time                    : The time position (seconds)
                // Quaternion rotate            : Rotation to apply at this keyframe
                // Vector3 translate            : Translation to apply at this keyframe
                // Vector3 scale                : Scale to apply at this keyframe
	SKELETON_ANIMATION_LINK         = 0x5000
	// Link to another skeleton, to re-use its animations

		// char* skeletonName					: name of skeleton to get animations from
		// float scale							: scale to apply to trans/scale keys

};

NS_CC_END


#endif
