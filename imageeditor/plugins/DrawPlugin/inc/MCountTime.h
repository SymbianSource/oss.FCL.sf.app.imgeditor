#ifndef MCOUNTTIME_H_
#define MCOUNTTIME_H_

class MCountTime
	{
public:
	virtual void Notify() = 0;
	virtual ~MCountTime() {}
protected:
	MCountTime() {}
	};

#endif
