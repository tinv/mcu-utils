
#ifndef MU_BUS_FAKE_H
#define MU_BUS_FAKE_H

#include <stdint.h>
#include <bus/bus.h>
#include <vector>

class MuBusInterface
{
      public:

	virtual ~MuBusInterface() {}

	virtual int init() = 0;
	virtual int publish(const struct zbus_channel *chan, const void* msg) = 0;
	virtual int addObserver(const struct zbus_channel *chan, const struct zbus_observer* obs,
		struct zbus_observer_node* node) = 0;
	virtual int removeObserver(const struct zbus_channel *chan,
		const struct zbus_observer *obs) = 0;
	virtual int read(const struct zbus_channel *chan, void* msg) = 0;
	virtual const void* constMessage(const struct zbus_channel *chan) = 0;

};

class MuBusFake : public MuBusInterface
{
      public:
	MuBusFake();
	virtual ~MuBusFake()
	{
		mObservers.clear();
	}

	int init() override;
	int publish(const struct zbus_channel *chan, const void* msg) override;
	int addObserver(const struct zbus_channel *chan, const struct zbus_observer* obs,
		struct zbus_observer_node* node) override;
	int removeObserver(const struct zbus_channel *chan, const struct zbus_observer *obs) override;
	int read(const struct zbus_channel *chan, void* msg) override;
	const void* constMessage(const struct zbus_channel *chan) override;

      private:

	std::vector<struct zbus_observer_node*> mObservers;

};

extern MuBusFake* MuBusFakeObj;
extern struct mu_bus_if muBusFake;

#endif // MU_BUS_FAKE_H
