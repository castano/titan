// TODO: actually viewport is in the camera object, but it should be a diferent object
//		 to enable multiple viewports!
//		 Camera should be asociated with an entity, that could be a player, a path ...




class Camera : public camera_s {
public:

	void Init(float fov, float w, float h, float x, float y);


public:

	//Frustum			*frustum;
};

