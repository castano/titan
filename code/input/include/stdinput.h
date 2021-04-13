


class stdInput : public IInputDevice {
public:	
	stdInput();
	int Init(int flags);
	void Shut(void);
	void Refresh(void);

	int	GetValue(int device, int index);

	const input_caps *GetInputCaps(void);

private:
	int center_x, center_y;
	int	mx, my;
	int flags;
};
		