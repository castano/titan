

typedef enum {
	OB_NONE,
	OB_Q2BSP,
	OB_Q3BSP,
	OB_LANDSCAPE,
	OB_MD3,
	OB_3DS,
	OB_MDL,
	OB_BEZIERPATCH,
}OBJECT_TYPE;




class Object {
public:
	virtual void Init(void)=0;
	virtual void Update(void)=0;
	virtual void Render(void)=0;
	virtual OBJECT_TYPE Id(void)=0;

};

