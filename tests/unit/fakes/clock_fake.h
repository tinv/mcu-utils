
#ifndef MU_CLOCK_FAKE_H
#define MU_CLOCK_FAKE_H

#include <stdint.h>

class MuClockInterface
{
      public:

	virtual ~MuClockInterface() {}
	virtual void init() = 0;
	virtual uint64_t nowMs() const = 0;
	virtual void advance(uint64_t ms) = 0;
};

class MuClockFake : public MuClockInterface
{
      public:
	MuClockFake();
	virtual ~MuClockFake() {};
	void init() override;
	uint64_t nowMs() const override;
	void advance(uint64_t ms) override;

      private:

	uint64_t mCurrentTime;

};


#endif // MU_CLOCK_FAKE_H
